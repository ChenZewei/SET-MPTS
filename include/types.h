#ifndef TYPES_H
#define TYPES_H

#include <string.h>
//including GNU Multiple Precision Arithmetic Library
#include <gmpxx.h>

#define uint unsigned int
#define ulong unsigned long


typedef mpz_class int_t;		//multiple precision integer
typedef mpq_class fraction_t;		//multiple precision rational number
typedef mpf_class float_t;		//multiple precision floating number
#endif