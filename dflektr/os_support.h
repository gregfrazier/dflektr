#pragma once

//#define _WIN32_GUI

#define MAX(a,b) (((a)>(b))?(a):(b))

// Packet Buffer Size
#define BUFFER_SIZE 4096

#ifndef _WIN32
	#include <unistd.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <sys/wait.h>
	#include <arpa/inet.h>
	#include <stdlib.h>

	typedef unsigned int SOCKET;
	typedef struct timeval TIMEVAL;
	#define recv(s, buf, len, flags) read(s, buf, len)
	#define send(s, buf, len, flags) write(s, buf, len)
	#define _strdup(s1) strdup(s1)
	#define SD_BOTH 2
	#define SD_SEND 1
	#define SD_RECEIVE 0
	#define closesocket(fd) close(fd)
	#define SOCKET_ERROR (-1)
	#define INVALID_SOCKET (SOCKET)(~0)
#else
	//#include <windows.h>	
	#include <stdlib.h>	
	#include <string.h>
	#include <tchar.h>
#endif

#ifdef __APPLE__
	#include <cstdlib>
#endif

#ifdef WAIT_INTERVENTION
	extern HANDLE ghMutex;
#endif
