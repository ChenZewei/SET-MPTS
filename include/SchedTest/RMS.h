#ifndef RMS_H
#define RMS_H

#include <math.h>
#include <algorithm>
#include <iostream>

#include "../processors.h"
#include "../tasks.h"

using namespace std;

fraction_t get_utilization_bound(TaskSet taskset)
{	
	fraction_t temp = 1, t_num = taskset.get_taskset_size(), bound;
	temp /= t_num;
	bound = t_num;
	bound *= pow(2,temp.get_d())-1;
	return bound;
}

bool is_RM_schedulable(TaskSet taskset)//Uniprocessor
{
	fraction_t temp = 1, t_num = taskset.get_taskset_size(), bound;

	if(0 == t_num)
		return true;
	temp /= t_num;

	bound = t_num;

	bound *= pow(2,temp.get_d())-1;

	if(taskset.get_utilization_sum() <= bound)
		return true;
	else
		return false;
}

bool is_RM_schedulable(TaskSet taskset, ProcessorSet processorset)//for identical multiprocessor platform
{
	uint processor_num = processorset.get_processor_num();
	if(1 == processor_num)//Uniprocessor
	{
		return is_RM_schedulable(taskset);
	}
	else
	{
		if(taskset.is_implicit_deadline())
		{
			fraction_t u_sum = taskset.get_utilization_sum();
			fraction_t u_max = taskset.get_utilization_max();
			if(u_sum <= processor_num*(1-u_max)/2+u_max)
				return true;
			else
				return false;
		}
		else if(taskset.is_constrained_deadline())
		{
			
		}
		else
		{

		}
        }
}

bool is_RM_schedulable(TaskSet taskset, ProcessorSet processorset, Task task)
{
	uint processor_num = processorset.get_processor_num();
	if(1 == processor_num)
	{
		fraction_t temp = 1, t_num = taskset.get_taskset_size(), bound;
		temp /= t_num;
		bound = t_num;
		bound *= pow(2,temp.get_d())-1;
		if(taskset.get_utilization_sum() + task.get_utilization() <= bound)
			return true;
		else
			return false;
	}
}

#endif
