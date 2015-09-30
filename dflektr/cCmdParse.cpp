#include <string.h>
#include <stdlib.h>
#include "cCmdParse.h"


cCmdParse::cCmdParse(char* arguments[], int argCount)
{
	// Initialize
	this->badArguments = false;
	this->useSSH = false;
	this->verbose = false;
	this->hasDest = false;

	bool hasDestPort = false, hasSrcPort = false;
	char y, *command;
	
	// Skip the first argument (it's the executable name)
	for(int CycleArguments = 1; CycleArguments < argCount; CycleArguments++)
	{
		// Scan through the arguments to see if one matches, otherwise we should throw an error
		y = (arguments[CycleArguments])[0];

		if(y == '-')
		{
			command = arguments[CycleArguments];
			command++; // Move one char over.
			
			if(strcmp(command, "destport") == 0)
			{
				// Get the port, it will be the next "command"
				if(CycleArguments + 1 < argCount){
					command = arguments[++CycleArguments];
					destPort = strtol(command, NULL, 10);
					hasDestPort = true;
				}
			}
			else if(strcmp(command, "srcport") == 0)
			{
				if(CycleArguments + 1 < argCount){
					command = arguments[++CycleArguments];
					this->srcPort = strtol(command, NULL, 10);
					hasSrcPort = true;
				}
			}
			else if(strcmp(command, "destip") == 0)
			{
				if(CycleArguments + 1 < argCount){
					command = arguments[++CycleArguments];
					this->destIPAddress = (char*)malloc(sizeof(char) * (strlen(command) + 1));
					memcpy(destIPAddress, command, strlen(command)+1);
					this->hasDest = true;
				}
			}
			else if(strcmp(command, "ssh") == 0)
			{
				this->useSSH = true;
			}
			else if(strcmp(command, "verbose") == 0)
			{
				this->verbose = true;
			}
		}
	}
	if( (hasDestPort & hasDest & hasSrcPort) == false )
		this->badArguments = true;
	else
		this->badArguments = false;
}


cCmdParse::~cCmdParse(void)
{
	if(hasDest)
		free(destIPAddress);
}
