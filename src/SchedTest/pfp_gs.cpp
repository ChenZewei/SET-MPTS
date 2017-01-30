#include "pfp_gs.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"
#include "assert.h"

ulong pfp_gs_local_blocking(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong blocking = 0;
	uint p_id = ti.get_partition();
	
	foreach(resources.get_resources(), resource)
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

ulong pfp_gs_spin_time(Task& ti, uint resource_id, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	uint p_num = processors.get_processor_num();
	uint p_id = ti.get_partition();
//cout<<"p_num:"<<p_num<<endl;
//cout<<"p_id:"<<p_id<<endl;
	ulong sum = 0;
//cout<<"11111"<<endl;
	for(uint i = 0; i < p_num; i++)
	{
//cout<<"22222"<<endl;
//cout<<i<<endl;
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

ulong pfp_gs_remote_blocking(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong blocking = 0;

//cout<<"1111"<<endl;
	foreach(ti.get_requests(), request)
	{
//cout<<"2222"<<endl;
		uint q = request->get_resource_id();
		blocking += request->get_num_requests()*pfp_gs_spin_time(ti, q, tasks, processors, resources);
	}
	return blocking;
}

ulong pfp_gs_NP_blocking(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong blocking = 0;

	foreach(resources.get_resources(), resource)
	{
		if(resource->is_global_resource())
		{
			uint q = resource->get_resource_id();
			foreach(tasks.get_tasks(), tx)
			{
				if((tx->is_request_exist(q)) && (ti.get_partition() == tx->get_partition()) && (ti.get_id() < tx->get_id()))
				{
					ulong length = pfp_gs_spin_time(ti, q, tasks, processors, resources) + tx->get_request_by_id(q).get_max_length();
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
//cout<<"111"<<endl;
	ulong wcet = ti.get_wcet();
	ti.set_response_time(wcet);
//cout<<"222"<<endl;
	ulong local_blocking = pfp_gs_local_blocking(ti, tasks, processors, resources);
//cout<<"333"<<endl;
	ulong remote_blocking = pfp_gs_remote_blocking(ti, tasks, processors, resources);
//cout<<"444"<<endl;
	ulong NP_blocking = pfp_gs_NP_blocking(ti, tasks, processors, resources);
//cout<<"555"<<endl;
	ulong response_time = ti.get_response_time();
	ulong test_start = wcet + remote_blocking + max(local_blocking, NP_blocking);
	ulong test_end = ti.get_deadline();

	ti.set_local_blocking(local_blocking);
	ti.set_remote_blocking(remote_blocking);
	ti.set_total_blocking(local_blocking + remote_blocking);
//cout<<"666"<<endl;
	while(response_time < test_end)
	{
//cout<<"777"<<endl;
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

//cout<<"888"<<endl;
//cout<<test_start<<" "<<response_time<<endl;
		assert(test_start >= response_time);
		
		if(test_start != response_time)
			response_time = test_start;
		else
		{
			ti.set_response_time(response_time);
			return response_time;
		}
//cout<<"999"<<endl;		
	}
	return test_end + 100;
}

bool is_pfp_gs_tryAssign_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong response_bound;
	for(uint t_id = 0; t_id < tasks.get_taskset_size(); t_id++)
	{
		Task& task = tasks.get_task_by_id(t_id);
//cout<<"task:"<<t_id<<" partition:"<<task.get_partition()<<endl;
		if(task.get_partition() == MAX_INT)
			continue;
		response_bound = pfp_gs_response_time(task, tasks, processors, resources);
//cout<<"-----"<<endl;
//cout<<"response_bound:"<<response_bound<<endl;
//cout<<"deadline:"<<task.get_deadline()<<endl;
		if(response_bound > task.get_deadline())	
			return false;
	}
//cout<<"true"<<endl;
	return true;
}

ulong pfp_gs_tryAssign(Task& ti, uint p_id, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong s = MAX_LONG;
	Processor& processor = processors.get_processors()[p_id];
//cout<<"111"<<endl;
	//temporarily assign ti to processor[p_id]
	processor.add_task(&ti);
	ti.set_partition(p_id);

	TaskQueue& taskqueue = processor.get_taskqueue();//Task set U(all task assigned to processor[p_id])
	TaskQueue U = taskqueue;
	uint queue_size = U.size();
//cout<<"queue size:"<<queue_size<<endl;
//cout<<"222"<<endl;

	foreach(taskqueue, task)
	{
		Task *tx = ((Task*)(*task));
		tx->set_priority(MAX_INT);
	}

	for(int pi = queue_size - 1; pi >= 0; pi--)
	{
//cout<<pi<<endl;
//cout<<queue_size-1<<endl;
//cout<<"333"<<endl;
		
		TaskQueue C = U;
		TaskQueue C2;

		foreach(C, task)
		{
//cout<<"task:"<<((Task*)(*task))->get_id();
			Task *tx = ((Task*)(*task));
//cout<<"1"<<endl;	
			if(tx->get_partition() != p_id)
			{
//cout<<"test!!!!!!!!!!!!!!"<<endl;
				continue;
			}

//cout<<"2"<<endl;	
			if((MAX_INT == tx->get_priority()) || (pi == tx->get_priority()))
			//if(MAX_INT == tx->get_priority())
			{
				tx->set_priority(pi);
				if(is_pfp_gs_tryAssign_schedulable(tasks, processors, resources))
				{
					tx->set_priority(MAX_INT);
					//C.remove(*task);
					//continue;
					C2.push_back(*task);
				}
				
				tx->set_priority(MAX_INT);
			}
//cout<<"3"<<endl;	
		}
//cout<<"444"<<endl;
		if(0 == C2.size())
		{
			//temporarily remove ti from processor[p_id]
			processor.remove_task(&ti);
			ti.set_partition(MAX_INT);
			return -1;
		}
		else
		{
			ulong max_period = 0;
			TaskQueue::iterator it;
			foreach(C2, task)
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
					U.remove(*it);
			}
		}
//cout<<"555"<<endl;
	}
//cout<<"666"<<endl;

	foreach(U, ti)
	{
		ulong period_i = ((Task*)(*ti))->get_period();
		ulong response_time_i = ((Task*)(*ti))->get_response_time();

		if(s > (period_i - response_time_i))
		{
			s = period_i - response_time_i;	
		}
	}
//cout<<"777"<<endl;

//temporarily remove ti from processor[p_id]
	processor.remove_task(&ti);
	ti.set_partition(MAX_INT);

	foreach(taskqueue, task)
	{
		Task *tx = ((Task*)(*task));
		cout<<"task:"<<(tx->get_id())<<endl;
		cout<<"priority:"<<(tx->get_priority())<<endl;
	}

	return s;
}

typedef struct
{
	ulong p_id;
	ulong s;
}gs_tryAssign;

bool is_pfp_gs_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)//Greedy Slacker heuristic partitioning
{
	tasks.DM_Order();
	tasks.init();
	uint p_num = processors.get_processor_num();
//cout<<""<<endl;
//cout<<"1"<<endl;
	foreach(tasks.get_tasks(), tx)
	{
		vector<gs_tryAssign> C;

		for(uint p_id = 0; p_id < p_num; p_id++)
		{
			ulong s = pfp_gs_tryAssign((*tx), p_id, tasks, processors, resources);
			if(0 <= s)
			{
				gs_tryAssign temp;
				temp.p_id = p_id;
				temp.s = s;
				C.push_back(temp);
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
//cout<<"pid,s:"<<c->p_id<<" "<<c->s<<endl;
				if(max_s < c->s)
				{
					max_s = c->s;
					assignment = c->p_id;
				}
			}
			Processor& processor = processors.get_processors()[assignment];
			processor.add_task(&(*tx));
			tx->set_partition(assignment);
		}
	}
//cout<<"3"<<endl;
/*
	foreach(tasks.get_tasks(), task)
	{
		cout<<"task:"<<task->get_id()<<endl;
		cout<<"partition:"<<task->get_partition()<<endl;
		cout<<"priority:"<<task->get_priority()<<endl;
	}
*/
	return true;
}

















