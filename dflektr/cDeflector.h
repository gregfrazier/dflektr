#pragma once
#include "os_support.h"
#include <string.h>

class cDeflector
{
public:
	cDeflector(SOCKET cli_socket, struct sockaddr_in targ, struct sockaddr_in cli);
	~cDeflector(void);
	static void AcceptThread(void* p);
	void connectTarget();
	void reflect();
	void forwardMessage(fd_set* readFD, SOCKET* source, SOCKET* dest);
private:
	SOCKET target_socket;
	SOCKET client_socket;
	struct sockaddr_in target;
	struct sockaddr_in client;

	bool valid;
};

