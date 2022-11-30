#ifndef _STD_TYPES_H
#define _STD_TYPES_H


typedef unsigned char uint8;
typedef unsigned short uint16;


#define NSTEPPER 2
#define NSERVOS 3
#define NMOTOR (NSTEPPER+NSERVOS)
#define SIZE_MOVE (2+NMOTOR*2+2)


#endif