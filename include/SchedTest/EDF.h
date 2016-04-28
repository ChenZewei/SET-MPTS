#include <iostream>
#include "../tasks.h"
#include "../processors.h"

using namespace std;

bool is_EDF_schedulable(TaskSet taskset)//Uniprocessor
{
	if(taskset.get_utilization_sum() <= 1)
		return true;
	else
		return false;
}

bool is_EDF_schedulable(TaskSet taskset, ProcessorSet processorset)//for identical multiprocessor
{
	if(1 == processorset.get_processor_num())//Uniprocessor
	{
		return is_EDF_schedulable(taskset);
	}
	else
	{
		if(taskset.is_implicit_deadline())
		{
			
		}
		else
		{

		}
        }
}
