#ifndef RTA_PFP_WF_SPIN_H
#define RTA_PFP_WF_SPIN_H

/*
** RTA for partitioned fix priority scheduling using spinlock protocol with worst-fit allocation
** 
** Awieder and Brandenburg's RTA with spin locks (2013)
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

class RTA_PFP_WF_spinlock: public PartitionedSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong interference(Task& task, ulong interval);
		ulong response_time(Task& ti);
		bool alloc_schedulable();

	public:
		RTA_PFP_WF_spinlock();
		RTA_PFP_WF_spinlock(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources);
		bool is_schedulable();
	
};

#endif

