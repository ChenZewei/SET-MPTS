#include "pfp_gs.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"
#include "assert.h"

ulong pfp_gs_local_blocking(Task& ti, Taskset& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong blocling = 0;
	uint p_id = ti.get_partition();
	
	foreach(Resources.get_resources(), resource)
	{
		uint q = resource->get_resource_id();
		foreach(tasks.get_tasks(), tj)
		{
			if((tj->is_request_exist(q)) && (resource->get_ceiling() < ti.get_id()) && (ti.get_id() < tj->get_id()) && (p_id == resource->get_locality()))
			{
				ulong length = tj->get_request_by_id(q).get_max_length();
				if(blocking < length)
					blocking = length;
			}
		}
	}
	return blocking;
}

ulong pfp_gs_spin_time(Task& ti, uint resource_id, Taskset& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	uint p_id = ti.get_partition();
	ulong sum = 0;
	for(uint i = 0; i < processors.get_processor_num(); i++)
	{
		if(p_id == i)
			continue;
		Processor& processor = processors.get_processors()[i];
		ulong max_spin_time = 0;
		foreach(tasks.get_tasks(), tx)
		{
			if((i = tx->get_partition()) && (tx->is_request_exist(resource_id)))
			{
				ulong length = tx->get_request_by_id(resource_id).get_max_length();
				if(max_spin_time < length)
					max_spin_time = length;
			}
		}
		sum += max_spin_time;
	}
	return sum;
}

ulong pfp_gs_remote_blocking(Task& ti, Taskset& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong blocking = 0;

	foreach(ti.get_requests(), request)
	{
		uint q = request->get_resource_id();
		blocking += request->get_num_requests()*pfp_gs_spin_time(ti, q, tasks, processors, resources);
	}
	return blocking;
}

ulong pfp_gs_NP_blocking(Task& ti, Taskset& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong blocking = 0;

	foreach(resources.get_resources(), resource)
	{
		if(resource->is_global_resoruce())
		{
			uint q = resource->get_resource_id();
			foreach(tasks.get_tasks(), tx)
			{
				if((tx->is_request_exist(q)) && (ti.get_partition() == tx->get_partition()) && (ti.get_id() < tx->get_id()))
				{
					ulong length = get_spin_time(ti, q, tasks, processors, resources) + tx->get_request_by_id(q).get_max_length();
					if(blocking < length)
						blocking = length;
				}
			}
		}
	}
	return blocking;
}

ulong pfp_gs_response_time(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong wcet = ti.get_wcet();
	ulong local_blocking = pfp_gs_local_blocking(ti, tasks, processors, resources);
	ulong remote_blocking = pfp_gs_remote_blocking(ti, tasks, processors, resources);
	ulong NP_blocking = pfp_gs_NP_blocking(ti, tasks, processors, resources);
	ulong response_time = ti.get_response_time();
	ulong test_start = wcet + remote_blocking + max(local_blocking, NP_blocking);
	ulong test_end = ti.get_deadline();

	ti.set_local_blocking(local_blocking);
	ti.set_remote_blockimg(remote_blocking);
	ti.set_total_blocking(local_blocking + remote_blocking);

	while(response_time < test_end)
	{
		test_start = wcet + remote_blocking + max(local_blocking, NP_blocking);
		foreach_higher_priority_task(tasks.get_tasks(), ti, th)
		{
			if(ti.get_partition() == th->get_partition())
			{
				ulong wcet_h = th->get_wcet();
				ulong period_h = th->get_period();
				ulong remote_blocking_h = th->get_remote_blocking();
				test_start += ceiling(response_time, period_h)*(wcet_h + remote_blocking_h);
			}
		}

		assert(test_start >= response_time);
		
		if(test_start != response_time)
			response_time = test_start;
		else
		{
			ti.set_response_time(response_time);
			return response_time;
		}
		
	}

	return test_end + 100;
}

bool is_pfp_gs_tryAssign_schedulable(Taskset& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong response_bound;
	for(uint t_id = 0; t_id < tasks.get_taskset_size(); t_id++)
	{
		Task& task = tasks.get_task_by_id(t_id);

		if(task.get_partition() == MAX_INT)
			continue;
		response_bound = pfp_gs_response_time(task, tasks, processors, resources);
		if(response_bound > task.get_deadline())	
			return false;
	}
	return true;
}

ulong pfp_gs_tryAssign(Task& ti, uint p_id, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong s = MAX_LONG;
	Porcessor& processor = processors.get_processors()[p_id];

	//temporarily assign ti to processor[p_id]
	processor.add_task(&ti);
	ti.set_partition(p_id);

	TaskQueue& taskqueue = processor.get_taskqueue();//Task set U(all task assigned to processor[p_id])
	TaskQueue U = taskqueue;
	uint queue_size = U.size();

	for(uint pi = queue_size - 1; pi >= 0; pi--)
	{
		TaskQueue C = U;
		foreach(C, task)
		{
			Task *tx = ((Task*)(*task));
				
			if(tx->get_partition() != p_id)
				continue;

			if(MAX_INT == tx->get_priority())
			{
				tx->set_priority(pi);
				if(!is_pfp_gs_tryAssign_schedulable(tasks, processors, resources))
				{
					C.remove(task);
				}
				
				tx->set_priority(MAX_INT);
			}
		}
		if(0 == C.size())
			return -1;
		else
		{
			ulong max_period = 0;
			TaskQueue::iterator it;
			foreach(C, task)
			{
				if(max_period < ((Task*)(*task))->get_period())
				{
					it = task;
					max_period = ((Task*)(*task))->get_period();
				}
			}
			((Task*)(*it))->set_priority(pi);
			foreach(U, ti)
			{
				if(ti == it)
					U.remove(it);
			}
		}
	}

	foreach(U, ti)
	{
		ulong period_i = ((Task*)(*ti))->get_period();
		ulong response_time_i = ((Task*)(*ti))->get_response_time();

		if(s > (period_i - response_time_i))
		{
			s = period_i - response_time_i;	
		}
	}

	return s;
}

typedef struct
{
	ulong p_id,
	ulong s
}gs_tryAssign;

bool is_pfp_gs_schedulable(Taskset& tasks, ProcessorSet& processors, ResourceSet& resources)//Greedy Slacker heuristic partitioning
{
	tasks.DM_Order();
	uint p_num = processors.get_processor_num();
	
	foreach(tasks.get_tasks(), tx)
	{
		vector<gs_tryAssign> C;

		for(uint p_id = 0; p_id < p_num; p_id++)
		{
			ulong s = ulong pfp_gs_tryAssign((*tx), p_id, tasks, processors, resources);
			if(0 <= s)
			{
				C.push_back(gs_tryAssign(p_id, s));
			}
		}

		if(0 >= C.size())
			return false;
		else
		{
			ulong max_s = 0;
			uint assignment;
			foreach(C, c)
			{
				if(max_s < c.s)
				{
					max_s = c.s;
					assignment = c.p_id;
				}
			}
			tx->set_partition(assignment);
		}
	}
	return true;
}

















