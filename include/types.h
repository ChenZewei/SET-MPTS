#ifndef TYPES_H
#define TYPES_H

#include <string.h>
//including GNU Multiple Precision Arithmetic Library
#include <gmpxx.h>
#include <vector>

#define uint unsigned int
#define ulong unsigned long

using namespace std;

typedef mpz_class int_t;		//multiple precision integer
typedef mpq_class fraction_t;		//multiple precision rational number
typedef mpf_class floating_t;		//multiple precision float number

typedef struct
{
	double min;
	double max;
}range;

typedef vector<int> int_set;
typedef vector<double> double_set;
typedef vector<range> range_set;

#endif
