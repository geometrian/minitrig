#include "_misc.hpp"


namespace minitrig {


static float const arcsin_coeffs4_1[4] = { 1.5707288f, -0.2121144f, 0.0742610f, -0.0187293f }; //original
static float const arcsin_coeffs4_2[4] = { 1.5707583427e+00f, -2.1287518740e-01f, 7.6897978783e-02f, -2.0892569795e-02f };
static float const arcsin_coeffs5[5]   = { 1.5707583427e+00f, -2.1287535131e-01f, 7.6900094748e-02f, -2.0897671580e-02f, 3.3740022900e-06f };

float arcsin(float x) {
	//Handbook of Mathematical Functions via http://http.developer.nvidia.com/Cg/asin.html plus my improved coefficients

	//Absolute error <= 5.26e-5

	float negate = x<0.0f ? 1.0f : 0.0f;
	x = abs(x);

	//float ret = arcsin_coeffs4_1[0] + arcsin_coeffs4_1[1]*x + arcsin_coeffs4_1[2]*x*x + arcsin_coeffs4_1[3]*x*x*x;
	//float ret = arcsin_coeffs4_2[0] + arcsin_coeffs4_2[1]*x + arcsin_coeffs4_2[2]*x*x + arcsin_coeffs4_2[3]*x*x*x;
	float ret = arcsin_coeffs5[0] + arcsin_coeffs5[1]*x + arcsin_coeffs5[2]*x*x + arcsin_coeffs5[3]*x*x*x + arcsin_coeffs5[4]*x*x*x*x;
	ret = F32_HPI - sqrt(1.0f-x)*ret;

	return ret - 2.0f * negate * ret;
}


}
