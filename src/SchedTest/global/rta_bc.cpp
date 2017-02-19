#include "rta_bc.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"


RTA_BC::RTA_BC(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources): GlobalSched(false, RTA, GLOBAL, FIX_PRIORITY, NONE, "", "BC")
{
	this->tasks = tasks;
	this->processors = processors;
	this->resources = resources;
}

ulong RTA_BC::workload(Task& task, ulong interval)
{
	ulong a = min(task.get_wcet(), (interval + task.get_response_time() - task.get_wcet()%task.get_period()));
	return task.get_wcet() * floor((interval + task.get_response_time() - task.get_wcet())/task.get_period()) + a;
}

ulong interference(Task& tk, Task& ti, ulong interval)
{
	return min(interval - tk.get_wcet() + 1, max((ulong)0, workload(ti, interval)));
}

ulong RTA_BC::response_time(Task& ti, TaskSet& tasks, ProcessorSet& processors)
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
		for(uint x = 0; i < ti.get_id(); x++)
		{
			Task& tx = tasks.get_task_by_id(x);
			sum += interference(ti, tx, response);
		}
		demand = floor(sum/p_num) + wcet;
		if(response == demand)
			return 	response;
		else
			response = demand;
	}
	return test_end + 100;
}

bool RTA_BC::is_schedulable()
{
	ulong response_bound;

	for (uint i = 0; i < tasks.get_taskset_size(); i ++)
	{
		Task& ti = tasks.get_task_by_id(i);
		ulong original_bound = ti.get_response_time();
		response_bound = response_time(ti, tasks, processors);
		if (response_bound > ti.get_deadline())
			return false;
		if(response_bound > original_bound)
		{
			ti.set_response_time(response_bound);
		}
	}
	return true;
}

bool RTA_BC::is_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong response_bound;

	for (uint i = 0; i < tasks.get_taskset_size(); i ++)
	{
		Task& ti = tasks.get_task_by_id(i);
		ulong original_bound = ti.get_response_time();
		response_bound = response_time(ti, tasks, processors);
		if (response_bound > ti.get_deadline())
			return false;
		if(response_bound > original_bound)
		{
			ti.set_response_time(response_bound);
		}
	}
	return true;
}

bool is_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint TEST_TYPE, uint ITER_BLOCKING)
{
	ulong response_bound;

	for (uint i = 0; i < tasks.get_taskset_size(); i ++)
	{
		Task& ti = tasks.get_task_by_id(i);
		ulong original_bound = ti.get_response_time();
		response_bound = response_time(ti, tasks, processors);
		if (response_bound > ti.get_deadline())
			return false;
		if(response_bound > original_bound)
		{
			ti.set_response_time(response_bound);
		}
	}
	return true;
}

RTA_BC* Factory_RTA_BC::creativeSchedTest() { return new RTA_BC; }
