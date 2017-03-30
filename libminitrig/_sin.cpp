#include "_misc.hpp"


namespace minitrig {


float sin(float x) {
	/*if (x>PI) {
		do {
			x -= 2.0f*PI;
		} while (x>PI);
	} else if (x<-PI) {
		do {
			x += 2.0f*PI;
		} while (x<-PI);
	}

	//http://forum.devmaster.net/t/fast-and-accurate-sine-cosine/9648
	//Error around 10^-3
	float y = (4.0f/PI)*x - (4.0f/(PI*PI))*x*abs(x);
	y = 0.775f*y + 0.225f*y*abs(y);
	return y;*/

	if (x>PI) {
		do {
			x -= 2.0f*PI;
		} while (x>PI);
	} else if (x<-PI) {
		do {
			x += 2.0f*PI;
		} while (x<-PI);
	}

	//Since no one seems to know how to do this reasonably, just use Maclaurin approximations at -pi/2, 0, and pi/2.  This seems to
	//	be sufficient to get accuracy ~10^-6.  Then optimize it to improve the accuracy.

	if (x>0.5f*PI) {
		x = PI - x;
	} else if (x<-0.5f*PI) {
		x = -PI - x;
	}

	float y;
	if (x<0.25f*PI) {
		if (x>-0.25f*PI) {
			//-0.25*PI < x < 0.25*PI
			y = x - 1.6666673124e-01f*x*x*x + 8.3330580965e-03f*x*x*x*x*x - 1.9618441002e-04f*x*x*x*x*x*x*x - 2.2868576366e-07f*x*x*x*x*x*x*x*x*x;
		} else {
			x -= -0.5f*PI;
			y = 1.0f - 4.9999979138e-01f*x*x + 4.1666436940e-02f*x*x*x*x - 1.3899283949e-03f*x*x*x*x*x*x + 2.6317742595e-05f*x*x*x*x*x*x*x*x;
			y = -y;
		}
	} else {
			x -= 0.5f*PI;
			y = 1.0f - 4.9999979138e-01f*x*x + 4.1666436940e-02f*x*x*x*x - 1.3899283949e-03f*x*x*x*x*x*x + 2.6317742595e-05f*x*x*x*x*x*x*x*x;
	}

	return y;
}


}
