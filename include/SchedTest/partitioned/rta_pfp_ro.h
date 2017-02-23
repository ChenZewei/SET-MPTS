#ifndef RTA_PFP_RO_H
#define RTA_PFP_RO_H

#include "p_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

class RTA_PFP_RO: public PartitionedSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong ro_workload(Task& ti, ulong interval);
		ulong ro_agent_workload(Task& ti, uint resource_id, ulong interval);
		ulong ro_get_bloking(Task& ti);
		ulong ro_get_interference_R(Task& ti, ulong interval);
		ulong ro_get_interference_AC(Task& ti, ulong interval);
		ulong ro_get_interference_UC(Task& ti, ulong interval);
		ulong ro_get_interference(Task& ti, ulong interval);
		ulong response_time(Task& ti);
		bool worst_fit_for_resources(uint active_processor_num);
		bool is_first_fit_for_tasks_schedulable(uint start_processor);
		bool alloc_schedulable();
	public:
		RTA_PFP_RO();
		RTA_PFP_RO(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		~RTA_PFP_RO();
		bool is_schedulable();
};

#endif
