#ifndef RO_PFP_H
#define RO_PFP_H

//Resource-Oriented Partitioned Scheduling

#include "type.h"

bool worst_fit_for_resources(ResourceSet& resources, ProcessorSet& processors, uint active_processor_num)
{
	resources.sort_by_utilization();

	foreach(resources.get_resources(), resource)
	{
		Processor& processor;
		fraction_t r_utilization = 1;
		uint assignment = 0;
		for(uint i = 0; i < active_processor_num; i++)
		{
			processor = processors.get_processors()[i];
			if(r_utilization > processor.get_resource_utilization())
			{
				r_utilization = procesor.get_resource_utilization();
				assignmeng = i;
			}
		}
		processor = processors.get_processors()[assignment];
		if(processor.add_resource(resource))
		{
			resource->set_locality(assignment);
		}
		else
			return false;
	}
	return true;
}

bool is_pfp_schedulable();

bool is_first_fit_for_tasks_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint start_processor, uint TEST_TYPE, uint ITER_BLOCKING)
{
	bool schedulable;
	uint p_num = processors.get_processor_num();
	tasks.RM_Order();

	foreach(tasks.get-tasks(), ti)
	{
		Processor& processor;
		uint assignment;
		schedulable = false;
		for(uint i = start_processor; i < start_processor + p_num; i++)
		{
			assignment = i%p_num;
			processor = processors.get_processors()[assignment];
			if(processor.add_task(ti))
			{
				ti->set_partition(assignment);
				if(is_pfp_schedulable())
				{
					schedulable = true;
					break;
				}
				else
				{
					ti->init();
					processor.remove_task(ti);
				}
			}
		}

		if(!schedulable)
			return schedulable;
	}
	
	return schedulable;
}

bool is_ro_pfp_schedulable(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint TEST_TYPE, uint ITER_BLOCKING)
{
	bool schedulable;
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	
	for(uint i = 1; i <= p_num; i++)
	{
		//initialzation
		tasks.init();
		processor.init();
		resources.init();
		
		if(!worst_fit_for_resources(resources, procesors, i))
			continue;

		schedulable = is_first_fit_for_tasks_schedulable(tasks, processors, resources, i%p_num, TEST_TYPE, ITER_BLOCKING);
		if(schedulable)
			return schedulable;
	}
	return schedulable;
}




#endif
