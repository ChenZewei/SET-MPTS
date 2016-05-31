#include <iostream>
#include "../tasks.h"
#include "../processors.h"
#include "../resources.h"
#include "bin_packing.h"

using namespace std;

bool is_worst_fit_u_schedulable(const TaskSet& tasks, 
				const ProcessorSet& processors,
				const ResourceSet& resources,
				uint cpu_num,
				uint TEST_TYPE,
				uint ITER_BLOCKING)
{
	for (uint i = 0; i < cpu_num; i ++)
	{
		Processor processor = processors.get_processors()[i];
		processor.clear();
	}
	for (uint t_id = 0; t_id < tasks.get_taskset_size(); t_id ++)
	{
		if (!worst_fit_by_utilization(tasks, processors, resources, t_id, TEST_TYPE, ITER_BLOCKING))
			return false;
	}
	return true;
}
