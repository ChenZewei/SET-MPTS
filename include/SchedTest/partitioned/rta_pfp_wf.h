#ifndef RTA_PFP_WF_H
#define RTA_PFP_WF_H

/*
** RTA for partitioned scheduling with worst-fit allocation
** 
** Audsly et al.'s standard RTA (1993)
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

class RTA_PFP_WF: public PartitionedSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong interference(Task& task, ulong interval);
		ulong response_time(Task& ti);
		bool alloc_schedulable();

	public:
		RTA_PFP_WF();
		RTA_PFP_WF(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources);
		bool is_schedulable();
	
};

#endif


