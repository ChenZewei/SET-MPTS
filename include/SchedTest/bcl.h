#ifndef BCL_H
#define BCL_H

#include <algorithm> // for min

#include "../tasks.h"
#include "../processors.h"

bool is_bcl_ftp_schedulable(TaskSet taskset, ProcessorSet processorset)
{
	int m = processorset.get_processor_num();
	for(int k = 0; k < taskset.get_taskset_size(); k++)
	{
		int sum = 0;
		int Ck = taskset.get_task_wcet(k);
		int Dk = taskset.get_task_deadline(k);
		int rhs = m *(Dk - Ck);
		for(int i = 0; i < taskset.get_taskset_size(); i++)
		{
			
			if(taskset.get_task_density(i) < taskset.get_task_density(k))
			{
				
				int Ci = taskset.get_task_wcet(i);
				int Di = taskset.get_task_deadline(i);
				int Ti = taskset.get_task_period(i);
				int m = processorset.get_processor_num();
				int temp = floor((Dk + Di - Ci)/Ti)*Ci + min(Ci,(Dk + Di - Ci) % Ti);
				sum += min((Dk - Ck), temp);
			}
		}
		
		if(sum >= rhs)
		{
			return false;
		}
	}
	return true;
}

bool is_bcl_edf_schedulable(TaskSet taskset, ProcessorSet processorset)
{
	int m = processorset.get_processor_num();
	for(int k = 0; k < taskset.get_taskset_size(); k++)
	{
		int sum = 0;
		int Ck = taskset.get_task_wcet(k);
		int Dk = taskset.get_task_deadline(k);
		int rhs = m *(Dk - Ck);
		for(int i = 0; i < taskset.get_taskset_size(); i++)
		{
			if(i != k)
			{
				
				int Ci = taskset.get_task_wcet(i);
				int Di = taskset.get_task_deadline(i);
				int Ti = taskset.get_task_period(i);
				int temp = floor(Dk/Ti)*Ci + min(Ci,Dk % Ti);
				sum += min((Dk - Ck), temp);
			}
		}
		if(sum >= rhs)
		{
			return false;
		}
	}
	return true;
}


#endif
