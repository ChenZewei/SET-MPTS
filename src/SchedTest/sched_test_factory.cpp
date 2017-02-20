#include "sched_test_factory.h"
#include "rta_native.h"
#include "rta_bc.h"

SchedTestBase* SchedTestFactory::createSchedTest(string test_name, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	if(strcmp(test_name.data(), "RTA-native"))
	{
		return new RTA_native(tasks, processors, resources);
	}
	else if(strcmp(test_name.data(), "RTA-BC"))
	{
		return new RTA_BC(tasks, processors, resources);
	}
	else
		return NULL;
}
