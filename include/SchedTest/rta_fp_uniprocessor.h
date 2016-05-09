#ifndef RTA_FP_UNIPROCESSOR_H
#define RTA_FP_UNIPROCESSOR_H

#include <math.h>
#include <math-helper.h>
#include <algorithm>
#include <iostream>

#include "../tasks.h"
#include "../processors.h"

using namespace std;

ulong interf_standard(const TaskSet tasks, uint t_id, ulong interval)
{
	return tasks[t_id].get_wcet() * int(ceiling((interval + tasks[t_id].get_jitter()) / tasks[t_id].get_period()));
}

// Audsly et al.'s standard RTA (1993)
ulong rta_standard(const TaskSet tasks, uint t_id, uint ITER_BLOCKING)
{
	ulong	test_end = tasks[t_id].get_deadline();
	ulong	test_start = tasks[t_id].get_total_blocking() + tasks[t_id].get_wcet();
	ulong	response = test_start;
	while (respone <= test_end)
	{
		switch (ITER_BLOCKING)
		{
			case 0:
				ulong demand = test_start;
				break;
			case 1:
				// add functions to bound "totoal blocking" here
				// XXXXXXXX				
				ulong demand = tasks[t_id].get_total_blocking() + tasks[t_id].get_wcet();
				break;
		}

		ulong interference = 0;
		for (uint th = 0; th < t_id; th ++)
		{
			if (tasks[th].get_partition() == tasks[t_id].get_partition())
			{
				interference += interf_standard(tasks, th, response);
			}
		}
		
		demand += interference;

		if (response == demond)
			return response + tasks[t_id].get_jitter();
		else 
			response = demond;
	}
	return test_end + 100;
}

bool is_pfp_rta_standard_schedulable(const TaskSet tasks, uint ITER_BLOCKING)
{
	for (uint t_id = 0; t_id < tasks.get_taskset_size(); t_id ++)
	{
		response_bound = rta_standard(tasks, t_id, ITER_BLOCKING);
		if (response_bound <= tasks[t_id].get_deadline())
			tasks[t_id].response_time = response_bound;
		else
			return false;
	}
	return true;
}
#endif
