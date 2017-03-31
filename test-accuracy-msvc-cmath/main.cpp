#include <cassert>
#include <cmath>
#include <cstdio>

#include <string>
#include <vector>

#include "../libminitrig/include-f32.hpp"
#include "../libminitrig/include-highprec.hpp"

#include "../libminitrig/_misc.hpp"


static void gen_accuracy_data(char const* cachename, char const* fnname, float(*fn)(float), float low,float high, size_t steps) {
	assert(steps>0);
	assert(high>=low);

	std::vector<float> xs(steps+1);
	for (size_t i=0;i<=steps;++i) xs[i]=((float)(i)/(float)(steps))*(high-low) + low;

	std::vector<float> gts(steps+1);
	{
		std::string filename = ".cache/"+std::string(cachename)+"_"+std::to_string(steps+1)+"_"+std::to_string(low)+"_"+std::to_string(high)+".f32";
		FILE* file = fopen(filename.c_str(),"rb");
		if (file==nullptr) {
			fprintf(stderr,"Could not open cache file \"%s\".  Run the `test-gen-cache` first to generate.\n",filename.c_str());
			getchar();
			return;
		}
		fread(gts.data(), sizeof(float),steps+1, file);
		fclose(file);
	}

	std::vector<float> errs(steps+1);
	for (size_t i=0;i<=steps;++i) errs[i]=fabsf( gts[i] - fn(xs[i]) );

	FILE* file = fopen( (".accuracy/"+std::string(fnname)+".f32").c_str(), "wb" );
	fwrite(&low, sizeof(float),1, file);
	fwrite(&high, sizeof(float),1, file);
	fwrite(errs.data(), sizeof(float),errs.size(), file);
	fclose(file);
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
	#define N 10000
	gen_accuracy_data( "sin", "sin-32f-minitrig", minitrig::sin, -F32_2PI,2.0f*F32_2PI, N);
	gen_accuracy_data( "sin", "sin-32f-msvc",            ::sinf, -F32_2PI,2.0f*F32_2PI, N);

	gen_accuracy_data( "cos", "cos-32f-minitrig", minitrig::cos, -F32_2PI,2.0f*F32_2PI, N);
	gen_accuracy_data( "cos", "cos-32f-msvc",            ::cosf, -F32_2PI,2.0f*F32_2PI, N);

	gen_accuracy_data( "arcsin", "arcsin-32f-minitrig", minitrig::arcsin, -1.0f,1.0f, N);
	gen_accuracy_data( "arcsin", "arcsin-32f-msvc",              ::asinf, -1.0f,1.0f, N);

	gen_accuracy_data( "arccos", "arccos-32f-minitrig", minitrig::arccos, -1.0f,1.0f, N);
	gen_accuracy_data( "arccos", "arccos-32f-msvc",              ::acosf, -1.0f,1.0f, N);

	return 0;
}
