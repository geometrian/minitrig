#include "_misc.hpp"


namespace minitrig {


float arccos(float x) {
	//Handbook of Mathematical Functions via http://http.developer.nvidia.com/Cg/acos.html plus my improved coefficients

	//Absolute error <= 4.24e-5

	float negate = x<0.0f ? 1.0f : 0.0f;
	x = abs(x);

	//float ret = ((-0.0187293f*x + 0.0742610f)*x - 0.2121144f)*x + 1.5707288f;
	//float ret = 1.5707437992e+00f - 2.1242122352e-01f*x + 7.5053036213e-02f*x*x - 1.9164543599e-02f*x*x*x;
	float ret = 1.5707540512e+00f - 2.1270623803e-01f*x + 7.5885929167e-02f*x*x - 1.9007002935e-02f*x*x*x - 1.0748786153e-03f*x*x*x*x;
	ret *= sqrt(1.0f-x);
	ret -= 2.0f * negate * ret;

	return negate*PI + ret;
}


}
