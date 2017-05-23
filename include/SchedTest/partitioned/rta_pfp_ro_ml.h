#ifndef RTA_PFP_RO_ML_H
#define RTA_PFP_RO_ML_H

#include "p_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

class RTA_PFP_RO_ML: public PartitionedSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong blocking_bound(Task& ti, uint r_id);
		ulong request_bound(Task& ti, uint r_id);
		ulong angent_exec_bound(Task& ti, uint p_id);
		ulong NCS_workload(Task& ti, ulong interval);//Non-Critical-Section
		ulong CS_workload(Task& ti, uint resource_id, ulong interval);//Critical-Section
		ulong response_time_AP(Task& ti);
		ulong response_time_SP(Task& ti);
		bool worst_fit_for_resources(uint active_processor_num);
		bool is_first_fit_for_tasks_schedulable(uint start_processor);
		bool alloc_schedulable();
	public:
		RTA_PFP_RO_ML();
		RTA_PFP_RO_ML(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		~RTA_PFP_RO_ML();
		bool is_schedulable();
};

#endif
