#pragma once
#ifdef _WIN32
	#include <WinSock2.h>
	#include <process.h>
#endif
#include <stdio.h>
#include <vector> // need to check if this is standard
#include "os_support.h"
#include "cDeflector.h"

class cListener
{
public:
	cListener(char *targ_address, int targ_port, bool use_ssh = false, bool verbose = false);
	~cListener(void);
	void startListener(int listen_port);
	bool isStateValid() { return valid; };
private:
	// Server reflecting to
	struct sockaddr_in target_svr;
	//char *target_svr_ip;

	// The proxy (the deflector)
	struct sockaddr_in local_svr;
	//char *local_svr_ip;

	SOCKET local_socket;

	bool valid;
	bool ssh;
#ifdef _WIN32
	WSADATA wsaData;
#endif

	// Pool of Clients
	std::vector<cDeflector*> clients;

	void bindsocket(int listen_port);
	void acceptConn();
};

