#ifndef RTA_PFP_FF_H
#define RTA_PFP_FF_H

/*
**
** 
**
*/

#include "p_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

/*
class Task;
class TaskSet;
class Request;
class ProcessorSet;
class ResourceSet;
*/

class RTA_PFP_FF: public PartitionedSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong interference(Task& task, ulong interval);
		ulong response_time(Task& ti);
		bool alloc_schedulable();

	public:
		RTA_PFP_FF();
		RTA_PFP_FF(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		bool is_schedulable();
	
};

#endif


