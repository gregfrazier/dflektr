#include "cListener.h"

//#define LISTEN_PORT 1337

cListener::cListener(char *targ_address, int targ_port, bool use_ssh, bool verbose)
{
	struct hostent *h;

	this->valid = true;
	this->ssh = use_ssh;

#ifdef _WIN32
	int rst;
	rst = WSAStartup(MAKEWORD(2, 2), &wsaData); // Use winsock 2.2
	if(rst != 0){
		this->valid = false;
		fprintf(stderr, "WSAStartup Failed: %d\n", rst);
		return;
	}
#endif

	this->target_svr.sin_family = AF_INET;      //internetwork: UDP, TCP, etc.

	fprintf(stderr, "Target Address: %s\n", targ_address);
	h = gethostbyname(targ_address);
	if(h == NULL)
	{
		this->valid = false;
		fprintf(stderr, "gethostbyname Failed\n");
		return;
	}
	memcpy(&target_svr.sin_addr, h->h_addr_list[0], h->h_length);

	target_svr.sin_port = htons(targ_port);     //converts ushort to big endian format for TCP

	//target_svr_ip = _strdup(inet_ntoa(target_svr.sin_addr));
}
/***************************************************************************************************************************/
cListener::~cListener(void)
{
	shutdown(local_socket, SD_BOTH); // shutdown both RECV and SEND commands, since the system is deconstructing
	closesocket(local_socket);
#ifdef _WIN32
	WSACleanup();
#endif
}
/***************************************************************************************************************************/
void cListener::startListener(int listen_port)
{
	bindsocket(listen_port);

	int listenError = listen(local_socket, SOMAXCONN);
	if(listenError == SOCKET_ERROR)
	{
#ifdef _WIN32
		fprintf(stderr, "Sockets Error: Cannot listen on created socket: %i\n", WSAGetLastError());
#else
		fprintf(stderr, "Sockets Error: Cannot listen on created socket\n");
#endif
		listenError = closesocket(local_socket);		
		if(listenError == SOCKET_ERROR)
			fprintf(stderr, "Sockets Error: Cannot close socket\n");
#ifdef _WIN32
		WSACleanup();
#endif
		this->valid = false;
		return;
	}

	fprintf(stderr, "Listening on port %i...\n", listen_port);

	for(;;) // Run forever, need to write in some code to allow this be closed.
	{
		//fprintf(stderr, "GetAsyncKeyState: %i\n", GetAsyncKeyState(0x58));
		acceptConn(); // This is blocking and will only loop when it receives a request for a new connection
	}
}
/***************************************************************************************************************************/
void cListener::acceptConn()
{
	SOCKET clientSocket;
	struct sockaddr_in client_info;
	int client_info_len = sizeof(client_info); // uses it has a pointer, so need to create it here

#ifdef _WIN32
	clientSocket = accept(local_socket, (struct sockaddr*) &client_info, &client_info_len);
#else
	clientSocket = accept(local_socket, (struct sockaddr*) &client_info, (socklen_t*) &client_info_len);
#endif
	if(clientSocket == INVALID_SOCKET)
	{
		fprintf(stderr, "Sockets Error: Cannot accept client connection request\n");
		clientSocket = closesocket(local_socket);		
		if(clientSocket == SOCKET_ERROR)
			fprintf(stderr, "Sockets Error: Cannot close socket\n");
#ifdef _WIN32
		WSACleanup();
#endif
		this->valid = false;
		return;
	}
	fprintf(stderr, "Client %s:%d connected\n", inet_ntoa(client_info.sin_addr), client_info.sin_port);

	// Fire off cDeflector under another thread
	cDeflector *child = new cDeflector(clientSocket, target_svr, client_info);
	
	// Store it
	this->clients.push_back(child);

	// Prune the list (do this later...)
	//closesocket(clientSocket);
}
/***************************************************************************************************************************/
void cListener::bindsocket(int listen_port)
{
	this->local_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP); //IPPROTO_TCP); // Might be IPPROTO_IP
	if(local_socket == INVALID_SOCKET)
	{
		fprintf(stderr, "Sockets Error: Cannot create a socket\n");
#ifdef _WIN32
		WSACleanup();
#endif
		this->valid = false;
		return;
	}
	
	memset(&local_svr, 0, sizeof(local_svr));           // Zero out struct
	local_svr.sin_family = AF_INET;                     // internetwork: UDP, TCP, etc.

	// This causes problems with multiple active network adapters, might want to investigate getaddrinfo
	// http://www.beej.us/guide/bgnet/output/html/singlepage/bgnet.html#bind
#ifdef _WIN32
	local_svr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // Any incoming interface (0.0.0.0)
#else
	local_svr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

	local_svr.sin_port = htons(listen_port);

	// Allows binding to a port that already is in use:
	//setsockopt(local_socket, SOL_SOCKET, SO_REUSEADDR, ???, ???);
	
	int bindFailure = bind(local_socket, (struct sockaddr *) &local_svr, sizeof(local_svr));
	if(bindFailure == SOCKET_ERROR)
	{
		fprintf(stderr, "Sockets Error: Cannot bind to socket\n");
		
		bindFailure = closesocket(local_socket);		
		if(bindFailure == SOCKET_ERROR)
			fprintf(stderr, "Sockets Error: Cannot close socket\n");

#ifdef _WIN32
		WSACleanup();
#endif
		this->valid = false;
		return;
	}
	return;
}
