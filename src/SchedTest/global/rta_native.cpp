#include "rta_native.h"

RTA_native::RTA_native(): GlobalSched(false, RTA, GLOBAL, FIX_PRIORITY, NONE, "", "native") {}

RTA_native::RTA_native(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources): GlobalSched(false, RTA, GLOBAL, FIX_PRIORITY, NONE, "", "native")
{
	this->tasks = tasks;
	this->processors = processors;
	this->resources = resources;
}

ulong RTA_native::workload(Task& task, ulong interval)
{
	return task.get_wcet() * ceiling(interval, task.get_period()) + task.get_wcet();
}

ulong RTA_native::response_time(Task& ti, TaskSet& tasks, ProcessorSet& processors)
{
	ulong wcet = ti.get_wcet();
	ulong test_end = ti.get_deadline();
	ulong test_start = ti.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	uint p_num = processors.get_processor_num();
	while(response < test_end)
	{
		ulong sum = 0;
		//for(uint x = 0; x < ti.get_id(); x++)
		foreach_higher_priority_task(tasks.get_tasks(), ti, th)
		{
			//Task& tx = tasks.get_task_by_id(x);
			sum += workload((*th), response);
		}
		demand = sum/p_num + wcet;
		if(response == demand)
			return 	response;
		else
			response = demand;
	}
	return test_end + 100;
}

bool RTA_native::is_schedulable()
{
	ulong response_bound;

	tasks.RM_Order();

	//for (uint i = 0; i < tasks.get_taskset_size(); i ++)
	foreach(tasks.get_tasks(), ti)
	{
		//Task& ti = tasks.get_task_by_id(i);
		ulong original_bound = ti->get_response_time();
		response_bound = response_time((*ti), tasks, processors);
		if (response_bound > ti->get_deadline())
			return false;
		if(response_bound > original_bound)
		{
			ti->set_response_time(response_bound);
		}
	}
	return true;
}

bool RTA_native::is_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong response_bound;

	foreach(tasks.get_tasks(), ti)
	{
		//Task& ti = tasks.get_task_by_id(i);
		ulong original_bound = ti->get_response_time();
		response_bound = response_time((*ti), tasks, processors);
		if (response_bound > ti->get_deadline())
			return false;
		if(response_bound > original_bound)
		{
			ti->set_response_time(response_bound);
		}
	}
	return true;
}

bool RTA_native::is_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint TEST_TYPE, uint ITER_BLOCKING)
{
	ulong response_bound;

	foreach(tasks.get_tasks(), ti)
	{
		//Task& ti = tasks.get_task_by_id(i);
		ulong original_bound = ti->get_response_time();
		response_bound = response_time((*ti), tasks, processors);
		if (response_bound > ti->get_deadline())
			return false;
		if(response_bound > original_bound)
		{
			ti->set_response_time(response_bound);
		}
	}
	return true;
}

