#include <cassert>
#include <cmath>
#include <cstdio>

#include <string>
#include <vector>

#include "../libminitrig/include-f32.hpp"
#include "../libminitrig/include-highprec.hpp"


static void test( char const* name, float(*fn_mini)(float), float(*fn_gt)(float), float low,float high, size_t steps) {
	assert(steps>0);
	assert(high>=low);

	std::vector<float> xs(steps+1);
	for (size_t i=0;i<=steps;++i) {
		xs[i] = ((float)(i)/(float)(steps))*(high-low) + low;
	}

	std::vector<float> gts(steps+1);
	std::string filename = "cache/"+std::string(name)+"_"+std::to_string(steps+1)+"_"+std::to_string(low)+"_"+std::to_string(high)+".f32";
	FILE* file = fopen(filename.c_str(),"rb");
	if (file==nullptr) {
		for (size_t i=0;i<=steps;++i) {
			gts[i] = fn_gt(xs[i]);
		}
		file = fopen(filename.c_str(),"wb");
		fwrite(gts.data(), sizeof(float),steps+1, file);
		fclose(file);
		file = fopen(filename.c_str(),"rb");
	}
	fread(gts.data(), sizeof(float),steps+1, file);
	fclose(file);

	float max_err = 0.0f;
	float at_x;
	for (size_t i=0;i<=steps;++i) {
		float err = fabsf( gts[i] - fn_mini(xs[i]) );
		if (err>max_err) {
			max_err = err;
			at_x = xs[i];
		}
	}
	printf("Max err %s in range [%f,%f]: %e (at x=%f)\n",name,(double)(low),(double)(high),(double)(max_err),(double)at_x);
}

template <typename T> static T clamp(T x, T low,T high) {
	if (x<low) return low;
	if (x>high) return high;
	return x;
}
static void graph( float(*fn)(float), float low,float high, size_t height=11) {
	size_t data[80];
	for (size_t i=0;i<80;++i) {
		float x = ((float)(i)/80.0f)*(high-low) + low;

		float temp = 0.5f*fn(x) + 0.5f;
		temp *= (float)(height);
		data[i] = (size_t)clamp<int>((int)roundf(temp), 0,(int)height);
		data[i] = height - data[i];
	}

	for (size_t j=0;j<=height;++j) {
		for (size_t i=0;i<80;++i) {
			char c = ' ';
			if (j==0||j==height) c='-';
			else if (j==height/2) c='=';
			if (j==data[i]) c='*';
			printf("%c",c);
		}
	}
}

int main(int /*argc*/, char* /*argv*/[]) {
	#define PI 3.14159265358979323f
	#define N 10000
	test( "cos", minitrig::cos, highprec::cos, -1.0f*PI,1.0f*PI, N);
	test( "sin", minitrig::sin, highprec::sin, -1.0f*PI,1.0f*PI, N);
	test( "arccos", minitrig::arccos, highprec::arccos, -1.0f,1.0f, N);
	test( "arcsin", minitrig::arcsin, highprec::arcsin, -1.0f,1.0f, N);

	printf("%f %f %f %f %f\n",minitrig::sin(0),minitrig::sin(0.5f*PI),minitrig::sin(PI),minitrig::sin(1.5f*PI),minitrig::sin(2.0f*PI));

	graph( minitrig::sin, -2.0f*PI,2.0f*PI );

	getchar();

	return 0;
}
