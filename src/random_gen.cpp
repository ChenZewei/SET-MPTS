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
	cout<<fixed;
	cout<<"original prob:"<<prob<<endl;
	fraction_t p(0.1);
	cout<<p.get_num()<<endl;
	cout<<p.get_den()<<endl;
	while(prob != floor(prob))
	{
		prob *= 10;
		i *= 10;
	}
	cout<<"current prob:"<<prob<<endl;
	cout<<i<<endl;
	for(uint i = 0; i < 20; i++)
		cout<<uniform_integral_gen(1, i)<<endl;
	if(uniform_integral_gen(1, i) <= prob)
		return true;
	else 
		return false;
}
