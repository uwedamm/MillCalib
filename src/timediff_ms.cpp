#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "textoutput.h"

#if defined(__linux__) || defined(__FreeBSD__)
#include <sys/timex.h>
#else
#include <sysinfoapi.h>
#endif






#if defined(__linux__) || defined(__FreeBSD__)
struct ntptimeval ntp_start, ntp_act;

void tm_wait_ms(int ms)
{
   struct timespec tim;
   
   tim.tv_sec = ms/1000L;
   tim.tv_nsec = 1000000L*(ms % 1000);

   if(nanosleep(&tim , NULL) < 0 )   
   {
      perror("nanosleep() system call failed\n");
      prtMsg(STATUS_ERROR,"nanosleeps ystem call failed\n");
   }
//	nanosleep((const struct timespec[]){{0, 1000000L*ms}}, NULL);
}



void tm_start(void)
{
	ntp_gettime(&ntp_start);
}

// return TRUE, when tm_ms is passed since tm_start was called
int tm_passed_ms(int tm_ms)
{
	ntp_gettime(&ntp_act);


	return (((signed long long) ntp_act.time.tv_sec * 1000000000ll +
	       (signed long long) ntp_act.time.tv_usec) -
	       ((signed long long) ntp_start.time.tv_sec * 1000000000ll +
	       (signed long long) ntp_act.time.tv_usec))
			> (signed long long)tm_ms * 1000000ll;
}
#else   // windows version:

DWORD ntp_start, ntp_act;

void tm_start(void)
{
    if(sizeof(ntp_start)!=4)
    {
        prtMsg(STATUS_ERROR,"sizeof(DWORD) != 4 cannot be used for GetTickCount()\n");
    }

    ntp_start = GetTickCount();
}

// return TRUE, when tm_ms is passed since tm_start was called
int tm_passed_ms(int tm_ms)
{
    ntp_act=GetTickCount();

    return (ntp_act-ntp_start) > (unsigned long)tm_ms;
}
#endif // #else defined(__linux__) || defined(__FreeBSD__)



int calc_timeout_ms(double dist_mm, int speed_mmmin)
{
	return (int)(1000.0*dist_mm*60.0/(double)speed_mmmin);
}
