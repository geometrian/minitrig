#include "_misc.hpp"


namespace minitrig {


float arcsin(float x) {
	//Handbook of Mathematical Functions via http://http.developer.nvidia.com/Cg/asin.html plus my improved coefficients

	//Absolute error <= 5.26e-5

	float negate = x<0.0f ? 1.0f : 0.0f;
	x = abs(x);

	float ret = 1.5707583427e+00f - 2.1287551522e-01f*x + 7.6898902655e-02f*x*x - 2.0892916247e-02f*x*x*x;
	ret = F32_HPI - sqrt(1.0f-x)*ret;

	return ret - 2.0f * negate * ret;
}


}
