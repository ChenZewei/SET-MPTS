#ifndef RANDOM_GEN_H
#define RAMDOM_GEN_H

#include <random>
#include "types.h"
using namespace std;

//static default_random_engine generator(time(NULL));
class Random_Gen
{
	private:
		default_random_engine generator;
	public:
		Random_Gen();
		double exponential_gen(double lambda);
		int uniform_integral_gen(int min, int max);
		bool probability(double prob);
};

#endif
