#ifndef _MARLIN_H_
#define _MARLIN_H_

#include "Mill.h"
#include "koord.h"
#include "rs232_cl.h"
#include "config.h"


class Marlin:public Mill
{
private:

	Config *config;
	
	int rcvAcknowledge(const char *msg,int timeout_ms=2000);

public:
	RS232 *rs232;
	Marlin(Config *p_config);
	~Marlin(void);

	int connected(void){return rs232 ? 1:0;}

	void send(unsigned char *gcodestr){rs232->sendAsc(gcodestr);}


	void rcvResult(void);
    
    int getPosition(Koord *resKoo);
	int FreeMoveAbs(Koord targetpos, int speed, const char *errmsg);
	int FreeMoveRel(Koord posoff, int speed, const char *errmsg);
    int SaveMoveAbs(Koord targetpos, int speed, const char *errmsg);
	int SaveMoveRel(Koord posoff, int speed, const char *errmsg);
    int ProbeMoveAbs(Koord target, Koord &pos, int speed, const char *errmsg, int abtastenflag=0);
	int ProbeMoveRel(Koord posoff, Koord &pos, int speed, const char *errmsg, int abtastenflag=0);

};
#endif
