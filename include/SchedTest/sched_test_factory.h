#ifndef SCHED_TEST_FACTORY_H
#define SCHED_TEST_FACTORY_H

#include "types.h"
//#include "sched_test_base.h"

class TaskSet;
class ProcessorSet;
class ResourceSet;
class SchedTestBase;

class SchedTestFactory
{
	public:
		SchedTestBase* createSchedTest(string test_name, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources);
};

#endif
