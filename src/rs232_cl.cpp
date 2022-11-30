#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "textoutput.h"

#if defined(__linux__) || defined(__FreeBSD__)
#include <sys/ioctl.h>
#include <fcntl.h>
#endif

#include <stdlib.h>


#include "Std_Types.h"
#include "rs232_cl.h"
#include "error.h"
#include "timediff_ms.h"


// Some reading, but not used here:
// 	https://www.cmrr.umn.edu/~strupp/serial.html#5_1_3
//  Serial Programming Guide for POSIX Operating Systems





RS232::RS232(const char *devname, int baudrate, int resetmarlin)
{

	comnr=atoi(devname);

	if(debug & 1)
		printf("RS232::RS232: comnr=%d\n",comnr);

	// 16: /dev/ttyUSB0, 
	// 0: no flow-ctrl
	if(RS232_OpenComport(comnr, baudrate, "8N1", 0))
	{
		comnr=-1;
		prtMsg(STATUS_ERROR, "RS232::RS232 connection failed");
	}
	else
	{
		if (resetmarlin)
		{
			RS232_enableDTR(comnr);
			RS232_disableDTR(comnr);
			prtMsg(STATUS_DEBUG, "RS232::RS232: toggled DTR, to reset MARLIN/arduino\n");
		} else {
			prtMsg(STATUS_DEBUG, "RS232::RS232: do not toggle DTR, to start rs232 without reset to MARLIN/arduino\n");
		}
	}
	
	readpos=0;		// start of buffer, when reading data
	linesbufpos=0;	// start linesbuf from beginning

	prtMsg(STATUS_DEBUG, "RS232::RS232 finished");
	return;
}

RS232::~RS232(void)
{
	size_t n;
	prtMsg(STATUS_DEBUG, "RS232::~RS232");
	
	if (comnr >= 0)
		RS232_CloseComport(comnr);
	else
	{
		prtMsg(STATUS_DEBUG, "RS232, comnr<0 -> no close()");
	}
	
	for(n = 0 ; n < linesbuf.size() ; n++)
	{
		delete [] linesbuf.at(n);
	}
	prtMsg(STATUS_DEBUG, "RS232::~RS232() linesbuf deleted");
}



/* send len bytes to RS232 */
void RS232::send(unsigned char *p_buf, int len)
{
    int lenwrt;

	if(comnr<0)
	{
		prtMsg(STATUS_ERROR, "RS232::send() comnr<0, RS232::RS232() could not open the device?");
	}
    lenwrt = RS232_SendBuf(comnr, p_buf, len);
	if(lenwrt!=len)
	{
		printf("RS232::send: %d/%d bytes written: \"%s\" written\n", lenwrt, len, p_buf);
        prtMsg(STATUS_ERROR, "RS232::send failed");
	}
}

void RS232::sendAsc(unsigned char *str)
{
	if(debug & 1)
		printf(" >> RS232::sendAsc: %s\n",str);
    send(str,strlen((const char *)str));
}


void RS232::buf2linesbuf(void)
{	
	// char *strchr(char*c,char z)
	// return address of first occurance of z in c
	char *cp=&buf[0];
	char *cp_newline;	// points to the '\n'
	char *cp_line;		// reserves space for the line, to be added to linesbuf Vector
	int linelen;
	
	while((cp_newline=strchr(cp,'\n')))
	{
		linelen=(int)(cp_newline-cp)+1;

// cp_line not used???
		cp_line=new char [linelen];
		*cp_newline='\0';
		strcpy(cp_line,cp);	// copy line to new linebuffer

		if(debug & 1)
		{
			printf("RS232::buf2linesbuf: \n");
			printf("Received:%ld:%s\n",(unsigned long int)linesbuf.size(),cp);
		}
		linesbuf.push_back(cp_line);
		
		cp = cp_newline + 1;	// start of next line or '\0'
	}
	
}





// if data available, read as long as end of message != \n
// append \0 to read data
// at the end cleanup()
void RS232::readIntoBuf(void)
{
	ssize_t readlen=0;		// position where actually is written to buf
	int retflag=0;			// flag to signal, that '\n' was found at the end of the buf
	
	if(comnr < 0)
	{
		prtMsg(STATUS_ERROR, "RS232::readIntoBuf() comnr<0, rs232_Init() call missing?");
		exit(1);
	}

	readpos=0;
	do
	{
		do{
			readlen = RS232_PollComport(comnr, (unsigned char *)&buf[readpos], (MAXBUFLEN-1) - readpos);
			if(readlen>0)
			{
				buf[readlen+readpos]='\0';
				readpos+=readlen;
				if (buf[readpos-1]=='\n')
					retflag=1;
				else
					retflag=0;
				if (readpos >= (MAXBUFLEN-1))
				{
					prtMsg(STATUS_ERROR, "readIntoBuf: MAXBULEN!!!");
					return;
				}
			}
			if(readlen<0)
			{
				retflag=0;
				prtMsg(STATUS_ERROR, "RS232::readIntoBuf() read failed");
				return;
			}
		}while(readlen>0);
	} while(!retflag && readpos>0);
	// !available AND '\n' at the end
	buf2linesbuf();
}


// find last line in linesbuf, which starts with startch (downto linesbufpos)
// mark that line as read
// return NULL, if no corresponding line was read
const char *RS232::getLine(char startch, signed long long timeout)
{
	size_t n;
	const char *cp=NULL;

	if(timeout>0)
		tm_start();

//    prtMsg(STATUS_DEBUG, "RS232::getLine()");
//    printf("startch=='%c'",startch);
// timeout loop (if timeout>0)
	do{
		// read all available data from rs232 till \n is found in the end
//        prtMsg(STATUS_DEBUG, "RS232::getLine()");
		readIntoBuf();
		
		if(startch == '?')
		{// return 1st unread line
			if(linesbufpos < linesbuf.size())
			{
				cp = linesbuf.at(linesbufpos);
				linesbufpos++;
			}
		}
		else
		{
			for( n = linesbuf.size() - 1 ; n >= linesbufpos ; n--)
			{// return last line, which fits to startch
				cp = linesbuf.at(n);
				if(cp[0]==startch)
				{
					linesbufpos=n+1;
					break;
				}
				else
				{
					cp=NULL;
				}
			}
		}
		if(!(timeout>0) || cp != NULL)
			break;
	} while(!tm_passed_ms(timeout));
	
//     prtMsg(STATUS_DEBUG, "RS232::getLine() ended");
	return cp;
}

