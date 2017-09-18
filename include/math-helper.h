#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#include "types.h"

static inline ulong ceiling(ulong numer, ulong denom)
{
	if (numer % denom == 0)
		return (numer / denom);
	else
		return (numer / denom) + 1;
}

static inline ulong ceiling(ulong numer, double denom)
{
	if(numer/denom < (long(numer/denom) + _EPS))
		return (ulong)(numer / denom);
	else
		return (ulong)(numer / denom) + 1;
}
/*
bool double_equal(double a, double b)
{
	return fabs(a - b) < EPS;
}
*/
#endif
