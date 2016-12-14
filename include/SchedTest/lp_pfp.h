#include <iostream>
//#include "../tasks.h"
//#include "../processors.h"
//#include "../resources.h"
#include "lp_dpcp.h"

using namespace std;

typedef struct
{
	fraction_t x_d;
	fraction_t x_i;
	fraction_t x_p;
}Structure;

string get_structure_name(uint t_id, uint r_id, uint seq, int BLOCKING_TYPE)
{
	std::ostringstream buf;
	switch(BLOCKING_TYPE)
	{
		case DIRECT_REQUEST_DELAY:
			buf<<"Xd[";
			break;
		case INDIRECT_REQUEST_DELAY:
			buf<<"Xi[";
			break;
		case PREEMPTION_DELAY:
			buf<<"Xp[";
			break;
	}
	buf<<t_id<<","<<r_id<<","<<seq<<"]";
	return buf.str();
}

ulong local_blocking(uint t_id, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	Task& task_i = tasks.get_task_by_id(t_id);
	Resources& r = resources.get_resources();	
	Resource_Requests& rr = task_i.get_requests();
	uint p_id = task_i.get_partition();//processor id
	ulong r_i = task_i.get_response_time();//response time of task i(t_id)

	LinearProgram local_bound;
	

	lp_dpcp_local_objective(task_i, tasks, resources, local_bound);

//construct constraints
	lp_dpcp_constraint_1(task_i, tasks, resources, local_bound);
	lp_dpcp_constraint_2(task_i, tasks, resources, local_bound);

	GLPKSolution lb_solution = new GLPKSolution(local_bound, );
	
}

ulong remote_blocking(uint t_id, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	
}

ulong total_blocking(uint t_id, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong blocking_l = local_blocking(t_id, tasks, processors, resources);
	ulong blocking_r = remote_blocking(t_id, tasks, processors, resources);
	return blocking_l + blocking_r;
}

ulong interference(uint t_id, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	
}

bool is_pfp_schedulable(TaskSet& tasks, 
				ProcessorSet& processors,
				ResourceSet& resources,
				uint TEST_TYPE,
				uint ITER_BLOCKING)
{
	ulong response_bound;
	for(uint t_id = 0; t_id < tasks.get_taskset_size(); t_id++)
	{
		Task& task = tasks.get_task_by_id(t_id);
		if(task.get_partition() == MAX_LONG)
			continue;

		switch(TEST_TYPE)
		{
			case 0://DPCP
				
				break;
			case 1://MPCP
				
				break;
			default:
				
				break;
		}

		if(response_bound <= task.get_deadline())
			task.set_response_time(response_bound);
		else
			return false;
	}
	return true;
}
