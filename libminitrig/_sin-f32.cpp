#include "_misc.hpp"

#include <cassert>


namespace minitrig {


//1 Dev Master implementation
//2 My Maclaurin implementation
#define IMPL 2


#if IMPL == 1

float sin(float x) {
	if (x>F32_PI) {
		do { x-=F32_2PI; } while (x> F32_PI);
	} else if (x<-F32_PI) {
		do { x+=F32_2PI; } while (x<-F32_PI);
	}

	//http://forum.devmaster.net/t/fast-and-accurate-sine-cosine/9648

	//Error claimed to be around 10^-3

	float y = (float)(4.0/F64_PI)*x - (float)(4.0/(F64_PI*F64_PI))*x*abs(x);
	y = 0.775f*y + 0.225f*y*abs(y);
	return y;
}

#elif IMPL == 2

float sin(float x) {
	//Need to mod to get in correct range.  This is the source of a lot of the error.
	#if 1
		x -= (float)floor(x/F32_2PI)*F32_2PI;
		assert(x>=-1e-5f && x<F32_2PI+1e-5f);
		if (x>F32_PI) x-=F32_2PI;
	#else
		double temp = (double)(x);
		temp -= (double)floor(temp/F64_2PI)*F64_2PI;
		assert(temp>=-1e-5 && temp<F64_2PI+1e-5);
		if (temp>F64_PI) temp-=F64_2PI;
		x = (float)(temp);
	#endif

	//Implement pieces of range using reflections of other pieces
	if (x>F32_HPI) {
		x = F32_PI - x;
	} else if (x<-F32_HPI) {
		x = -F32_PI - x;
	}

	//Maclaurin approximations at -π/2, 0, and π/2.  This seems to
	//	be sufficient to get accuracy ~10^-6.  Then optimize it to improve the accuracy.
	float y;
	if (x<F32_QPI) {
		if (x>-F32_QPI) {
			//-0.25*PI < x < 0.25*PI
			y = x - 1.6666673124e-01f*x*x*x + 8.3330580965e-03f*x*x*x*x*x - 1.9618441002e-04f*x*x*x*x*x*x*x - 2.2868576366e-07f*x*x*x*x*x*x*x*x*x;
		} else {
			x -= -F32_HPI;
			y = 1.0f - 4.9999979138e-01f*x*x + 4.1666436940e-02f*x*x*x*x - 1.3899283949e-03f*x*x*x*x*x*x + 2.6317742595e-05f*x*x*x*x*x*x*x*x;
			y = -y;
		}
	} else {
			x -= F32_HPI;
			y = 1.0f - 4.9999979138e-01f*x*x + 4.1666436940e-02f*x*x*x*x - 1.3899283949e-03f*x*x*x*x*x*x + 2.6317742595e-05f*x*x*x*x*x*x*x*x;
	}
	return y;
}

#endif


}
