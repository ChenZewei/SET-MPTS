#include "rta_pfp_gs.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"
#include "assert.h"
#include "iteration-helper.h"
#include "math-helper.h"

RTA_PFP_GS::RTA_PFP_GS(): PartitionedSched(false, RTA, FIX_PRIORITY, SPIN, "", "spinlock") {}

RTA_PFP_GS::RTA_PFP_GS(TaskSet tasks, ProcessorSet processors, ResourceSet resources): PartitionedSched(false, RTA, FIX_PRIORITY, SPIN, "", "spinlock")
{
	this->tasks = tasks;
	this->processors = processors;
	this->resources = resources;
	
	this->tasks.DM_Order();
	this->processors.init();
}

RTA_PFP_GS::~RTA_PFP_GS() {}

ulong RTA_PFP_GS::pfp_gs_local_blocking(Task& ti)
{
	ulong blocking = 0;
	uint p_id = ti.get_partition();

	foreach_lower_priority_task(tasks.get_tasks(), ti, tj)
	{
		foreach(tj->get_requests(), request)
		{
			uint q = request->get_resource_id();
			Resource& resource_q = resources.get_resources()[q];
			if(resource_q.is_global_resource())
				continue;
			
			if(resource_q.get_ceiling() <= ti.get_priority())
			{
				ulong length = request->get_max_length();
				if(blocking < length)
					blocking = length;
			}
		}
	}

	return blocking;
}

ulong RTA_PFP_GS::pfp_gs_spin_time(Task& ti, uint resource_id)
{
	uint p_num = processors.get_processor_num();
	uint p_id = ti.get_partition();
	ulong sum = 0;
	for(uint i = 0; i < p_num; i++)
	{
		if(p_id == i)
			continue;
		Processor& processor = processors.get_processors()[i];
		ulong max_spin_time = 0;
		foreach(tasks.get_tasks(), tx)
		{
			if((i == tx->get_partition()) && (tx->is_request_exist(resource_id)))
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

ulong RTA_PFP_GS::pfp_gs_remote_blocking(Task& ti)
{
	ulong blocking = 0;

	foreach(ti.get_requests(), request)
	{
		uint q = request->get_resource_id();
		blocking += request->get_num_requests() * pfp_gs_spin_time(ti, q);
	}
	return blocking;
}

ulong RTA_PFP_GS::pfp_gs_NP_blocking(Task& ti)
{
	ulong blocking = 0;

	foreach_lower_priority_task(tasks.get_tasks(), ti, tj)
	{
		if(ti.get_partition() != tj->get_partition())
			continue;

		foreach(tj->get_requests(), request)
		{
			uint q = request->get_resource_id();
			Resource& resource_q = resources.get_resources()[q];
			if(!resource_q.is_global_resource())
				continue;

			ulong length = pfp_gs_spin_time((*tj), q) + request->get_max_length();
			if(blocking < length)
				blocking = length;
		}
	}

/*
	foreach(resources.get_resources(), resource)
	{
		if(resource->is_global_resource())
		{
			uint q = resource->get_resource_id();
			foreach(tasks.get_tasks(), tx)
			{
				if((tx->is_request_exist(q)) && (ti.get_partition() == tx->get_partition()) && (ti.get_id() < tx->get_id()))
				{
					ulong length = pfp_gs_spin_time((*tx), q) + tx->get_request_by_id(q).get_max_length();
					if(blocking < length)
						blocking = length;
				}
			}
		}
	}
*/
	return blocking;
}

ulong RTA_PFP_GS::response_time(Task& ti)
{
	ulong wcet = ti.get_wcet();
	ulong jitter = ti.get_jitter();
	//ti.set_response_time(wcet);
	ulong local_blocking = pfp_gs_local_blocking(ti);
	ulong remote_blocking = pfp_gs_remote_blocking(ti);
	ulong NP_blocking = pfp_gs_NP_blocking(ti);
	ulong test_start = wcet + remote_blocking + max(local_blocking, NP_blocking);
	ulong test_end = ti.get_deadline();
	ulong response_time = test_start;

	ti.set_local_blocking(local_blocking);
	ti.set_remote_blocking(remote_blocking);
	ti.set_total_blocking(local_blocking + remote_blocking);
/*
	foreach(tasks.get_tasks(), tx)
	{
		cout<<"Task"<<tx->get_id()<<" partition:"<<tx->get_id()<<" priority:"<<tx->get_priority()<<endl;
	}
*/
	while(response_time + jitter <= test_end)
	{
		test_start = wcet + remote_blocking + max(local_blocking, NP_blocking);

		foreach_higher_priority_task(tasks.get_tasks(), ti, th)
		{
			if(ti.get_partition() == th->get_partition())
			{
				ulong wcet_h = th->get_wcet();
				ulong period_h = th->get_period();
				ulong jitter_h = th->get_jitter();
				ulong remote_blocking_h = th->get_remote_blocking();
				test_start += ceiling(response_time + jitter_h, period_h)*(wcet_h + remote_blocking_h);
			}
		}

		assert(test_start >= response_time);
		
		if(test_start != response_time)
			response_time = test_start;
		else
		{
			//ti.set_response_time(response_time);
			return response_time;
		}	
	}
	return test_end + 100;
}

bool RTA_PFP_GS::alloc_schedulable()
{
	ulong response_bound;
	for(uint t_id = 0; t_id < tasks.get_taskset_size(); t_id++)
	{
		Task& task = tasks.get_task_by_id(t_id);
		if(task.get_partition() == MAX_INT)
			continue;

		response_bound = response_time(task);
		task.set_response_time(response_bound);

		if(response_bound > task.get_deadline())
			return false;
	}
	return true;
}


bool RTA_PFP_GS::alloc_schedulable(Task& ti)
{
	if(MAX_INT == ti.get_partition())
		return false;
	
	ulong response_bound = response_time(ti);
	ti.set_response_time(response_bound);

	if(response_bound > ti.get_deadline())
		return false;
	else
		return true;
}

long RTA_PFP_GS::pfp_gs_tryAssign(Task& ti, uint p_id)
{
	long s = 0x7fffffffffffffff;
	Processor& processor = processors.get_processors()[p_id];
//cout<<"<================= try assign task:"<<ti.get_id()<<" to processor:"<<p_id<<" =================>"<<endl;

	//temporarily assign ti to processor[p_id]
	processor.add_task(&ti);
	ti.set_partition(p_id);

	foreach_task_assign_to_other_processor(tasks.get_tasks(), p_id, tx)
	{
		if(MAX_INT == tx->get_partition())
			continue;
//		cout<<"task"<<tx->get_id();
		if(!alloc_schedulable(*tx))
		{
			processor.remove_task(&ti);
			ti.set_partition(MAX_INT);
//			cout<<"X";
			return -1;
		}
	}

	TaskQueue& taskqueue = processor.get_taskqueue();//Task set U(all task assigned to processor[p_id])
	TaskQueue U = taskqueue;
	uint queue_size = U.size();

	foreach_task_assign_to_processor(tasks.get_tasks(), p_id, tx)
	{
		tx->set_priority(0);
	}

	for(int pi = queue_size - 1; pi >= 0; pi--) // pi from |U| -> 0
	{
		TaskQueue C;
/*
		foreach_task_assign_to_processor(tasks.get_tasks(), p_id, tx)
		{
			tx->set_priority(pi);
			if(alloc_schedulable(*tx))
				C.push_back(tx);
			tx->set_priority(0);
		}
*/
		foreach(U, taskptr)
		{
			((Task*)(*taskptr))->set_priority(pi);
			if(alloc_schedulable(*((Task*)(*taskptr))))
				C.push_back(*taskptr);
			((Task*)(*taskptr))->set_priority(0);
		}

		//Assign priority to the max period task in U
		ulong max_period = 0;
		TaskQueue::iterator it;
		foreach(U, taskptr)
		{
			if(max_period < ((Task*)(*taskptr))->get_period())
			{
				it = taskptr;
				max_period = ((Task*)(*taskptr))->get_period();
			}
		}
		((Task*)(*it))->set_priority(pi);

		//remove the task assigned to te priority
		U.remove(*it);

	}

	foreach(taskqueue, ti)
	{
		long slack = ((Task*)(*ti))->get_period() - ((Task*)(*ti))->get_response_time();
//cout<<"slack:"<<slack<<endl;
		if(s > slack)
		{
			s = slack;	
/*
cout<<"period_i:"<<period_i<<endl;
cout<<"response_time_i:"<<response_time_i<<endl;
cout<<"s:"<<s<<endl;
*/
		}
	}

//remove ti from processor[p_id]
	processor.remove_task(&ti);
	ti.set_partition(MAX_INT);

	return s;
}

typedef struct
{
	uint p_id;
	long s;
	uint priority;
}gs_tryAssign;

bool RTA_PFP_GS::is_schedulable()//Greedy Slacker heuristic partitioning
{
	tasks.DM_Order();
	tasks.init();
	uint p_num = processors.get_processor_num();

	foreach(tasks.get_tasks(), tx)
	{
		vector<gs_tryAssign> C;

		for(uint p_id = 0; p_id < p_num; p_id++)
		{
			long s = pfp_gs_tryAssign((*tx), p_id);
//			cout<<"TryAssign task_"<<tx->get_id()<<" to proc_"<<p_id<<" slack:"<<s<<endl;
			if(0 <= s)
			{
				gs_tryAssign temp;
				temp.p_id = p_id;
				temp.s = s;
				temp.priority = tx->get_priority();
				C.push_back(temp);
			}
		}
		if(0 == C.size())
			return false;
		else
		{
			long max_s = 0;
			uint assignment;
			uint priority;
			foreach(C, c)
			{
//				cout<<" "<<c->s<<" "<<c->p_id<<" "<<c->priority<<endl;
				if(max_s < c->s)
				{
					max_s = c->s;
					assignment = c->p_id;
					priority = c->priority;
				}
			}
			Processor& processor = processors.get_processors()[assignment];
			processor.add_task(&(*tx));
			tx->set_partition(assignment);
			tx->set_priority(priority);
		}
	}

	return true;
}

















