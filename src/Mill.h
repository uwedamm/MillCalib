#ifndef _MILL_H_
#define _MILL_H_


#include "koord.h"
#include "textoutput.h"

class Mill
{
protected:
	Koord m_actkoo;
	

public:
	Mill(void)
    {
        prtMsg(STATUS_DEBUG, "Marlin::Marlin constructor");
        m_actkoo=Koord(0.0,0.0,0.0);
    }
	virtual ~Mill(void){};

	Koord Get_actkoo(void){return m_actkoo;}
	
	virtual int connected(void){return 0;}; // if not exist in derived class -> not connected

// all functions below need to be implemented in derived class!
	virtual void send(unsigned char *gcode)=0;
	virtual void rcvResult(void)=0;
	virtual int getPosition(Koord *resKoo)=0;
	virtual int FreeMoveAbs(Koord targetpos, int speed, const char *errmsg)=0;
	virtual int FreeMoveRel(Koord posoff, int speed, const char *errmsg)=0;
	virtual int SaveMoveAbs(Koord targetpos, int speed, const char *errmsg)=0;
	virtual int SaveMoveRel(Koord posoff, int speed, const char *errmsg)=0;
	virtual int ProbeMoveAbs(Koord target, Koord &pos, int speed, const char *errmsg, int abtastenflag=0)=0;
	virtual int ProbeMoveRel(Koord posoff, Koord &pos, int speed, const char *errmsg, int abtastenflag=0)=0;
};
#endif
