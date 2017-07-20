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

	this->resources.update(&(this->tasks));
	this->processors.update(&(this->tasks), &(this->resources));

	this->tasks.Density_Decrease_Order();
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
//			cout<<"global resource:"<<resource_q.get_resource_id()<<endl;
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
	if(MAX_INT == ti.get_partition())
		return ti.get_deadline();
//cout<<"RTA for task:"<<ti.get_id()<<endl;
//cout<<"wcet:"<<ti.get_wcet()<<" period:"<<ti.get_period()<<" deadline:"<<ti.get_deadline()<<" r_t:"<<ti.get_response_time()<<endl;
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
		cout<<"Task"<<tx->get_id()<<" partition:"<<tx->get_partition()<<" priority:"<<tx->get_priority()<<endl;
	}
*/
	while(response_time + jitter <= test_end)
	{
		test_start = wcet + remote_blocking + max(local_blocking, NP_blocking);
//cout<<"remote blocking:"<<remote_blocking<<" local_b:"<<local_blocking<<" NP_b:"<<NP_blocking<<endl;
		foreach_higher_priority_task(tasks.get_tasks(), ti, th)
		{
			if(ti.get_partition() == th->get_partition())
			{
				ulong wcet_h = th->get_wcet();
				ulong period_h = th->get_period();
				ulong jitter_h = th->get_jitter();
				ulong remote_blocking_h = th->get_remote_blocking();
				ulong interference = ceiling(response_time + jitter_h, period_h)*(wcet_h + remote_blocking_h);
				test_start += interference;
//cout<<"Task:"<<th->get_id()<<" wcet:"<<th->get_wcet()<<" period:"<<th->get_period()<<" deadline:"<<th->get_deadline()<<" r_t:"<<th->get_response_time()<<" interference:"<<interference<<endl;
//cout<<"........................"<<endl;
			}
		}

		assert(test_start >= response_time);
		
		if(test_start != response_time)
			response_time = test_start;
		else
		{
			//ti.set_response_time(response_time);
//			cout<<"rt:"<<response_time<<endl;
			return response_time;
		}	
	}
	
//	cout<<"rt(miss):"<<response_time<<endl;
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
//cout<<"rb of task "<<ti.get_id()<<":"<<response_bound<<endl;
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
/*
foreach_task_assign_to_processor(tasks.get_tasks(), p_id, task)
{
	cout<<"Task:"<<task->get_id()<<" priority:"<<task->get_priority()<<endl;
}
*/
//cout<<""<<endl;
	//store original priority
	Priorities origin;
	foreach_task_assign_to_processor(tasks.get_tasks(), p_id, task)
	{
		priority_storage ps;
		ps.t_id = task->get_id();
		ps.priority = task->get_priority();
		origin.push_back(ps);
	}



	//temporarily assign ti to processor[p_id]
	if(!processor.add_task(ti.get_id()))
	{
//cout<<"add task failed!"<<endl;
		return -1;
	}
	ti.set_partition(p_id);

	foreach_task_assign_to_other_processor(tasks.get_tasks(), p_id, tx)
	{
		if(MAX_INT == tx->get_partition())
			continue;
//		cout<<"other processor task"<<tx->get_id();
		if(!alloc_schedulable(*tx))
		{
			processor.remove_task(ti.get_id());
			ti.set_partition(MAX_INT);
//			cout<<"X";
			return -1;
		}
	}

	set<uint> taskqueue = processor.get_taskqueue();//Task set U(all task assigned to processor[p_id])
	set<uint> U = taskqueue;
	uint queue_size = U.size();

	foreach(U, t_id)
	{
		Task& task = tasks.get_task_by_id(*t_id);
		task.set_priority(0);
	}

	for(int pi = queue_size - 1; pi >= 0; pi--) // pi from |U| -> 0
	{
//cout<<"try to assign priority "<<pi<<" on processor "<<p_id<<endl;
		set<uint> C;
/*
		foreach_task_assign_to_processor(tasks.get_tasks(), p_id, tx)
		{
			tx->set_priority(pi);
			if(alloc_schedulable(*tx))
				C.push_back(tx);
			tx->set_priority(0);
		}
*/
		foreach(U, t_id)
		{
			Task& task = tasks.get_task_by_id(*t_id);
			task.set_priority(pi);
			if(alloc_schedulable(task))
				C.insert(*t_id);
			task.set_priority(0);
		}

		if(0 == C.size())
		{
			//restore original priority
			foreach(origin, pare)
			{
				Task& task = tasks.get_task_by_id(pare->t_id);
				task.set_priority(pare->priority);
			}

			processor.remove_task(ti.get_id());
			ti.set_partition(MAX_INT);
			return -1;
		}

		//Assign priority to the max period task in C
		ulong max_period = 0;
		uint max_id;
		foreach(C, t_id)
		{
			Task& task = tasks.get_task_by_id(*t_id);
//			cout<<"task:"<<((Task*)(*taskptr))->get_id()<<" partition:"<<((Task*)(*taskptr))->get_partition()<<" wcet:"<<((Task*)(*taskptr))->get_wcet()<<" period:"<<((Task*)(*taskptr))->get_period()<<" response time:"<<((Task*)(*taskptr))->get_response_time()<<endl;
			if(max_period < task.get_period())
			{
				max_id = *t_id;
				max_period = task.get_period();
			}
		}
		tasks.get_task_by_id(max_id).set_priority(pi);

		//remove the task assigned to te priority
		U.erase(max_id);

	}

	foreach(taskqueue, t_id)
	{
		Task& task = tasks.get_task_by_id(*t_id);
		long slack = task.get_period() - task.get_response_time();

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
	processor.remove_task(ti.get_id());
	ti.set_partition(MAX_INT);

	return s;
}

bool RTA_PFP_GS::is_schedulable()//Greedy Slacker heuristic partitioning
{
	tasks.Density_Decrease_Order();
	tasks.init();
	uint p_num = processors.get_processor_num();
/*
cout<<"=====start====="<<endl;
foreach(tasks.get_tasks(), task)
{
	cout<<"Task:"<<task->get_id()<<" Partition:"<<task->get_partition()<<" priority:"<<task->get_priority()<<endl;
	foreach(task->get_requests(), request)
		cout<<request->get_resource_id()<<"\t";
	cout<<endl;
	cout<<"----------------------------"<<endl;
}


foreach(resources.get_resources(), resource)
{
	cout<<"Resource:"<<resource->get_resource_id();
	cout<<" ceiling:"<<resource->get_ceiling();
	cout<<" global:"<<(resource->is_global_resource()?"yes":"no")<<endl;
	resource->display_task_queue();
}
*/
	foreach(tasks.get_tasks(), tx)
	{
		vector<gs_tryAssign> C;

//		vector<PS> OA;//Original Assignments
//		vector<PS> SA;//Successful Assignments
		
		for(uint p_id = 0; p_id < p_num; p_id++)
		{
/*
			PS original_prioritys;
			foreach(processors.get_processors()[p_id].get_taskqueue(), task)
			{
				priority_storage ps;
				ps.t_id = ((Task*)(*task))->get_id();
				ps.priority = ((Task*)(*task))->get_priority();
				original_prioritys.push_back(ps);
			}
			OA.push_back(prioritys);
*/
//			cout<<"TryAssign task_"<<tx->get_id()<<" to proc_"<<p_id<<endl;
			long s = pfp_gs_tryAssign((*tx), p_id);
/*		
			PS new_prioritys;
			foreach(processors.get_processors()[p_id].get_taskqueue(), task)
			{
				priority_storage ps;
				ps.t_id = ((Task*)(*task))->get_id();
				ps.priority = ((Task*)(*task))->get_priority();
				new_prioritys.push_back(ps);
			}
			SA.push_back(prioritys);
*/
//			cout<<s<<endl;
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
		{
/*
cout<<"=====fail====="<<endl;
			foreach(tasks.get_tasks(), task)
			{
				cout<<"Task:"<<task->get_id()<<" Partition:"<<task->get_partition()<<" priority:"<<task->get_priority()<<" U:"<<task->get_utilization().get_d()<<endl;
				cout<<"----------------------------"<<endl;
			}

	for(uint k = 0; k < p_num; k++)
	{
		cout<<"Processor "<<k<<" utilization:"<<processors.get_processors()[k].get_utilization().get_d()<<endl;
		foreach(processors.get_processors()[k].get_taskqueue(), t_id)
		{
			Task& task = tasks.get_task_by_id(*t_id);
			cout<<"task"<<task.get_id()<<"\t";
		}
		cout<<endl;
	}
*/
			return false;
		}
		else
		{
			long max_s = -1;
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
//			cout<<"max: "<<max_s<<" "<<assignment<<" "<<priority<<endl;
			Processor& processor = processors.get_processors()[assignment];
			processor.add_task(tx->get_id());
			tx->set_partition(assignment);
			tx->set_priority(priority);

/*
			for(uint p_id = 0; p_id < p_num; p_id++)
			{
				if(assignment == p_id)
				{

				}
				else
				{

				}
			}
*/

		}
	}

cout<<"=====success====="<<endl;
	foreach(tasks.get_tasks(), task)
	{
		cout<<"Task:"<<task->get_id()<<" Partition:"<<task->get_partition()<<" priority:"<<task->get_priority()<<" U:"<<task->get_utilization().get_d()<<endl;
		cout<<"----------------------------"<<endl;
	}

	for(uint k = 0; k < p_num; k++)
	{
		cout<<"Processor "<<k<<" utilization:"<<processors.get_processors()[k].get_utilization().get_d()<<endl;
		foreach(processors.get_processors()[k].get_taskqueue(), t_id)
		{
			Task& task = tasks.get_task_by_id(*t_id);
			cout<<"task"<<task.get_id()<<"\t";
		}
		cout<<endl;
	}

	return true;
}

















