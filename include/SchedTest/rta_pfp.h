#ifndef RTA_PFP_H
#define RTA_PFP_H

#include <math.h>
#include <math-helper.h>
#include <algorithm>
#include <iostream>

#include "../tasks.h"
#include "../processors.h"
#include "../resources.h"

using namespace std;

ulong interf_standard(const Task* task, ulong interval)
{
	return task->get_wcet() * ceiling((interval + task->get_jitter()), task->get_period());
}

// Audsly et al.'s standard RTA (1993)
ulong rta_standard(const TaskSet& tasks, uint t_id, uint ITER_BLOCKING)
{
	Task task = tasks.get_tasks()[t_id];
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_total_blocking() + task.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	while (response <= test_end)
	{
		switch (ITER_BLOCKING)
		{
			case 0:
				demand = test_start;
				break;
			case 1:
				// add functions to bound "totoal blocking" here
				// XXXXXXXX				
				demand = task.get_total_blocking() + task.get_wcet();
				break;
		}

		ulong interference = 0;
		for (uint th = 0; th < t_id; th ++)
		{
			const Task& tsk = tasks.get_tasks()[th];
			if (tsk.get_partition() == task.get_partition())
			{
				interference += interf_standard(&tsk, response);
			}
		}
		
		demand += interference;

		if (response == demand)
			return response + task.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

ulong interf_with_spin(const Task* task, ulong interval)
{
	return (task->get_wcet() + task->get_spin()) * ceiling((interval + task->get_jitter()), task->get_period());
}

// Awieder and Brandenburg's RTA with spin locks (2013)
ulong rta_with_spin(const TaskSet& tasks, uint t_id, uint ITER_BLOCKING)
{
	Task task = tasks.get_tasks()[t_id];
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_spin() + task.get_local_blocking() + task.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	while (response <= test_end)
	{
		switch (ITER_BLOCKING)
		{
			case 0:
				demand = test_start;
				break;
			case 1:
				// add functions to bound "spin" and "local_blocking" here
				// XXXXXXXX				
				demand = task.get_local_blocking() + task.get_spin() + task.get_wcet();
				break;
		}

		ulong interference = 0;
		for (uint th = 0; th < t_id; th ++)
		{
			const Task& tsk = tasks.get_tasks()[th];
			if (task.get_partition() == tsk.get_partition())
			{
				interference += interf_with_spin(&tsk, response);
			}
		}
		
		demand += interference;

		if (response == demand)
			return response + task.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

ulong interf_with_suspension(const Task* task, ulong interval)
{
	return task->get_wcet() * ceiling((interval + task->get_response_time()), task->get_period());
}

// RTA with self-suspension (to appear in RTS journal)
ulong rta_with_suspension(const TaskSet& tasks, uint t_id, uint ITER_BLOCKING)
{
	Task task = tasks.get_tasks()[t_id];
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_total_blocking() + task.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	while (response <= test_end)
	{
		switch (ITER_BLOCKING)
		{
			case 0:
				demand = test_start;
				break;
			case 1:
				// add functions to bound "spin" and "local_blocking" here
				// XXXXXXXX				
				demand = task.get_total_blocking() + task.get_wcet();
				break;
		}

		ulong interference = 0;
		for (uint th = 0; th < t_id; th ++)
		{
			const Task& tsk = tasks.get_tasks()[th];
			if (task.get_partition() == tsk.get_partition())
			{
				interference += interf_with_suspension(&tsk, response);
			}
		}
		
		demand += interference;

		if (response == demand)
			return response + task.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

ulong interf_with_distributed_scheduling(const Task* task, ulong interval)
{
	return task->get_wcet_non_critical_sections() * ceiling((interval + task->get_response_time()), task->get_period());
}

ulong rta_with_distributed_scheduling(const TaskSet& tasks, const ResourceSet& resources, uint t_id, uint ITER_BLOCKING)
{
	Task task = tasks.get_tasks()[t_id];
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_total_blocking() + task.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	while (response <= test_end)
	{
		switch (ITER_BLOCKING)
		{
			case 0:
				demand = test_start;
				break;
			case 1:
				// add functions to bound "spin" and "local_blocking" here
				// XXXXXXXX				
				demand = task.get_total_blocking() + task.get_wcet();
				break;
		}

		ulong interference = 0;
		for (uint th = 0; th < t_id; th ++)
		{
			const Task& tsk = tasks.get_tasks()[th];
			if (task.get_partition() == tsk.get_partition())
			{
				interference += interf_with_distributed_scheduling(&tsk, response);
			}
		}
		for (uint res = 0; res < resources.get_resourceset_size(); res ++)
		{
			const Resource& resource = resources.get_resources()[res];
			if (resource.get_locality() == task.get_partition())
			{
				// add functions to bound the interference from resource agents
				// XXXXXXXX
			}
		}
		
		demand += interference;

		if (response == demand)
			return response + task.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

bool is_pfp_rta_schedulable(const TaskSet& tasks, const ResourceSet& resources, uint TEST_TYPE, uint ITER_BLOCKING)
{
	ulong response_bound;
	for (uint t_id = 0; t_id < tasks.get_taskset_size(); t_id ++)
	{
		switch (TEST_TYPE)
		{
			case 0:
				response_bound = rta_standard(tasks, t_id, ITER_BLOCKING);
				break;
			case 1:
				response_bound = rta_with_spin(tasks, t_id, ITER_BLOCKING);
				break;
			case 2:
				response_bound = rta_with_suspension(tasks, t_id, ITER_BLOCKING);
				break;
			case 3:
				response_bound = rta_with_distributed_scheduling(tasks, resources, t_id, ITER_BLOCKING);
				break;

		}
		
		Task task = tasks.get_tasks()[t_id];
		if (response_bound <= task.get_deadline())
			task.set_response_time(response_bound);
		else
			return false;
	}
	return true;
}
#endif
