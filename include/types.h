#ifndef TYPES_H
#define TYPES_H

#include <string.h>
//including GNU Multiple Precision Arithmetic Library
#include <gmpxx.h>
#include <vector>
#include <list>
#include <set>
#include <math.h>
#include <time.h>
#include <assert.h>
//#include "iteration-helper.h"
//#include "math-helper.h"

#define uint unsigned int
#define ulong unsigned long

#define MAX_LONG 0xffffffffffffffff
#define MAX_INT  0xffffffff
#define _EPS 0.000001

//output file format
#define PNG 	0x01
#define EPS 	0x02
#define SVG 	0x04
#define TGA 	0x08
#define JSON 	0x10

//VNode type
#define P_NODE 0x02
#define C_NODE 0x04
#define J_NODE 0x08

#define S_NODE 0x00
#define E_NODE 0x01

//Graph type
#define G_TYPE_P 0 //Paralleled
#define G_TYPE_C 1 //Conditional

#define P_EDF 0
#define BCL_FTP 1
#define BCL_EDF 2
#define WF_DM 3
#define WF_EDF 4
#define RTA_GFP 5
#define FF_DM 6
#define LP_PFP 7
#define LP_GFP 8
#define RO_PFP 9
#define ILP_SPINLOCK 10
#define GEDF_NON_PREEMPT 11
#define PFP_GS 12


#define TPS_TASK_MODEL 0 //Three Parameters Sporadic Task Model
#define DAG_TASK_MODEL 1

#define PRIORITY 0

#define INCREASE 0
#define DECREASE 1

#define INTERFERENCE	0
#define BLOCKING		1

//GLPK

#define GLPK_MEM_USAGE_CHECK 0
#define GLPK_TERM_OUT 0
#define ILP_SOLUTION_VAR_CHECK 1
#define TIME_LIMIT 60000

//CFG

#define SORT_DEBUG 0
#define RTA_DEBUG 0



using namespace std;

typedef mpz_class int_t;		//multiple precision integer
typedef mpq_class fraction_t;		//multiple precision rational number
typedef mpf_class floating_t;		//multiple precision float number

class Task;
class TaskSet;
class Processor;
class ProcessorSet;
class Resource;
class ResourceSet;

typedef struct
{
	double min;
	double max;
}Range;

typedef struct
{
	double utilization;
	uint exp_num;
	uint success_num;
}Result;

typedef struct
{
	bool LP;
	uint test_type;
	uint test_method;
	string sched_method;
	string priority_assignment;
	string locking_protocol;
	string test_name;
	string remark;
	string rename;
	string style;
}Test_Attribute;

typedef vector<int> Int_Set;
typedef vector<double> Double_Set;
typedef vector<Range> Range_Set;
typedef vector<Result> Result_Set;
typedef vector<Result_Set> Result_Sets;
typedef vector<Test_Attribute> Test_Attribute_Set;
typedef list<void*> TaskQueue;
typedef list<void*> ResourceQueue;



#endif
