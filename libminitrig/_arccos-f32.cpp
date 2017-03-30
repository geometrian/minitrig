#include "_misc.hpp"


namespace minitrig {


static float const arccos_coeffs4_1[5] = { 1.5707288f, -0.2121144f, 0.0742610f, -0.0187293f }; //original
static float const arccos_coeffs4_2[5] = { 1.5707582235e+00f, -2.1287369728e-01f, 7.6894350350e-02f, -2.0889861509e-02f };
static float const arccos_coeffs5[5]   = { 1.5707595348e+00f, -2.1292459965e-01f, 7.7204361558e-02f, -2.1477775648e-02f, 3.3965636976e-04f };

float arccos(float x) {
	//Handbook of Mathematical Functions via http://http.developer.nvidia.com/Cg/acos.html plus my improved coefficients

	//Absolute error <= 4.24e-5

	float negate = x<0.0f ? 1.0f : 0.0f;
	x = abs(x);

	//float ret = ((-0.0187293f*x + 0.0742610f)*x - 0.2121144f)*x + 1.5707288f;
	//float ret = arccos_coeffs4_1[0] + arccos_coeffs4_1[1]*x + arccos_coeffs4_1[2]*x*x + arccos_coeffs4_1[3]*x*x*x;
	//float ret = arccos_coeffs4_2[0] + arccos_coeffs4_2[1]*x + arccos_coeffs4_2[2]*x*x + arccos_coeffs4_2[3]*x*x*x;
	float ret = arccos_coeffs5[0] + arccos_coeffs5[1]*x + arccos_coeffs5[2]*x*x + arccos_coeffs5[3]*x*x*x + arccos_coeffs5[4]*x*x*x*x;
	ret *= sqrt(1.0f-x);
	ret -= 2.0f * negate * ret;

	return negate*F32_PI + ret;
}


}
