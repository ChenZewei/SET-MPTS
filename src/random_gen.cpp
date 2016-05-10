#include "random_gen.h"
#include <iostream>


default_random_engine Random_Gen::generator(time(NULL));

double Random_Gen::exponential_gen(double lambda)
{
	exponential_distribution<double> distribution(lambda);
	return distribution(generator);
}

int Random_Gen::uniform_integral_gen(int min, int max)
{
	uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

bool Random_Gen::probability(double prob)
{
	int i = 1;
	while(prob != floor(prob))
	{
		prob *= 10;
		i *= 10;
	}
	if(uniform_integral_gen(1, i) <= prob)
		return true;
	else 
		return false;
}
