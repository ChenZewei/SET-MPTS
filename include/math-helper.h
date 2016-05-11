#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#include "types.h"

static inline ulong ceiling(ulong numer, ulong denom)
{
	if (numer % denom == 0)
		return numer / denom;
	else
		return (numer / denom) + 1;
}

#endif
