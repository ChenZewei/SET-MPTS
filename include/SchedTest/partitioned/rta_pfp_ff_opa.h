#ifndef RTA_PFP_FF_OPA_H
#define RTA_PFP_FF_OPA_H

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

class RTA_PFP_FF_OPA: public PartitionedSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong interference(Task& task, ulong interval);
		ulong response_time(Task& ti);
		bool alloc_schedulable();
		bool BinPacking_FF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE);
		bool OPA(Task& ti);

	public:
		RTA_PFP_FF_OPA();
		RTA_PFP_FF_OPA(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		bool is_schedulable();
	
};

#endif


