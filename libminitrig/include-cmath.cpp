#include "include-cmath.hpp"

#include <cmath>


namespace cmath {


float cos(float x) { return ::cosf(x); }
double cos(double x) { return ::cos(x); }
float sin(float x) { return ::sinf(x); }
double sin(double x) { return ::sin(x); }

float arccos(float x) { return ::acosf(x); }
double arccos(double x) { return ::acos(x); }
float arcsin(float x) { return ::asinf(x); }
double arcsin(double x) { return ::asin(x); }


}
