#include <cassert>
#include <cmath>
#include <cstdio>
#include <random>

#include <omp.h>

#include <functional>

#include <string>

#include "../libminitrig/_misc.hpp"
#include "../libminitrig/include-f32.hpp"


namespace optimizing {


static float const sin_near_coeffs5_1[5] = { 1.0f, -1.0f/6.0f, 1.0f/120.0f, -1.0f/5040.0f, 1.0f/362880.0f };
static float const sin_near_coeffs5_2[5] = { 1.0000000000e+00f, -1.6666673124e-01f, 8.3330580965e-03f, -1.9618441002e-04f, -2.2868576366e-07f };

//-0.25*PI < x < 0.25*PI
static float sin_near5(float x, float const A[5]) {
	return A[0]*x + A[1]*x*x*x + A[2]*x*x*x*x*x + A[3]*x*x*x*x*x*x*x + A[4]*x*x*x*x*x*x*x*x*x;
}


static float const sin_far_coeffs_5_1[5] = { 1.0f, -0.5f, 1.0f/24.0f, -1.0f/720.0f, 1.0f/40320.0f };
static float const sin_far_coeffs_5_2[5] = { 1.0e+00f, -4.9999979138e-01f, 4.1666436940e-02f, -1.3899283949e-03f, 2.6317742595e-05f };

//0.25*PI < x < 0.5*PI
static float sin_far5(float x, float const A[5]) {
	x -= F32_HPI;
	return A[0] + A[1]*x*x + A[2]*x*x*x*x + A[3]*x*x*x*x*x*x + A[4]*x*x*x*x*x*x*x*x;
}


static float const arcsin_coeffs4_1[4] = { 1.5707288f, -0.2121144f, 0.0742610f, -0.0187293f }; //original
static float const arcsin_coeffs4_2[4] = { 1.5707583427e+00f, -2.1287518740e-01f, 7.6897978783e-02f, -2.0892569795e-02f };
static float const arcsin_coeffs5[5]   = { 1.5707583427e+00f, -2.1287535131e-01f, 7.6900094748e-02f, -2.0897671580e-02f, 3.3740022900e-06f };

static float arcsin4(float x, float const A[4]) {
	float negate = x<0.0f ? 1.0f : 0.0f;
	x = abs(x);
	float ret = A[0] + A[1]*x + A[2]*x*x + A[3]*x*x*x;
	ret = F32_HPI - sqrtf(1.0f-x)*ret;
	return ret - 2.0f * negate * ret;
}
static float arcsin5(float x, float const A[5]) {
	float negate = x<0.0f ? 1.0f : 0.0f;
	x = abs(x);
	float ret = A[0] + A[1]*x + A[2]*x*x + A[3]*x*x*x + A[4]*x*x*x*x;
	ret = F32_HPI - sqrtf(1.0f-x)*ret;
	return ret - 2.0f * negate * ret;
}


static float const arccos_coeffs4_1[5] = { 1.5707288f, -0.2121144f, 0.0742610f, -0.0187293f }; //original
static float const arccos_coeffs4_2[5] = { 1.5707582235e+00f, -2.1287369728e-01f, 7.6894350350e-02f, -2.0889861509e-02f };
static float const arccos_coeffs5[5]   = { 1.5707595348e+00f, -2.1292459965e-01f, 7.7204361558e-02f, -2.1477775648e-02f, 3.3965636976e-04f };

static float arccos4(float x, float const A[4]) {
	float negate = x<0.0f ? 1.0f : 0.0f;
	x = abs(x);
	float ret = A[0] + A[1]*x + A[2]*x*x + A[3]*x*x*x;
	ret *= sqrt(1.0f-x);
	ret -= 2.0f * negate * ret;
	return negate*F32_PI + ret;
}
static float arccos5(float x, float const A[5]) {
	float negate = x<0.0f ? 1.0f : 0.0f;
	x = abs(x);
	float ret = A[0] + A[1]*x + A[2]*x*x + A[3]*x*x*x + A[4]*x*x*x*x;
	ret *= sqrt(1.0f-x);
	ret -= 2.0f * negate * ret;
	return negate*F32_PI + ret;
}


}


inline static float get_max_err(std::vector<float> const& xs, std::vector<float> const& gts, std::function<float(float,float const*)> const& fn_mini,float const fn_params[]) {
	float max_err = 0.0f;
	for (int i=0;i<(int)xs.size();++i) {
		float err = fabsf( gts[(size_t)i] - fn_mini(xs[(size_t)i],fn_params) );

		if (err>max_err) {
			max_err = err;
		}
	}
	return max_err;
}

inline static void optimize(char const* name, float(*fn)(float,float const*),float const fn_params[],size_t fn_params_len, float low,float high, size_t steps) {
	assert(steps>0);
	assert(high>=low);

	std::vector<float> xs(steps+1);
	for (size_t i=0;i<=steps;++i) {
		xs[i] = ((float)(i)/(float)(steps))*(high-low) + low;
	}

	std::vector<float> gts(steps+1);
	{
		std::string filename = "cache/"+std::string(name)+"_"+std::to_string(steps+1)+"_"+std::to_string(low)+"_"+std::to_string(high)+".f32";
		FILE* file = fopen(filename.c_str(),"rb");
		if (file==nullptr) {
			fprintf(stderr,"Could not open cache file \"%s\".  Run the accuracy test first to generate.\n",filename.c_str());
			return;
		}
		fread(gts.data(), sizeof(float),steps+1, file);
		fclose(file);
	}

	printf("Optimizing %s:\n",name);

	std::vector<float> fn_params_best(fn_params_len);
	memcpy(fn_params_best.data(),fn_params, fn_params_len*sizeof(float));

	#define THREADS 11

	std::mt19937 rngs[THREADS]; for (size_t i=0;i<THREADS;++i) rngs[i].seed((unsigned)(i*356));
	std::vector<float> fn_params_temp[THREADS];
	for (size_t i=0;i<THREADS;++i) fn_params_temp[i].resize(fn_params_len);

	float best_err = get_max_err(xs, gts, fn,fn_params);

	int steps_since_last_improvement = 0;
	float step = 1e-1f;
	size_t iters = 0;
	bool improved = false;
	while (step>1e-9f) {
		float errs[THREADS];
		#pragma omp parallel num_threads(THREADS)
		{
			int id = omp_get_thread_num();

			memcpy(fn_params_temp[id].data(),fn_params_best.data(), fn_params_len*sizeof(float));

			int op = std::uniform_int_distribution<int>(0,4)(rngs[id]); //0,1,2,3,4
			switch (op) {
				case 0: {
					size_t index = std::uniform_int_distribution<size_t>(0,fn_params_len-1)(rngs[id]);
					float delta = std::uniform_real_distribution<float>(-step,step)(rngs[id]);
					fn_params_temp[id][index] += delta;
					break;
				}
				case 1: {
					size_t index1 = std::uniform_int_distribution<size_t>(0,fn_params_len-1)(rngs[id]);
					float delta1 = std::uniform_real_distribution<float>(-step,step)(rngs[id]);
					fn_params_temp[id][index1] += delta1;
					size_t index2 = std::uniform_int_distribution<size_t>(0,fn_params_len-1)(rngs[id]);
					float delta2 = std::uniform_real_distribution<float>(-step,step)(rngs[id]);
					fn_params_temp[id][index2] += delta2;
					break;
				}
				default:
					for (size_t i=0;i<fn_params_len;++i) {
						float delta = std::uniform_real_distribution<float>(-step,step)(rngs[id]);
						fn_params_temp[id][i] += delta;
					}
					break;
			}

			float err = get_max_err(xs, gts, fn,fn_params_temp[id].data());
			errs[id] = err;

			/*#pragma omp critical
			{
				printf("thread %d: [ ",id);
				for (float param : fn_params_temp[id]) {
					printf("%f ",(double)param);
				}
				printf("] -> %f\n",(double)err);
			}*/
		}

		#pragma omp barrier

		//printf("\r");for (size_t i=0;i<THREADS;++i) printf("%f ",errs[i]); printf("\n");
		bool improved_temp = false;
		for (size_t i=0;i<THREADS;++i) {
			if (errs[i]<best_err) {
				best_err = errs[i];
				memcpy(fn_params_best.data(),fn_params_temp[i].data(), fn_params_len*sizeof(float));
				improved_temp = true;
			} else if (errs[i]==best_err) {
				memcpy(fn_params_best.data(),fn_params_temp[i].data(), fn_params_len*sizeof(float));
			}
		}
		if (improved_temp) {
			steps_since_last_improvement = 0;
			step *= 1.0f/0.9f;
			printf("\r  Best err: %.10e; Step: %.10e     ",best_err,(double)step); fflush(stdout);
			improved = true;
		} else {
			if (++steps_since_last_improvement==10000) {
				step *= 0.6f;
				steps_since_last_improvement = 0;
				printf("\r  Best err: %.10e; Step: %.10e     ",best_err,(double)step); fflush(stdout);
			}
		}

		++iters;
	}

	printf("\nCompleted:\n");
	if (improved) {
		for (float param : fn_params_best) {
			printf("\n  %.10e\n",(double)param);
		}
	} else {
		printf("Did not improve.\n");
	}
}

int main(int /*argc*/, char* /*argv*/[]) {
	//optimize("sin",optimizing::sin_near5,optimizing::sin_near_coeffs5_1,5, 0.0f,F32_QPI, 10000 );
	//optimize("sin",optimizing::sin_near5,optimizing::sin_near_coeffs5_2,5, 0.0f,F32_QPI, 10000 );
	//optimize("sin",optimizing::sin_far5,optimizing::sin_far_coeffs_5_1,5, F32_QPI,F32_HPI, 10000 );
	//optimize("sin",optimizing::sin_far5,optimizing::sin_far_coeffs_5_2,5, F32_QPI,F32_HPI, 10000 );

	//optimize("arcsin",optimizing::arcsin4,optimizing::arcsin_coeffs4_1,4, -1.0f,1.0f, 10000 );
	//optimize("arcsin",optimizing::arcsin4,optimizing::arcsin_coeffs4_2,4, -1.0f,1.0f, 10000 );
	//optimize("arcsin",optimizing::arcsin5,optimizing::arcsin_coeffs5,5, -1.0f,1.0f, 10000 );

	//optimize("arccos",optimizing::arccos4,optimizing::arccos_coeffs4_1,4, -1.0f,1.0f, 10000 );
	//optimize("arccos",optimizing::arccos4,optimizing::arccos_coeffs4_2,4, -1.0f,1.0f, 10000 );
	//optimize("arccos",optimizing::arccos5,optimizing::arccos_coeffs5,5, -1.0f,1.0f, 10000 );

	getchar();

	return 0;
}
