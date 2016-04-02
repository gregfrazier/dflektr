#include "os_support.h"
#include <time.h>
#include "cListener.h"
#include "cCmdParse.h"
//#include "cWind.h"

#ifdef WAIT_INTERVENTION
HANDLE ghMutex;
#endif

void printHelp(char *i);

//#ifndef _WIN32_GUI
int main( int argc, char* argv[] )
//#else
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//#endif
{
#ifdef WAIT_INTERVENTION
	ghMutex = CreateMutex(NULL, FALSE, NULL);
#endif

//#ifndef _WIN32_GUI
	cCmdParse *p = new cCmdParse(argv, argc);

	if(p->badArguments)
	{
		printHelp(argv[0]);
		exit(0);
	}

	cListener *c = new cListener(p->destIPAddress, p->destPort, p->useSSH);
	c->startListener(p->srcPort);
	
	// Sadly, this never happens.
	delete(c);
	delete(p);
	exit(0);
//#else
//	// Create the GUI window and let it go from there.
//	if (SUCCEEDED(CoInitialize(NULL))) {
//		cWind *wnd = new cWind();
//		wnd->InitWnd("dflektr-win", 253, 96, 32, false);
//		wnd->Run();
//		wnd->DestroyWnd();
//	}
//	CoUninitialize();
//#endif
}

void printHelp(char *i)
{
	printf("dflektr - TCP Port Deflector / Modifier \n");
	printf("(c) 2014 Greg Frazier\n\n");
	printf("Usage:\n");
	printf("%s [-srcport PORT] [-destport PORT] [-destip IP.ADD.RESS] [-ssh] [-verbose]\n\n", i);
	printf("\tsrcport  - Port to listen on for client connections\n");
	printf("\tdestport - Port to connect to destination host\n");
	printf("\tdestip   - Address of destination host\n");
	printf("\tssh      - Activate SSH (not functional)\n");
	printf("\tverbose  - Output logs to STDOUT, otherwise silent\n\n");
	return;
}
