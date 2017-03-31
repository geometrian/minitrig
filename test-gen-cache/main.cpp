#include <cassert>
#include <cmath>
#include <cstdio>

#include <string>
#include <vector>

#include "../libminitrig/include-f32.hpp"
#include "../libminitrig/include-highprec.hpp"

#include "../libminitrig/_misc.hpp"


static void gen_cache(char const* name, float(*fn_mini)(float), float(*fn_gt)(float), float low,float high, size_t steps) {
	assert(steps>0);
	assert(high>=low);

	std::vector<float> xs(steps+1);
	for (size_t i=0;i<=steps;++i) xs[i]=((float)(i)/(float)(steps))*(high-low) + low;

	std::vector<float> gts(steps+1);
	std::string filename = ".cache/"+std::string(name)+"_"+std::to_string(steps+1)+"_"+std::to_string(low)+"_"+std::to_string(high)+".f32";
	printf("Cache entry \"%s\" ",filename.c_str());
	FILE* file = fopen(filename.c_str(),"rb");
	if (file==nullptr) {
		printf("(generating...)"); fflush(stdout);

		//Disabled.  It seems the arbitrary precision library isn't threadsafe.
		//#pragma omp parallel for
		for (int i=0;i<=(int)steps;++i) {
			gts[(size_t)i] = fn_gt(xs[(size_t)i]);
		}

		file = fopen(filename.c_str(),"wb");
		fwrite(gts.data(), sizeof(float),steps+1, file);

		printf("\rCache entry \"%s\" (completed!)   \n",filename.c_str());
	} else {
		printf("(already exists!)\n");
	}
	fclose(file);
}

int main(int /*argc*/, char* /*argv*/[]) {
	#define N 10000
	gen_cache( "sin", minitrig::sin, highprec::sin, 0.0f,F32_QPI, N);
	gen_cache( "sin", minitrig::sin, highprec::sin, F32_QPI,F32_HPI, N);
	gen_cache( "cos", minitrig::cos, highprec::cos, -F32_PI,F32_PI, N);
	gen_cache( "sin", minitrig::sin, highprec::sin, -F32_PI,F32_PI, N);
	gen_cache( "arccos", minitrig::arccos, highprec::arccos, -1.0f,1.0f, N);
	gen_cache( "arcsin", minitrig::arcsin, highprec::arcsin, -1.0f,1.0f, N);

	gen_cache( "sin", minitrig::sin, highprec::sin, -F32_2PI,2.0f*F32_2PI, N);
	gen_cache( "cos", minitrig::cos, highprec::cos, -F32_2PI,2.0f*F32_2PI, N);

	//printf("%f %f %f %f %f\n",minitrig::sin(0),minitrig::sin(0.5f*PI),minitrig::sin(PI),minitrig::sin(1.5f*PI),minitrig::sin(2.0f*PI));

	//graph( minitrig::sin, -2.0f*PI,2.0f*PI );

	getchar();

	return 0;
}
