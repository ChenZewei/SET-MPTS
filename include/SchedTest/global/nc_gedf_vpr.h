#ifndef NC_GEDF_VPR_H
#define NC_GEDF_VPR_H

/*
** 
** 
** 
*/

#include "g_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

class NC_GEDF_VPR: public GlobalSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong DBF(uint r_id, ulong interval);
		ulong blocking_time(uint r_id, ulong interval);
		bool condition_1(uint r_id);
		bool condition_2(uint r_id);
	public:
		NC_GEDF_VPR();
		NC_GEDF_VPR(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		bool is_schedulable();

};

#endif
