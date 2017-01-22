#include "p_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

void PartitionedSched::BinPacking_WF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE)
{
	processors.sort_by_task_utilization(INCREASE);

	Processor& processor = processors.get_processors()[0];
	if(processor.get_utilization() + ti.get_utilization() <= 1)
	{
		ti.set_partition(0);
		processor.add_task(&ti);
	}
	else
		return false;

	switch(MODE)
	{
		case UNTEST:
			break;
		case TEST:
			if(!is_schedulable(tasks, processors, resources))
			{
				ti.set_partition(MAX_INT);
				processor.remove_task(&ti);
				return false;
			}
			break;
		default:
			break;
	}

	return true;
}

void PartitionedSched::BinPacking_BF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE)
{
	uint p_num = processors.get_processor_num();
	processors.sort_by_task_utilization(DECREASE);
	uint p_id;

	for(uint assign = 0; assign < p_num; assign++)
	{
		Processor& processor = processors.get_processors()[assign];
		if(processor.get_utilization() + ti.get_utilization() <= 1)
		{
			ti.set_partition(assign);
			processor.add_task(&ti);
			p_id = assign;
			break;
		}
		else
			if((p_num - 1) == assign)
				return false;
	}

	Processor& processor = processors.get_processors()[p_id];
	switch(MODE)
	{
		case UNTEST:
			break;
		case TEST:
			if(!is_schedulable(tasks, processors, resources))
			{
				ti.set_partition(MAX_INT);
				processor.remove_task(&ti);
				return false;
			}
			break;
		default:
			break;
	}

	return true;
}

void PartitionedSched::BinPacking_FF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE)
{

}

void PartitionedSched::BinPacking_NF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE)
{

}

