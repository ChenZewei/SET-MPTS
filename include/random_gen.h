#ifndef RANDOM_GEN_H
#define RAMDOM_GEN_H

#include <random>
#include "types.h"
using namespace std;

default_random_engine generator(time(NULL));

double exponential_gen(double lambda)
{
	exponential_distribution<double> distribution(lambda);
	return distribution(generator);
}

int uniform_integral_gen(int min, int max)
{
	uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}







#endif
