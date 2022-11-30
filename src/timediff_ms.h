#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "error.h"

#if defined(__linux__) || defined(__FreeBSD__)
    #include <sys/timex.h>
#else
#endif

void tm_wait_ms(int ms);

void tm_start(void);
// return TRUE, when tm_ms is passed since tm_start was called
int tm_passed_ms(int tm_ms);

int calc_timeout_ms(double dist_mm, int speed_mmmin);


