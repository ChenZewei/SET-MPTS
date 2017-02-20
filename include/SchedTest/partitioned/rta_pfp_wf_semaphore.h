#ifndef RTA_PFP_WF_SEMAPHORE_H
#define RTA_PFP_WF_SEMAPHORE_H

/*
** RTA for partitioned fix priority scheduling using spinlock protocol with worst-fit allocation
** 
** RTA with self-suspension (to appear in RTS journal)
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

class RTA_PFP_WF_semaphore: public PartitionedSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong transitive_preemption(Task& ti, uint r_id);
		ulong DLB(Task& task_i);
		ulong DGB_l(Task& task_i);
		ulong DGB_h(Task& task_i);
		ulong MLI(Task& task_i);
		void calculate_total_blocking(Task& task_i);
		ulong interference(Task& task, ulong interval);
		ulong response_time(Task& ti);
		bool alloc_schedulable();

	public:
		RTA_PFP_WF_semaphore();
		RTA_PFP_WF_semaphore(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources);
		bool is_schedulable();
	
};

#endif

