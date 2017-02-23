#ifndef RTA_PFP_GS_H
#define RTA_PFP_GS_H

#include "types.h"
#include "p_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

class RTA_PFP_GS: public PartitionedSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong pfp_gs_local_blocking(Task& ti);
		ulong pfp_gs_spin_time(Task& ti, uint resource_id);
		ulong pfp_gs_remote_blocking(Task& ti);
		ulong pfp_gs_NP_blocking(Task& ti);
		ulong response_time(Task& ti);
		bool alloc_schedulable();
		long pfp_gs_tryAssign(Task& ti, uint p_id);
	public:
		RTA_PFP_GS();
		RTA_PFP_GS(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		~RTA_PFP_GS();
		bool is_schedulable();
};

#endif
