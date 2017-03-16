#ifndef LP_SCHED_H
#define LP_SCHED_H

#include "types.h"
#include "sched_test_base.h"

class Task;
class TaskSet;
class Request;
class ProcessorSet;
class ResourceSet;

class LPSched: public SchedTestBase
{
	public:
		LPSched(uint TestMethod, uint SchedMethod, uint PriorityAssignment, uint LockingProtocol = NONE, string name = "", string remark = "");
//		virtual void set_objective() = 0;
//		virtual void set_constraints() = 0;
};


#endif
