#ifndef RANDOM_GEN_H
#define RAMDOM_GEN_H

#include <random>
#include "types.h"
using namespace std;

class Random_Gen
{
	private:
		static default_random_engine generator;
	public:
		//Random_Gen();
		static double exponential_gen(double lambda);
		static int uniform_integral_gen(int min, int max);
		static ulong uniform_ulong_gen(ulong min, ulong max);
		static double uniform_real_gen(double min, double max);
		static bool probability(double prob);
};

#endif
