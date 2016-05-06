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


static inline int_t ceiling(const int_t &numer, const int_t &denom)
{
	int_t result;
	mpz_cdiv_q(result.get_mpz_t(), numer.get_mpz_t(), denom.get_mpz_t());
	return result;
}


static inline int_t round_up(const fraction_t &f)
{
	int_t result;
	mpz_cdiv_q(result.get_mpz_t(), f.get_num_mpz_t(), f.get_den_mpz_t());
	return result;
}


#endif
