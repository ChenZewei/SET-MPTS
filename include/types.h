#ifndef TYPES_H
#define TYPES_H

#include <string.h>
//including GNU Multiple Precision Arithmetic Library
#include <gmpxx.h>
#include <vector>

#define uint unsigned int
#define ulong unsigned long

#define MAX_LONG 0xffffffffffffffff
#define MAX_INT  0xffffffff

#define P_EDF 0
#define BCL_FTP 1
#define BCL_EDF 2
#define WF_DM 3
#define WF_EDF 4
using namespace std;

typedef mpz_class int_t;		//multiple precision integer
typedef mpq_class fraction_t;		//multiple precision rational number
typedef mpf_class floating_t;		//multiple precision float number

typedef struct
{
	double min;
	double max;
}Range;

typedef struct
{
	double x;
	double y;
}Result;

typedef vector<int> Int_Set;
typedef vector<double> Double_Set;
typedef vector<Range> Range_Set;
typedef vector<Result> Result_Set;


#endif
