#include "p_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

PartitionedSched::PartitionedSched(bool LinearProgramming, uint TestMethod, uint PriorityAssignment, uint LockingProtocol, string name, string remark): SchedTestBase(LinearProgramming, TestMethod, PARTITIONED, PriorityAssignment, LockingProtocol, name, remark)
{
	
}

bool PartitionedSched::BinPacking_WF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE)
{
	processors.sort_by_task_utilization(INCREASE);

	Processor& processor = processors.get_processors()[0];
	if(processor.get_utilization() + ti.get_utilization() <= 1)
	{
		ti.set_partition(processor.get_processor_id());
		processor.add_task(&ti);
	}
	else
		return false;

	switch(MODE)
	{
		case PartitionedSched::UNTEST:
			break;
		case PartitionedSched::TEST:
			if(!alloc_schedulable())
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

bool PartitionedSched::BinPacking_BF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE)
{
	uint p_num = processors.get_processor_num();
	processors.sort_by_task_utilization(DECREASE);
	uint p_id;

	for(uint assign = 0; assign < p_num; assign++)
	{
		Processor& processor = processors.get_processors()[assign];
		if(processor.get_utilization() + ti.get_utilization() <= 1)
		{
			ti.set_partition(processor.get_processor_id());
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
		case PartitionedSched::UNTEST:
			break;
		case PartitionedSched::TEST:
			if(!alloc_schedulable())
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

bool PartitionedSched::BinPacking_FF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE)
{

}

bool PartitionedSched::BinPacking_NF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE)
{

}
