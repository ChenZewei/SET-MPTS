#ifndef RANDOM_GEN_H
#define RAMDOM_GEN_H

#include <random>
#include "types.h"
using namespace std;

static default_random_engine generator(time(NULL));

double exponential_gen(double lambda);

<<<<<<< HEAD
int uniform_integral_gen(long min, long max)
{
	uniform_int_distribution<long> distribution(min, max);
	return distribution(generator);
}
=======
int uniform_integral_gen(int min, int max);
>>>>>>> implement class Chart to draw line plot using mathGL.

#endif
