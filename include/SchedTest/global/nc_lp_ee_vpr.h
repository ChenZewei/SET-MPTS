#ifndef NC_LP_EE_VPR_H
#define NC_LP_EE_VPR_H

/*
** 
** 
** 
*/

#include "g_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

class NC_LP_EE_VPR: public GlobalSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		set<ulong> get_time_point();
		ulong DBF_R(uint r_id, ulong interval);
		ulong blocking_time(uint r_id, ulong interval);
		ulong NCS_condition();
		bool condition_1(uint r_id);
		bool condition_2(uint r_id);
	public:
		NC_LP_EE_VPR();
		NC_LP_EE_VPR(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		bool is_schedulable();

};

#endif
