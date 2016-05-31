#include <iostream>
#include "../tasks.h"
#include "../processors.h"
#include "../resources.h"
#include "bin_packing.h"

using namespace std;

bool is_worst_fit_dm_schedulable(TaskSet& tasks, 
				ProcessorSet& processors,
				const ResourceSet& resources,
				uint cpu_num,
				uint TEST_TYPE,
				uint ITER_BLOCKING)
{
	for (uint i = 0; i < cpu_num; i ++)
	{
		processors.get_processors()[i].clear();
	}
	//tasks.sort_by_utilization();
	for (uint t_id = 0; t_id < tasks.get_taskset_size(); t_id ++)
	{
		if (!worst_fit_dm(tasks, processors, resources, t_id, TEST_TYPE, ITER_BLOCKING))
			return false;
	}
	return true;
}

bool is_worst_fit_edf_schedulable(TaskSet& tasks, 
				ProcessorSet& processors,
				const ResourceSet& resources,
				uint cpu_num,
				uint TEST_TYPE,
				uint ITER_BLOCKING)
{
	for (uint i = 0; i < cpu_num; i ++)
	{
		processors.get_processors()[i].clear();
	}
	//tasks.sort_by_utilization();
	for (uint t_id = 0; t_id < tasks.get_taskset_size(); t_id ++)
	{
		if (!worst_fit_edf(tasks, processors, resources, t_id, TEST_TYPE, ITER_BLOCKING))
			return false;
	}
	return true;
}
