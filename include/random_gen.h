#ifndef RANDOM_GEN_H
#define RAMDOM_GEN_H

#include <random>
#include "types.h"
using namespace std;

static default_random_engine generator(time(NULL));

double exponential_gen(double lambda);

int uniform_integral_gen(int min, int max);


#endif
