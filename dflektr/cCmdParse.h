#pragma once
class cCmdParse
{
public:
	cCmdParse(char* arguments[], int argCount);
	~cCmdParse(void);

	char *destIPAddress;
	int destPort;
	int srcPort;
	bool useSSH;

	bool badArguments;
	bool verbose;
	bool hasDest;
};

