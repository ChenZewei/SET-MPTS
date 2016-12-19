#include <iostream>
#include "../tasks.h"
#include "../processors.h"
#include "../resources.h"
#include "bin_packing.h"
#include "lp_pfp.h"

using namespace std;

bool is_worst_fit_dm_schedulable(TaskSet& tasks, 
				ProcessorSet& processors,
				ResourceSet& resources,
				uint cpu_num,
				uint TEST_TYPE,
				uint ITER_BLOCKING)
{
	processors.init();
	//tasks.sort_by_utilization();
	for (uint t_id = 0; t_id < tasks.get_taskset_size(); t_id ++)
	{
		if (!worst_fit_dm(tasks, processors, resources, t_id, TEST_TYPE, ITER_BLOCKING))
			return false;
	}
	return true;
}

bool is_worst_fit_pfp_schedulable(TaskSet& tasks, 
				ProcessorSet& processors,
				ResourceSet& resources,
				uint TEST_TYPE,//TEST_TYPE: 0-DPCP 1-MPCP
				uint ITER_BLOCKING)
{
	processors.init();
	for(uint t_id = 0; t_id < tasks.get_taskset_size(); t_id++)
	{
		if(!worst_fit_pfp(tasks, processors, resources, t_id, TEST_TYPE, ITER_BLOCKING))
			return false;
	}
	for(uint t_id = 0; t_id < tasks.get_taskset_size(); t_id++)
	{
		if(!is_rta_lp_pfp_schedulable(t_id, tasks, processors, resources, TEST_TYPE, ITER_BLOCKING))
			return false;
	}	
	return true;
}

bool is_first_fit_dm_schedulable(TaskSet& tasks, 
				ProcessorSet& processors,
				ResourceSet& resources,
				uint cpu_num,
				uint TEST_TYPE,
				uint ITER_BLOCKING)
{
	processors.init();
	//tasks.sort_by_utilization();
	for (uint t_id = 0; t_id < tasks.get_taskset_size(); t_id ++)
	{
		if (!first_fit_dm(tasks, processors, resources, t_id, TEST_TYPE, ITER_BLOCKING))
			return false;
	}
	return true;
}

bool is_worst_fit_edf_schedulable(TaskSet& tasks, 
				ProcessorSet& processors,
				ResourceSet& resources,
				uint cpu_num,
				uint TEST_TYPE,
				uint ITER_BLOCKING)
{
	processors.init();
	//tasks.sort_by_utilization();
	for (uint t_id = 0; t_id < tasks.get_taskset_size(); t_id ++)
	{
		if (!worst_fit_edf(tasks, processors, resources, t_id, TEST_TYPE, ITER_BLOCKING))
			return false;
	}
	return true;
}
