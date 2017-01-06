#ifndef SCHED_TEST_BASE_H
#define SCHED_TEST_BASE_H

#include "types.h"

//Test Method
#define UTI_BOUND	0
#define RTA 		1

//Priority Assignment
#define RM	0
#define DM	1
#define EDF	2

//Scheduling Method
#define GLOBAL		0
#define	PARTITIONED	1

//Locking Protocols
#define SPIN	0
#define PIP		1
#define	DPCP	2
#define	MPCP	3
#define	FMLP	4

class Task;
class TaskSet;
class Request;
class Resource;
class ResourceSet;
class Processor;
class ProcessorSet;
class LinearExpression;
class LinearProgram;

class SchedTestBase
{
	private:
		bool LinearProgramming;
		uint TestMethod;
		uint SchedMethod;
		uint PriorityAssignment;
		uint LockingProtocol;
	public:
		SchedTestBase(bool LP, uint TM, uint SM, uint PA, uint LoP);
		string get_test_name();
		virtual bool is_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resourcese, uint TEST_TYPE, uint ITER_BLOCKING);
		
};

#endif
