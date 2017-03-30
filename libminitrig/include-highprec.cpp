#include "include-highprec.hpp"

#pragma warning(push)
#pragma warning(disable:4267)
#include "../3rdparty/Precision/fprecision.h"
#pragma warning(pop)


namespace highprec {


#define PREC 128

float cos(float x) { return (float)(cos((double)(x))); }
double cos(double x) {
	float_precision temp(x,PREC);
	temp = ::cos(temp);
	return (float)(temp);
}
float sin(float x) { return (float)(sin((double)(x))); }
double sin(double x) {
	float_precision temp(x,PREC);
	temp = ::sin(temp);
	return (float)(temp);
}

float arccos(float x) { return (float)(arccos((double)(x))); }
double arccos(double x) {
	float_precision temp(x,PREC);
	temp = ::acos(temp);
	return (float)(temp);
}
float arcsin(float x) { return (float)(arcsin((double)(x))); }
double arcsin(double x) {
	float_precision temp(x,PREC);
	temp = ::asin(temp);
	return (float)(temp);
}


}
