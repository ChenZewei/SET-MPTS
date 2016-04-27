#include <iostream>
#include "../../include/SchedTest/RMS.h"
using namespace std;

RMS::RMS(uint p_num)
{
	processor_num = p_num;
}

bool RMS::is_uniprocessor()
{
	if(1 == processor_num)
		return true;
	else
		return false;
}


fraction_t RMS::get_utilization_bound(TaskSet taskset)
{	
	fraction_t temp = 1, t_num = taskset.get_taskset_size(), bound;
	temp /= t_num;
	bound = t_num;
	bound *= pow(2,temp.get_d())-1;
	return bound;
}

bool RMS::is_RM_schedulable(TaskSet taskset)//for identical multiprocessor platform
{
	if(is_uniprocessor())
	{
		fraction_t temp = 1, t_num = taskset.get_taskset_size(), bound;
		temp /= t_num;
		bound = t_num;
		bound *= pow(2,temp.get_d())-1;
		if(taskset.get_utilization_sum() <= bound)
			return true;
		else
			return false;
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
		else
		{

		}
        }
}

bool RMS::is_RM_schedulable(TaskSet taskset, Task task)
{
	if(is_uniprocessor())
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
