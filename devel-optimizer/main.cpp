#include <cassert>
#include <cmath>
#include <cstdio>
#include <random>

#include <functional>

#include "../libminitrig/include.hpp"

#define PI 3.14159265358979323


double get_max_err(std::function<float(float)> const& fn_dumb, std::function<double(double)> const& fn_math, double low,double high, size_t steps) {
	assert(steps>0);
	assert(high>=low);
	double max_err = 0.0;
	for (size_t i=0;i<=steps;++i) {
		double x = ((double)(i)/(double)(steps))*(high-low) + low;
		double a=fn_math(x), b=(double)fn_dumb((float)x);
		double err = abs( a - b );
		if (err>max_err) {
			max_err = err;
		}
	}
	return max_err;
}

void optimize(std::function<double(double)> const& fn_math, double low,double high, size_t steps) {
	std::mt19937 rng;

	#define N 5
	float best_A[N] = {
		//sin (near)
		//1.0f, -1.0f/6.0f, 1.0f/120.0f, -1.0f/5040.0f, 1.0f/362880.0f
		1.0000000000e+00f, -1.6666673124e-01f, 8.3330580965e-03f, -1.9618441002e-04f, -2.2868576366e-07f
		//0x1p+0, -0x1.555568p-3, 0x1.1116EAp-7, -0x1.A0EEB8p-13, 0x1.0966D8p-19

		//sin (far)
		//1.0f, -0.5f, 1.0f/24.0f, -1.0f/720.0f, 1.0f/40320.0f
		//1.0e+00f, -4.9999979138e-01f, 4.1666436940e-02f, -1.3899283949e-03f, 2.6317742595e-05f

		//arcsin
		//1.5707440376e+00f, -2.1242626011e-01f, 7.5070030987e-02f, -1.9178723916e-02f
		//1.5707541704e+00f, -2.1270722151e-01f, 7.5888827443e-02f, -1.9007723778e-02f, -1.0771057568e-03f

		//arccos
		//1.5707437992e+00f, -2.1242122352e-01f, 7.5053036213e-02f, -1.9164543599e-02f
		//1.5707540512e+00f, -2.1270623803e-01f, 7.5885929167e-02f, -1.9007002935e-02f, -1.0748786153e-03f
	};

	double best_err = std::numeric_limits<double>::infinity();

	int steps_since_last_improvement = 0;
	float step = 1e-1f;
	size_t iters = 0;
	while (step>1e-12f) {
		float A[N]; memcpy(A,best_A, N*sizeof(float));

		if (iters>0) {
			size_t index = std::uniform_int_distribution<size_t>(0,N-1)(rng);
			for (size_t i=0;i<N;++i) {
				float delta = std::uniform_real_distribution<float>(-step,step)(rng);
				A[i] += delta;
			}
		}

		std::function<float(float)> fn_dumb = [&](float x) -> float {
			//return (A[0]*x*x*x + A[1]*x*x + A[2]*x + A[3]) / (A[4]*x*x*x + A[5]*x*x + A[6]*x + A[7]);
			/*float x_accum = 1.0f;
			float accum = 0.0f;
			for (size_t i=0;i<N;++i) {
				accum += A[i]*x_accum;
				x_accum *= x*x;
			}
			return accum;*/
			return A[0]*x + A[1]*x*x*x + A[2]*x*x*x*x*x + A[3]*x*x*x*x*x*x*x + A[4]*x*x*x*x*x*x*x*x*x;

			//x -= 0.5f*(float)PI;
			//return A[0] + A[1]*x*x + A[2]*x*x*x*x + A[3]*x*x*x*x*x*x + A[4]*x*x*x*x*x*x*x*x;

			/*float negate = x<0.0f ? 1.0f : 0.0f;
			x = abs(x);
			float ret = A[0] + A[1]*x + A[2]*x*x + A[3]*x*x*x + A[4]*x*x*x*x;
			ret = 0.5f*(float)PI - sqrtf(1.0f-x)*ret;
			return ret - 2.0f * negate * ret;*/

			/*float negate = x<0.0f ? 1.0f : 0.0f;
			x = abs(x);
			float ret = A[0] + A[1]*x + A[2]*x*x + A[3]*x*x*x + A[4]*x*x*x*x;
			ret *= sqrt(1.0f-x);
			ret -= 2.0f * negate * ret;
			return negate*(float)PI + ret;*/
		};
		double err = get_max_err( fn_dumb, fn_math, low,high, steps);

		if (err<best_err) {
			best_err = err;
			memcpy(best_A,A, N*sizeof(float));
			steps_since_last_improvement = 0;
			step *= 1.0f/0.9f;
			printf("\rBest err: %e; Step: %e     ",best_err,(double)step); fflush(stdout);
		} else {
			if (++steps_since_last_improvement==1000) {
				step *= 0.9f;
				steps_since_last_improvement = 0;
				printf("\rBest err: %e; Step: %e     ",best_err,(double)step); fflush(stdout);
			}
		}

		++iters;
	}

	printf("\nDone\n");
	for (size_t i=0;i<N;++i) {
		printf("  %.10e\n",best_A[i]);
	}
}

double math_fn(double x) {
	//return ::sin(x);
	return ::sin(x);
}

int main(int argc, char* argv[]) {
	optimize( math_fn, 0.0,0.25*PI, 10000 );
	//optimize( math_fn, 0.25*PI,0.5*PI, 10000 );

	getchar();

	return 0;
}
