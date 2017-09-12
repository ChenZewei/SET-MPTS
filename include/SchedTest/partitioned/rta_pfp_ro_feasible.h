#ifndef RTA_PFP_RO_FEASIBLE_H
#define RTA_PFP_RO_FEASIBLE_H

#include "p_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

class RTA_PFP_RO_FEASIBLE: public PartitionedSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		bool condition_1();
		bool condition_2();
		bool condition_3();
		ulong DBF_R(Task& ti, uint r_id, ulong interval);
		bool alloc_schedulable();
	public:
		RTA_PFP_RO_FEASIBLE();
		RTA_PFP_RO_FEASIBLE(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		~RTA_PFP_RO_FEASIBLE();
		bool is_schedulable();
};

#endif
