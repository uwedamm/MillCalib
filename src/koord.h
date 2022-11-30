#ifndef KOORD_H
#define KOORD_H

#include <cmath>

//#include <math.h>

class Koord {
  public:
	double x,y,z;
	
	Koord(){x=y=z=0.0;}
	Koord(double p_x,double p_y,double p_z){x=p_x;y=p_y;z=p_z;}
	Koord(const Koord &koo){x=koo.x;y=koo.y;z=koo.z;};
	Koord operator = (const Koord koo) {
		x=koo.x;
		y=koo.y;
		z=koo.z;
		return *this;
		}
	Koord operator- (const Koord koo) {
		return Koord(x-koo.x,y-koo.y,z-koo.z);
		}
	Koord operator- () {
		return Koord(-x,-y,-z);
		}


	Koord operator+ (const Koord koo) {
		return Koord(x+koo.x,y+koo.y,z+koo.z);
		}

	Koord operator/ (double fak) {
		return Koord(x/fak, y/fak, z/fak);
		}

	Koord operator* (double fak) {
		return Koord(x*fak, y*fak, z*fak);
		}

	static Koord crossProd(Koord a,Koord b){return Koord(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);}

	static double absKoord(Koord koo)
	{
		return sqrt(koo.x*koo.x + koo.y*koo.y + koo.z*koo.z);
	}
		
		
};


#endif
