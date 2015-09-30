#ifdef _WIN32
#include <WinSock2.h>
#include <process.h>
#include <conio.h>
#endif

#include "cDeflector.h"
#include <stdio.h>
#include <errno.h>
#include "cAnsiUtil.h"

cDeflector::cDeflector(SOCKET cli_socket, struct sockaddr_in targ, struct sockaddr_in cli) : valid(true)
{
	// fill information
	memcpy(&this->client, &cli, sizeof(cli));
	memcpy(&this->target, &targ, sizeof(targ));
	this->client_socket = cli_socket;

	// Thread it
#ifdef _WIN32
	_beginthread(cDeflector::AcceptThread, 0, (void*)this);
#else
	// Need to double fork to not take up system PIDs and avoid zombie processes
	pid_t pidChild;
	pid_t pidGrandChild;

	pidChild = fork();

	if(pidChild == -1)
	{
		// Failure
		fprintf(stderr, "Forking Failed\n");
		return;
	}else if(pidChild == 0){
		// Child
		;
	}else{
		// Parent
		int status;
		waitpid(pidChild, &status, NULL);
		return;
	}

	// Only child can get here; now fork it

	pidGrandChild = fork();

	if(pidGrandChild == -1)
	{
		// Failure
		fprintf(stderr, "Forking Failed\n");
		exit(0);
		return;
	}else if(pidGrandChild == 0){
		// Grandchild
		;
	}else{
		// Man-Child, exit immediately
		exit(0);
		return;
	}

	// Only Grand-child can get here
	this->connectTarget();
#endif
}
/***************************************************************************************************************************/
cDeflector::~cDeflector(void)
{
}
/***************************************************************************************************************************/
void cDeflector::AcceptThread(void* p)
{
	cDeflector *self = (cDeflector *)p;
	self->connectTarget(); // Connect to target and reflect incoming from client to target.
}
/***************************************************************************************************************************/
void cDeflector::connectTarget()
{
	// Create socket to connect to server with
	this->target_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP); //IPPROTO_TCP);
	if(target_socket == INVALID_SOCKET)
	{
		fprintf(stderr, "Sockets Error: Cannot create a socket\n");
		this->valid = false;
		return;
	}

	// Connect to the server for relay
	int connectFailure = connect(target_socket, (struct sockaddr*) &target, sizeof(target));
	if(connectFailure == SOCKET_ERROR)
	{
#ifdef _WIN32
		fprintf(stderr, "Sockets Error: Cannot connect to target: %s; %i\n", inet_ntoa(target.sin_addr), WSAGetLastError());
#else
		fprintf(stderr, "Sockets Error: Cannot connect to target\n");
#endif
		
		connectFailure = closesocket(target_socket);		
		if(connectFailure == SOCKET_ERROR)
			fprintf(stderr, "Sockets Error: Cannot close target socket\n");
		this->valid = false;
		return;
	}
	fprintf(stderr, "\n======================================\n");
	fprintf(stderr, "Deflecting %s:%d ", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
	fprintf(stderr, "to %s:%d\n", inet_ntoa(target.sin_addr), ntohs(target.sin_port));
	fprintf(stderr, "======================================\n");
	// Start Reflection
	this->reflect();
}
/***************************************************************************************************************************/
void cDeflector::reflect()
{
	fd_set mainFD = { 0 };
	fd_set readFD = { 0 };
	TIMEVAL t;
	SOCKET fd;
	int socketStatus;

#ifdef WAIT_INTERVENTION
	DWORD dwWaitResult;
	int ch;
#endif

#ifdef VERBOSE_OUTPUT
	char modBuf[BUFFER_SIZE + 8];
	int modBufSize = BUFFER_SIZE + 1; // For the terminator
#endif

	// Maximum Wait (I think)
	t.tv_sec = 448;
	t.tv_usec = 0;

	// Sockets are identified by a integer, so need to get the biggest one -- only for OSX/BSD.
	fd = MAX(this->client_socket, this->target_socket) + 1;

	FD_ZERO(&mainFD);                     // Initializes the set
	FD_SET(this->client_socket, &mainFD); // Adds Client Socket to set
	FD_SET(this->target_socket, &mainFD); // Adds Target Socket to set

	while(this->valid)
	{
		// select destroys the FD_SET, instead of recreating, we copy it
		memcpy(&readFD, &mainFD, sizeof(mainFD));

		// Query the sockets and see which ones have data to read (blocks)
		socketStatus = select(fd,         // ignored on Windows/Linux, required for BSD/OSX
			                  &readFD,    // Ready to read?
							  NULL,       // Ready to write?, ignored
							  NULL,       // Errors pending?, ignored
							  &t);        // timeout
		if(socketStatus == SOCKET_ERROR)
		{
			fprintf(stderr, "Sockets Error: Could not query the socket status\n");

			switch(errno){
				case EBADF:
					fprintf(stderr, "Sockets Error: Not a valid open socket\n");
					break;
				case EINTR:
					fprintf(stderr, "Sockets Error: Select was interrupted.\n");
					break;
				case EINVAL:
					fprintf(stderr, "Sockets Error: Invalid Argument(s).\n");
					break;
			}
		
			socketStatus = closesocket(client_socket);		
			if(socketStatus == SOCKET_ERROR)
				fprintf(stderr, "Sockets Error: Cannot close client socket\n");

			socketStatus = closesocket(target_socket);		
			if(socketStatus == SOCKET_ERROR)
				fprintf(stderr, "Sockets Error: Cannot close target socket\n");

			this->valid = false;
		}
		// Forward Client
		forwardMessage(&readFD, &(this->client_socket), &(this->target_socket));
		// Forward Server
		forwardMessage(&readFD, &(this->target_socket), &(this->client_socket));
	}
	fprintf(stderr, "\n\nCLOSING SOCKET FROM CLIENT / TO SERVER\n");
	shutdown(this->client_socket, SD_RECEIVE); // Shuts down the RECV command for this socket.
	shutdown(this->target_socket, SD_RECEIVE);
	closesocket(this->client_socket);
	closesocket(this->target_socket);

	// Close the thread
#ifndef _WIN32
	exit(0);
#else
	_endthread();
#endif
}
/***************************************************************************************************************************/
void cDeflector::forwardMessage(fd_set* readFD, SOCKET* source, SOCKET* dest)
{
//	int socketStatus;
	int bytesRcv;

	#ifdef WAIT_INTERVENTION
		DWORD dwWaitResult;
		int ch;
	#endif

	char recvBuf[BUFFER_SIZE];
	int recvBufSize = BUFFER_SIZE;

	// Check if the socket is yeilding data
	if(FD_ISSET(*source, readFD))
	{
		// Client is sending data, read it.
#ifdef WAIT_INTERVENTION
		dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
		if(dwWaitResult == WAIT_OBJECT_0){
			printf("Read from source? ");
			ch = 'y'; //_getch();			
			if(ch == 'y'){
				printf("Y\n");
#endif
				bytesRcv = recv(*source, recvBuf, recvBufSize, 0);
				if(bytesRcv == SOCKET_ERROR || bytesRcv <= 0)
				{
					fprintf(stderr, "Sockets Error: Nothing was retrieved from Source\n");
					this->valid = false;
				}
				
				// Manipulation of data can be hooked here
				
				// Forward to the target
#ifdef WAIT_INTERVENTION
				printf("Send from source to dest? ");
				ch = 'y'; //_getch();			
				if(ch == 'y'){
					printf("Y\n");
#endif
					if(send(*dest, recvBuf, bytesRcv, 0) != bytesRcv)
					{
						fprintf(stderr, "Sockets Error: Destination buffer length did not equal Source length\n");
						this->valid = false;
					}
#ifdef WAIT_INTERVENTION
				}else{
					printf("N\n");
				}
			}else if(ch == 'x'){
				printf("X\n");
				this->valid = false;
			}else{
				printf("N\n");
			}
		}
#endif
	}
}