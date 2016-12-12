#include <iostream>
#include <glpk.h>
#include "../tasks.h"
#include "../processors.h"
#include "../resources.h"

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
	Task& task = tasks.get_task_by_id(t_id);
	Resources& r = resources.get_resources();	
	Resource_Requests& rr = task.get_requests();
	uint p_id = task.get_partition();//processor id
	ulong r_i = task.get_response_time();//response time of task i(t_id)
	glp_prob *lp = glp_create_prob();
	glp_set_prob_name(lp, "local_blocking");
	glp_set_obj_dir(lp, GLP_MAX);
	vector<int> ia, ja, ra;
	vector<int> lri;//local resource id
	ulong contant_blocking = 0;
	vector<int> start_index;
	Request& request;
	start_index.push_back(1);

	for(uint q = 0; q < r.size(); q++)
	{
		Resource& resource = r[q];
		if(resource.get_locality() == p_id)
		{
			lri.push_back(q);
			for(uint i = 0; i < rr.size(); rr++)
			{
				request = rr[i];
				if(q == request.get_resource_id())
					constant_blocking += request.get_num_requests()*request.get_max_length();
			}
		}
	}
	
	for(uint x = 0; x < tasks.get_taskset_size(); x++)//get tasks except task i
	{
		if(t_id == x)
		{
			int temp = start_index(start_index.size() - 1);
			start_index.push_back(temp);
			continue;
		}
		Task& task_x = tasks.get_task_by_id(x);
		Resource_Requests& rr_x = task_x.get_requests();
		for(uint q = 0; q < lri.size(); q++)
		{
			uint resource_id = lri[q];
			for(uint i = 0; i < rr_x.size(); i++)
			{
				request = rr_x[i];
				if(resource_id == request.get_resource_id())
				{
					
				}
			}
		}
			
	}
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
