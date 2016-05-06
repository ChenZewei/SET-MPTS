#include "random_gen.h"



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
