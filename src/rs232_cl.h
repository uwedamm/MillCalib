#ifndef _RS232_CL_H_
#define _RS232_CL_H_

//#include <termios.h>

// usage of https://gitlab.com/Teuniz/RS-232
// to control RS232 on either of win or linux
#include "rs232.h"


#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <vector>

#define MAXBUFLEN 8192

class RS232
{
private:
	// filedescriptor to send data via RS232 from RPI to Arduino
	//int fd;
	int comnr;

	char buf[MAXBUFLEN];
	char retbuf[MAXBUFLEN];		// buffer to return to caller

	std::vector<const char *> linesbuf;
	size_t linesbufpos;						// position of last NOT returned line


	
//	ssize_t startpos;		// currently not used
	ssize_t readpos;		//position, where next data will be stored

	void cleanup(void);		// remove "ok\n" in the beginning of buf, move rest of buf to the beginning

    void send(unsigned char *buf,int len);


public:
	RS232(void){printf("RS232(void) default constructor shall not be used\n");exit(1);};



// RS232_OpenComport(int comport_number, int baudrate, const char *mode, int flowctrl)
    RS232(const char *device, int baudrate,int resetmarlin);
	~RS232(void);

	int connected(void){return comnr>=0 ? 1:0;}

    void sendAsc(unsigned char *str);

// currently not used
//	ssize_t receive(char *buf,ssize_t maxlen);
//	ssize_t readbin(unsigned char *buf,ssize_t maxlen);


	//int available(void);

	void buf2linesbuf(void);
	
	
	// read all available data into buf (do not stop, if last read char != '\n');
	// store each read line into linesbuf vector
	void readIntoBuf(void);

	// get first line (!= "ok\n" by design)
	// 	AND move remaining data to beginning of buf
	//const char *getLine(char startch);
	const char *getLine(char startch, signed long long timeout = 0);
	


};

#endif
