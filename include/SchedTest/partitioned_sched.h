#include <iostream>
#include "../tasks.h"
#include "../processors.h"
#include "RMS.h"
#include "EDF.h"

using namespace std;

bool is_Partitioned_FTP_Schedulable(TaskSet taskset, ProcessorSet processorset)
{
	if(1 == processorset.get_processor_num())//Uniprocessor
	{
		return is_RM_schedulable(taskset);
	}
	else//Multiprocessor
	{
		
	}
}

bool is_Partitioned_EDF_Schedulable(TaskSet taskset, ProcessorSet processorset)//for both implicit and constraint deadline task mode
{
	uint processor_num = processorset.get_processor_num();
	if(taskset.get_density_sum() <= processor_num - (processor_num - 1)*taskset.get_density_max())
		return true;
	else
		return false;
}
