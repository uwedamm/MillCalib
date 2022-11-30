#include <stdio.h>

#include "ausw_math.h"
//#include <cmath>



double lin_reg(double *xarr,double *yarr,int n)
{
	double s_x,s_y,s_xx,s_xy,delta;
	int i;
	   
	s_x=s_y=s_xx=s_xy=0.0;
	for(i=0;i<n;i++)
	{
		// s_x=sum(x);
		s_x+=xarr[i];
		// s_y=sum(y);
		s_y+=yarr[i];
		// s_xx=x'*x;
		s_xx+=xarr[i]*xarr[i];
		// s_xy=x'*y;
		s_xy+=xarr[i]*yarr[i];
	}
	delta=n*s_xx-s_x*s_x;

// linear regression
	//a=(s_xx*s_y-s_x*s_xy)/delta;
	return (n*s_xy-s_x*s_y)/delta;	// m
	

//	sig_y=sqrt(1/(N-2) * sum((a + m * x - y).^2));
//	sig_a=sig_y * sqrt(s_xx/delta);
//	sig_b=sig_y * sqrt(N/delta);
}



// calculate middle of all points
// then calculate middle of angle for each point (range constrained by modulo 0..180)
double angle_reg(double *xarr, double *yarr, int n)
{
	double angle;
	double max_x=-1e8,max_y=-1e8;
	double min_x=1e8,min_y=1e8;
	double m;
	int i;
	   
	for(i=0;i<n;i++)
	{
		if(xarr[i]<min_x)
			min_x=xarr[i];
		if(xarr[i]>max_x)
			max_x=xarr[i];
		if(yarr[i]<min_y)
			min_y=yarr[i];
		if(yarr[i]>max_y)
			max_y=yarr[i];
	}		
	if(fabs(max_y-min_y) > fabs(max_x-min_x))
	{
		// "Steile Kurve -> x,y vertauschen für lin_reg
		m=lin_reg(yarr,xarr,n);
		// dx=dy=1 -> 45° -> m=1 -> atan=45
		// dx=0,dy=1 -> 0° -> m=0 -> atan=0
		angle=(m<0) ? (PI+atan(m)) : atan(m);
	} else {
		m=lin_reg(xarr,yarr,n);
		angle=PI/2-atan(m);
		//printf("delx>=dely, m=%g, angle=%g\n",m,angle);
	}
	return 180.0 / PI * angle;
}

// find intersection of two lines, which are given by angle and startpoint
Koord intersect(double phi_1,Koord pos_1,double phi_2, Koord pos_2)
{
	double a,b,c,d,e,f,g,h;
	double l;
	// x1+k*sin(phi1) = x2+l*sin(phi2)
	// y1+k*cos(phi1) = y2+l*cos(phi2)
	a = pos_1.x;
	b = sin(phi_1);
	c = pos_2.x;
	d = sin(phi_2);
	e = pos_1.y;
	f = cos(phi_1);
	g = pos_2.y;
	h = cos(phi_2);
	
	l = (b*(e-g) + f*(c-a))/(h*b - f*d);
	return Koord(pos_2 + Koord(l*sin(phi_2),l*cos(phi_2),0.0));
}


// Note: acos results in angles from 0...180deg (given 1...-1 as argument)
double angle(Koord a,Koord b)
{
	return 180.0/PI*acos(
			(a.x*b.x+a.y*b.y + a.z*b.z)/sqrt((a.x*a.x + a.y*a.y + a.z*a.z) * (b.x*b.x + b.y*b.y + b.z*b.z)));
}

