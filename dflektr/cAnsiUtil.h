#pragma once
class cAnsiUtil
{
public:
	static char *ignoreTerminate(char *in, int bufSize);
	static char *removeANSICodes(char *in, int bufSize);
	static char *removeNonDisplayAble(char *in, int bufSize);
	static bool isCSICommand(char in);
};

