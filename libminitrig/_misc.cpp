#include "_misc.hpp"

#include <cmath>


namespace minitrig {


float abs(float x) {
	return ::fabsf(x);
}

float sqrt(float x) {
	return ::sqrtf(x);
}

float fmod(float x,float m) {
	return ::fmodf(x,m);
}

//Error-ignoring versions (tested faster).  See http://stackoverflow.com/a/15341154/688624
int floor(float x) {
	int trunc = (int)(x);
	return trunc - ((float)(trunc)>x);
}
int floor(double x) {
	int trunc = (int)(x);
	return trunc - ((double)(trunc)>x);
}


}
