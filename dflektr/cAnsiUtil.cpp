#include "cAnsiUtil.h"
#include <stdlib.h>

// Just hacks to snoop on the data stream when forwarding a telnet session

char *cAnsiUtil::ignoreTerminate(char *in, int bufSize)
{
	int h = bufSize;
	char *k, *d;
	d = (char *) malloc(sizeof(char) * (bufSize + 1));
	if( d != NULL ){
		k = in;
		while(h > 0)
		{
			if(*k == '\0')
				d[bufSize-h] = ' ';
			//else if(*k == 0x7F) // Delete Key
			//	d[bufSize-h] = '«';
			//else if(*k == 0x08) // Backspace Key
			//	d[bufSize-h] = '«';
			//else if(*k == 0x07) // BEL
			//	d[bufSize-h] = '¤';
			//else if(*k == 0x0A)
			//	d[bufSize-h] = '¤';
			//else if(*k == 0x0D)
			//	d[bufSize-h] = '¤';
			//else if(*k == 0x1B)
			//	d[bufSize-h] = in[bufSize-h];
			//else if(*k < 0x20)
			//	d[bufSize-h] = '?';			
			else
				d[bufSize-h] = in[bufSize-h];
			k++; 
			--h;
		}
	}
	return d; // need to free it.
}

bool cAnsiUtil::isCSICommand(char in)
{
	// Misleading, this just checks if the command is a number (CSI argument) or ; (CSI command terminator)
	if(('0' <= (in) && (in) <= '9') || in == ';')
		return false;
	return true;
}

char *cAnsiUtil::removeANSICodes(char *in, int bufSize)
{
	int h = bufSize;
	char *k, *c, l;
	k = (char *) malloc(sizeof(char) * (bufSize + 1));
	if( k != NULL)
	{
		c = k;
		while (h > 0)
		{
			l = in[bufSize-h];
			if(l == 0x1B)
			{
				--h; // next char
				l = in[bufSize-h];
				switch(l)
				{
					case ']': // find BEL.. this is a command to set the title text of the telnet session. we can ignore it.
						while(h > 0 && l != 0x07){
							--h; // next char
							l = in[bufSize-h];
						}
						break;
					case '[': // standard ANSI-CSI escape stuff.. very irritating to work with.
						--h; // next char
						l = in[bufSize-h];
						bool done = false;
						do
						{						
							if(cAnsiUtil::isCSICommand(l))
							{
								switch(l)
								{
									case 'A': 
									case 'B': 
									case 'C': 
									case 'D': 
									case 'E': 
									case 'F': 
									case 'G': 
									case 'H': 
									case 'J': 
									case 'K': 
									case 'S': 
									case 'T': 
									case 'f': 
									case 'm': 
									case 'n': 
									case 's': 
									case 'u': 
										done = true;
										break;
									default:
										--h; // next char
										l = in[bufSize-h];
										break;
								}
							}else{
								--h; // next char
								l = in[bufSize-h];
							}
						}while((h > 0 && (!cAnsiUtil::isCSICommand(l))) || !done);

				}
			}else{
				*c++ = l;
			}
			--h;
		}
		*c = '\0';
	}
	return k;
}

char *cAnsiUtil::removeNonDisplayAble(char *in, int bufSize)
{
	int h = bufSize;
	char *k, *c, l;
	k = (char *) malloc(sizeof(char) * (bufSize + 1));
	if( k != NULL)
	{
		c = k;
		while (h > 0)
		{
			l = in[bufSize-h];
			if(l > 32 && l < 127)
			{
				*c++ = l;
			}else{
				*c++ = '\n';
			}
				//--h; // next char
				//l = in[bufSize-h];
			//}else{
			//	*c++ = l;
			//}
			--h;
		}
		*c = '\0';
	}
	return k;
}