
#include "Marlin.h"

#include "timediff_ms.h"
#include "error.h"

#include "textoutput.h"




Marlin::Marlin(Config *p_config)
{
	const char *cp;
	config = p_config;
	Koord dummy;
	int dynamictimeout_ms;

	prtMsg(STATUS_DEBUG, "Marlin::Marlin constructor");
	rs232 = new RS232(config->get_rs232devname(), 115200, config->get_marlinreset());

	if(!rs232->connected())
	{
		delete rs232;
		rs232 = NULL;
		prtMsg(STATUS_DEBUG, "could not connect -> delete rs232");
		return;
	}

	dynamictimeout_ms=10000;		// in the beginning wait 10 seconds for RS232 messages
	// as soon as the first message arrived, wait no more than 500ms for the next one
	prtMsg(STATUS_DEBUG, "results from MARLIN initialization:");
	while(NULL != (cp = rs232->getLine('?',dynamictimeout_ms)))
	{
		prtMsg(RS232_DIVERSE, cp);
		dynamictimeout_ms=500;
	}		

	if(getPosition(&dummy) < 0)
	{
		prtMsg(STATUS_INFO, "Marlin::Marlin unexpected error in getPosition");
		delete rs232;
		rs232=NULL;
	}
	prtMsg(STATUS_INFO, "Set default values for acceleration\n");
	rs232->sendAsc((unsigned char *)"M201 X100 Y100 Z100\r");
	rcvResult();

// set calibration values
#if 1==0
    prtMsg(STATUS_INFO, "set values for skew factors (enter)\n");
	getchar();
//	rs232->sendAsc((unsigned char *)"M852 I-0.00419 J-0.00003 K0.00038\r");
//	rs232->sendAsc((unsigned char *)"M852 I-0.00422 J0.00001 K0.00047\r");
// calibration 25.3.2022
	rs232->sendAsc((unsigned char *)"M852 I-0.00400 J0.0002 K0.0000\r");
	rcvResult();

    prtMsg(STATUS_INFO, "set values for backlash (enter)\n");
	getchar();
	// values after calibration 25.3.2022
	rs232->sendAsc((unsigned char *)"M425 F1 X0.050 Y0.050\r");
	rcvResult();

	// calibration 25.3.2022
	prtMsg(STATUS_INFO, "set values for bedleveling (enter)\n");
	getchar();
	rs232->sendAsc((unsigned char *)"M421 X0.000 Y0.000 Z0.00000\r");
	rcvResult();
	rs232->sendAsc((unsigned char *)"M421 X0.000 Y90.000 Z-0.020\r");
	rcvResult();
	rs232->sendAsc((unsigned char *)"M421 X90.000 Y90.000 Z0.005\r");
	rcvResult();
	rs232->sendAsc((unsigned char *)"M421 X90.000 Y0.000 Z0.008\r");
	rcvResult();
	rs232->sendAsc((unsigned char *)"M420 S1\r");
	rcvResult();
#else
    prtMsg(STATUS_INFO, "skewfactors, etc. disabled [ENTER]\n");
	getchar();
#endif
}


Marlin::~Marlin(void)
{
	prtMsg(STATUS_DEBUG, "Marlin destructor");
	if (rs232)
		delete rs232;
	prtMsg(STATUS_DEBUG, "Marlin destructor finished");
}


void Marlin::rcvResult(void)
{
	const char *cp;
	prtMsg(STATUS_DEBUG, "Marlin::rcvResult(): cleanup RS232 after user-command");
	while(NULL != (cp = rs232->getLine('?',4000)))
	{
		prtMsg(STATUS_DEBUG,cp);
		if(!strncmp(cp,"ok",2))
			break;
	}
}

// return 1, if acknowledged by "ok"
// exits application, if "ok" is not received within timeout
// ignores "busy" messages
// prints message if reading something !="ok"
// 

int Marlin::rcvAcknowledge(const char *msg,int timeout_ms)
{
	const char *cp = NULL;
	int retval=1;

	prtMsg(STATUS_DEBUG, "Marlin::rcvAcknowledge(): read RS232 until 'ok' for the following activity:");
	prtMsg(STATUS_DEBUG, msg);
	while(1)
	{
		if((cp = rs232->getLine('?',timeout_ms)))
		{
			if(!strncmp(cp,"ok",2))
			{
//				retval=1;
				break;
			}
			else
			{
				if(!strncmp(cp,"echo:busy",9))
				{
					prtMsg(STATUS_DEBUG,"Waiting for Acknowledge and received 'busy' (continue trying):");
					prtMsg(STATUS_DEBUG, msg);
					prtMsg(STATUS_DEBUG,cp);
				}
				else
                {
                    if(!strncmp(cp,"Error:Failed to reach target",28))
                    {
                        prtMsg(STATUS_INFO,"WARNING: Waiting for Acknowledge and received 'Error' (continue trying):");
                        prtMsg(STATUS_INFO, msg);
                        prtMsg(STATUS_INFO,cp);
                        retval=0;       // return "Error:" was found -> For Probe moves, this means, that block was NOT hit, continue waiting for "ok"
                    }
                    else
                    {
                        prtMsg(STATUS_INFO,"WARNING: Waiting for Acknowledge by 'ok', but got (continue trying):");
                        prtMsg(STATUS_INFO, msg);
                        prtMsg(STATUS_INFO,cp);
                    }
                }
			}
		}
		else
		{
			prtMsg(STATUS_ERROR,"Waiting for Acknowledge by 'ok', but did not receive within timeout (EXIT)");
			prtMsg(STATUS_ERROR, msg);
			break;
		}
	}
	return retval;
}

// return -1: error, position could not be read
//   throw 0; is executed for this case
// return 1: all fine
int Marlin::getPosition(Koord *resKoo)
{
	const char *cp;
	Koord pos(1e8,1e8,1e8);
	int retrycnt=5;

	prtMsg(STATUS_DEBUG, "readpos, getPosition-----------------------------------");
	do{
		rs232->sendAsc((unsigned char *)"M114 R\r");

		while(!(cp=rs232->getLine('X',1000)))
		{
			// check, if retry for M114 is still needed (seems obsolete)
			if(!(retrycnt--))
			{
				prtMsg(STATUS_DEBUG, "M114 does not succeed after 5 retries");
				*resKoo = Koord(1e8,1e8,1e8);
                throw 0;
				return -1;
			}
			rs232->sendAsc((unsigned char*)"M114 R\r");
		}


		sscanf(cp,"X:%lf Y:%lf Z:%lf",&pos.x,&pos.y,&pos.z);

		if(pos.x==1e8 || pos.y==1e8 || pos.z==1e8)
		{
			// this shall never happen!
			prtMsg(RS232_DIVERSE, cp);
			prtMsg(STATUS_DEBUG, "Marlin::getPosition() WARNING M114 returns implausible data");
		}
	} while(pos.x==1e8 || pos.y==1e8 || pos.z==1e8);


	if(!(rcvAcknowledge("M114")))
	{
		*resKoo = Koord(1e8,1e8,1e8);
		prtMsg(STATUS_DEBUG, "no/wrong acknowledge from M114 command");
        throw 0;
		return -1;
	}
	
	// only store new position, if M114 was acknowledged by "ok"
	m_actkoo = pos;
	*resKoo  = pos;

	if(debug)
		printf("  $$ M114 R result: %s  ==> %lf, %lf, %lf\n",cp,pos.x,pos.y,pos.z);

	return 1;
}// getPosition


// throw 0; in case of probe is not hit
// return 1: success (m_actkoo was updated)
// return 0: failed (exits application if final "ok" (acknowledge) from Marlin is missing)
int Marlin::SaveMoveAbs(Koord targetpos, int speed, const char *errmsg)
{
    // to be implemented (Use G38.2 and succeed in case of error is reported (error means, final position is reached without probe activated)
	char gc_buf[256];
	int timeout_ms;
    const char *cp;

	prtMsg(STATUS_DEBUG, "##################");
	prtMsg(STATUS_DEBUG, "SaveMoveAbs");

	timeout_ms = 2000 + calc_timeout_ms(Koord::absKoord(m_actkoo - targetpos), speed);

	sprintf(gc_buf,"G38.2 X%lf Y%lf Z%lf F%d\r",targetpos.x,targetpos.y, targetpos.z, speed);
	
	rs232->sendAsc((unsigned char*)gc_buf);

// how is "position is reached" reported by marlin?"
// "echo:busy: processing" multiple times each 2 seconds -> ignore
// "Error:Failed to reach target" -> move was successfull without probe getting active
// "ok" -> final message
	while(1)
	{
		if((cp = rs232->getLine('?',timeout_ms)))
		{
            if(!strncmp(cp,"ok",2))
            {
                // probe hit block -> report error
                throw 0;
                return 0;
            }
            if(!strncmp(cp,"echo:busy: processing",21))
                continue;
            if(!strncmp(cp,"Error:Failed to reach target",28)) // This is the "success" for SaveMove
                break;
        }
        else
        {
            prtMsg(STATUS_ERROR, "Marlin::SaveMoveAbs(): not acknowledged");
        }
	}
    if(rcvAcknowledge("SaveMoveAbs",timeout_ms))    // wait for final "ok"
	{
		prtMsg(STATUS_DEBUG,"Marlin::SaveMoveAbs acknowledged by ok");
		m_actkoo = targetpos;
		return 1;
    }
	prtMsg(STATUS_ERROR, "Marlin::SaveMoveAbs(): not acknowledged");
    return 0;
}
	

// return 1: success (m_actkoo was updated)
// return 0: failed (exits application if G1 command is not acknowledged by Marlin)
int Marlin::FreeMoveAbs(Koord targetpos, int speed, const char *errmsg)
{
	char sendbuf[256];
	int timeout_ms;
	int retval=0;

// do nothing, if move is to short
	if(Koord::absKoord(m_actkoo - targetpos)<=0.03)
		return 1;		// position already there -> positive feedback

	timeout_ms = 2000 + calc_timeout_ms(Koord::absKoord(m_actkoo - targetpos), speed);

	sprintf(sendbuf,"G1 X%.3lf Y%.3lf Z%.3lf F%d\r",targetpos.x,targetpos.y,targetpos.z,speed);
	rs232->sendAsc((unsigned char *)sendbuf);
	
	prtMsg(STATUS_DEBUG, "FreeMoveAbs read:");

	if(rcvAcknowledge("FreeMoveAbs",timeout_ms))
	{
		prtMsg(STATUS_DEBUG,"Marlin::FreeMoveAbs acknowledged by ok");
		// probe successfull
		m_actkoo = targetpos;
		retval = 1;
	} else {
		// free move failed
		prtMsg(STATUS_ERROR, "Marlin::FreeMoveAbs not acknowledged by ok");
	}
	return retval;
}




// throw 0; in case of probe is not hit
// return 1: success (m_actkoo was updated)
// return 0: failed (exits application if final "ok" (acknowledge) from Marlin is missing)
int Marlin::SaveMoveRel(Koord posoff, int speed, const char *errmsg)
{
    const char *cp;
	char gc_buf[256];
	int timeout_ms;

	prtMsg(STATUS_DEBUG, "##################");
	prtMsg(STATUS_DEBUG, "SaveMoveRel");

// do nothing, if move is to short
	if(Koord::absKoord(posoff)<=0.003)
		return 1;		// position already there -> positive feedback

	timeout_ms = 2000 + calc_timeout_ms(Koord::absKoord(posoff), speed);

	sprintf(gc_buf,"G38.2 X%.3lf Y%.3lf Z%.3lf F%d\r",m_actkoo.x+posoff.x,m_actkoo.y+posoff.y,m_actkoo.z+posoff.z,speed);
	rs232->sendAsc((unsigned char *)gc_buf);

// how is "position is reached" reported by marlin?"
// "echo:busy: processing" multiple times each 2 seconds -> ignore
// "Error:Failed to reach target" -> move was successfull without probe getting active
// "ok" -> final message
	while(1)
	{
		if((cp = rs232->getLine('?',timeout_ms)))
		{
            if(!strncmp(cp,"ok",2))
            {
                // probe hit block -> report error
                throw 0;
                return 0;
            }
            if(!strncmp(cp,"echo:busy: processing",21))
                continue;
            if(!strncmp(cp,"Error:Failed to reach target",28)) // This is the "success" for SaveMove
                break;
        }
        else
        {
            prtMsg(STATUS_ERROR, "Marlin::SaveMoveRel(): not acknowledged");
        }
	}
    if(rcvAcknowledge("SaveMoveRel",timeout_ms))    // wait for final "ok"
	{
        m_actkoo = m_actkoo + posoff;
		return 1;
    }
	prtMsg(STATUS_ERROR, "Marlin::SaveMoveRel(): not acknowledged");
    return 0;
}


// return 1: success (m_actkoo was updated)
// return 0: failed (exits application if "ok (acknowledge) from Marlin is missing)
int Marlin::FreeMoveRel(Koord posoff, int speed, const char *errmsg)
{
	char sendbuf[256];
	int timeout_ms;
	int retval=0;

// do nothing, if move is to short
	if(Koord::absKoord(posoff)<=0.003)
		return 1;		// position already there -> positive feedback

	timeout_ms = 2000 + calc_timeout_ms(Koord::absKoord(posoff), speed);

	sprintf(sendbuf,"G1 X%.3lf Y%.3lf Z%.3lf F%d\r",m_actkoo.x+posoff.x,m_actkoo.y+posoff.y,m_actkoo.z+posoff.z,speed);
	rs232->sendAsc((unsigned char *)sendbuf);

	if (rcvAcknowledge("FreeMoveRel", timeout_ms)) {
		prtMsg(STATUS_DEBUG, "Marlin::FreeMoveRel acknowledged by ok");
		// probe successfull
		m_actkoo = m_actkoo + posoff;
		retval = 1;
	} else {
		// free move failed (we should never come here)
		prtMsg(STATUS_ERROR, "Marlin::FreeMoveRel not acknowledged by ok");
	}
	return retval;
}




// if abtastenflag!=0 -> G38.4 else G38.2
// throw 0; in case of probe is not hit
// exits application if getPosition failed 
// return 1: success
// return 0: failed
int Marlin::ProbeMoveAbs(Koord targetpos, Koord &pos, int speed, const char *errmsg, int abtastenflag)
{
	char gc_buf[256];
	int timeout_ms;

	prtMsg(STATUS_DEBUG, "##################");
	prtMsg(STATUS_DEBUG, "Probing");

	pos=Koord(1e8,1e8,1e8);// initialize to "illegal coordinate"

	timeout_ms = 2000 + calc_timeout_ms(Koord::absKoord(m_actkoo - targetpos), speed);

	if(!abtastenflag)
		sprintf(gc_buf,"G38.2 X%lf Y%lf Z%lf F%d\r",targetpos.x,targetpos.y, targetpos.z, speed);
	else
		sprintf(gc_buf,"G38.4 X%lf Y%lf Z%lf F%d\r",targetpos.x,targetpos.y, targetpos.z, speed);
	
	rs232->sendAsc((unsigned char*)gc_buf);

// wait 2000ms + expected time for move for message
	if(rcvAcknowledge("ProbeMoveAbs",timeout_ms))
	{
		if(getPosition(&pos)<0)
		{
			prtMsg(STATUS_ERROR, "Marlin::ProbeMoveAbs(): getPosition error after Probing");
		}
        
		if(debug)
		{
			printf("Marlin::ProbeMoveAbs(): Position after probing: X%lf Y%lf Z%lf\n",pos.x,pos.y,pos.z);
		}
		return 1;
	}else{
		prtMsg(STATUS_INFO, "WARNING: Marlin::ProbeMoveAbs(): Did not hit the block");
        throw 0;
		return 0;
	}
}


// if abtastenflag!=0 -> G38.4 else G38.2
// throw 0; in case of probe is not hit
// return 1: success
// return 0: failed (exits application if getPosition failed)
int Marlin::ProbeMoveRel(Koord posoff, Koord &pos, int speed, const char *errmsg, int abtastenflag)
{
	char gc_buf[256];
	int timeout_ms;


	prtMsg(STATUS_DEBUG, "##################");
	prtMsg(STATUS_DEBUG, "Probing");

	pos=Koord(1e8,1e8,1e8);// initialize to "illegal coordinate"

	timeout_ms = 2000 + calc_timeout_ms(Koord::absKoord(posoff), speed);

	if(!abtastenflag)
		sprintf(gc_buf,"G38.2 X%lf Y%lf Z%lf F%d\r",m_actkoo.x+posoff.x,m_actkoo.y+posoff.y, m_actkoo.z+posoff.z, speed);
	else
		sprintf(gc_buf,"G38.4 X%lf Y%lf Z%lf F%d\r",m_actkoo.x+posoff.x,m_actkoo.y+posoff.y, m_actkoo.z+posoff.z, speed);

	rs232->sendAsc((unsigned char*)gc_buf);


	// wait 2000ms + expected time for move for message
	if (rcvAcknowledge("ProbeMoveRel", timeout_ms)) {
		if (getPosition(&pos) < 0) {    // m_actpos is set inside!
			prtMsg(STATUS_ERROR,"Marlin::ProbeMoveRel(): getPosition error after Probing");
//			return 0;
		}
		if(debug)
		{
			printf("Marlin::ProbeMoveRel(): Position after probing: X%lf Y%lf Z%lf\n",
					pos.x, pos.y, pos.z);
		}
		return 1;
	} else {
		prtMsg(STATUS_INFO, "WARNING: Marlin::ProbeMoveRel(): Did not hit the block");
        throw 0;
		return 0;
	}
}

