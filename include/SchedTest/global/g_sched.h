#ifndef G_SCHED_H
#define G_SCHED_H

#include "types.h"
#include "sched_test_base.h"

class Task;
class TaskSet;
class Request;
class ProcessorSet;
class ResourceSet;

class GlobalSched: public SchedTestBase
{
	public:
/*
		enum
		{
			UNTEST,
			TEST
		};
*/
		GlobalSched(bool LinearProgramming, uint TestMethod, uint PriorityAssignment, uint LockingProtocol = NONE, string name = "", string remark = "");

		
};

#endif
