#ifndef _AUSW_MATH_H_
#define _AUSW_MATH_H_

#include "koord.h"

#define PI	3.14159265358979323846f

double lin_reg(double *xarr,double *yarr,int n);

double angle_reg(double *xarr, double *yarr, int n);

Koord intersect(double phi_1,Koord pos_1,double phi_2, Koord pos_2);

double angle(Koord a,Koord b);

#endif
