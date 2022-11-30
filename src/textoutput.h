#ifndef TEXTOUTPUT_H
#define TEXTOUTPUT_H

extern int debug;

typedef enum msgType
{
    STATUS_INFO,
    STATUS_DEBUG,
    STATUS_ERROR,
    RS232SEND,
    RS232_MSG,
    RS232_DIVERSE
} msgType;


void prtMsg(msgType type, const char* txt);



#endif // TEXTOUTPUT_H
