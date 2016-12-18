#include <iostream>
//#include "../tasks.h"
//#include "../processors.h"
//#include "../resources.h"
#include "math-helper.h"
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
	ulong local_blocking = 0;
	Task& task_i = tasks.get_task_by_id(t_id);
	Resources& r = resources.get_resources();	
	Resource_Requests& rr = task_i.get_requests();
	uint p_id = task_i.get_partition();//processor id
	ulong r_i = task_i.get_response_time();//response time of task i(t_id)
	VarMapper var;
	LinearProgram local_bound;
	
	lp_dpcp_local_objective(task_i, tasks, resources, local_bound, var);

//construct constraints
	lp_dpcp_constraint_1(task_i, tasks, resources, local_bound, var);
	lp_dpcp_constraint_2(task_i, tasks, resources, local_bound, var);

	GLPKSolution *lb_solution = new GLPKSolution(local_bound, var.get_num_vars());

	if(lb_solution->is_solved())
	{
		local_blocking = lrint(lb_solution->evaluate(*(local_bound.get_objective())));
	}
	
	task_i.set_local_blocking(local_blocking);
	
	delete lb_solution;
	return local_blocking;
}

ulong remote_blocking(uint t_id, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong remote_blocking = 0;
	Task& task_i = tasks.get_task_by_id(t_id);
	Resources& r = resources.get_resources();	
	Resource_Requests& rr = task_i.get_requests();
	uint p_id = task_i.get_partition();//processor id
	ulong r_i = task_i.get_response_time();//response time of task i(t_id)
	VarMapper var;
	LinearProgram remote_bound;
	
	lp_dpcp_remote_objective(task_i, tasks, resources, remote_bound, var);

//construct constraints
	lp_dpcp_constraint_1(task_i, tasks, resources, remote_bound, var);
	lp_dpcp_constraint_2(task_i, tasks, resources, remote_bound, var);

	GLPKSolution *rb_solution = new GLPKSolution(remote_bound, var.get_num_vars());

	if(rb_solution->is_solved())
	{
		remote_blocking = lrint(rb_solution->evaluate(*(remote_bound.get_objective())));
	}

	task_i.set_remote_blocking(remote_blocking);
	
	delete rb_solution;
	return remote_blocking;
}

ulong total_blocking(uint t_id, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{	
	ulong total_blocking;
	Task& task_i = tasks.get_task_by_id(t_id);
	ulong blocking_l = local_blocking(t_id, tasks, processors, resources);
	ulong blocking_r = remote_blocking(t_id, tasks, processors, resources);

	total_blocking = blocking_l + blocking_r;
	task_i.set_total_blocking(total_blocking);
	return total_blocking;
}

ulong interference(Task& task, ulong interval)
{
	return task.get_wcet() * ceiling((interval + task.get_response_time()), task.get_period());
}

ulong rta_lp_pfp_suspension(uint t_id,
				TaskSet& tasks, 
				ProcessorSet& processors,
				ResourceSet& resources,
				uint ITER_BLOCKING)
{
	Task& task_i = tasks.get_task_by_id(t_id);
	ulong test_end = task_i.get_deadline();
	ulong test_start = task_i.get_total_blocking() + task.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	while (response <= test_end)
	{
		switch (ITER_BLOCKING)
		{
			case 0:
				demand = test_start;
				break;
			case 1:
				// add functions to bound "spin" and "local_blocking" here
				// XXXXXXXX			
				total_blocking(t_id, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
				demand = task_i.get_total_blocking() + task_i.get_wcet();
				break;
		}

		ulong interference = 0;
		for (uint th = 0; th < t_id; th ++)
		{
			Task& task_h = tasks.get_task_by_id(th);
			if (task_i.get_partition() == task_h.get_partition())
			{
				interference += interference(task_h, response);
			}
		}
//cout<<"interference1:"<<interference<<endl;
		demand += interference;

		if (response == demand)
			return response + task_i.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

bool is_rta_lp_pfp_schedulable(uint t_id,
				TaskSet& tasks,
				ProcessorSet& processors,
				ResourceSet& resources,
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
				response_bound = rta_lp_pfp_suspension(t_id, tasks, processors, resources, ITER_BLOCKING);
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
