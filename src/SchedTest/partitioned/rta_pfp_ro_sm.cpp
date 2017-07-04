#include "rta_pfp_ro_sm.h"
#include <assert.h>
#include "iteration-helper.h"
#include "math-helper.h"

RTA_PFP_RO_SM::RTA_PFP_RO_SM():PartitionedSched(false, RTA, FIX_PRIORITY, DPCP, "", "Resource-Oriented") {}

RTA_PFP_RO_SM::RTA_PFP_RO_SM(TaskSet tasks, ProcessorSet processors, ResourceSet resources):PartitionedSched(false, RTA, FIX_PRIORITY, DPCP, "", "Resource-Oriented")
{
	this->tasks = tasks;
	this->processors = processors;
	this->resources = resources;

	this->resources.update(&(this->tasks));
	this->processors.update(&(this->tasks), &(this->resources));
	
	this->processors.init();
}

RTA_PFP_RO_SM::~RTA_PFP_RO_SM() {}

ulong RTA_PFP_RO_SM::blocking_bound(Task& ti, uint r_id)
{
	ulong bound = 0;
	Request& request_q = ti.get_request_by_id(r_id);
	Resource& resource_q = resources.get_resources()[r_id];
	foreach_lower_priority_task(tasks.get_tasks(), ti, tl)
	{
		foreach(tl->get_requests(), request_v)
		{
			Resource& resource_v = resources.get_resources()[request_v->get_resource_id()];
			if(resource_v.get_ceiling() <= resource_q.get_ceiling())
			{
				ulong L_l_v = request_v->get_max_length();
				if(L_l_v > bound)
					bound = L_l_v;
			}
		}
	}
	return bound;
}

ulong RTA_PFP_RO_SM::request_bound(Task& ti, uint r_id)
{
	ulong deadline = ti.get_deadline();
	Request& request_q = ti.get_request_by_id(r_id);
	Resource& resource_q = resources.get_resources()[r_id];
	uint p_id = resource_q.get_locality();
	ulong test_start = request_q.get_max_length() + blocking_bound(ti, r_id);
	ulong bound = test_start;

	while(bound <= deadline)
	{
		ulong temp = test_start;
		foreach_higher_priority_task(tasks.get_tasks(), ti, th)
		{
			foreach(th->get_requests(), request_v)
			{
				Resource& resource_v = resources.get_resources()[request_v->get_resource_id()];
				if(p_id == resource_v.get_locality())
				{
					temp += CS_workload(*th, request_v->get_resource_id(), bound);
				}
			}
		}

		assert(temp >= bound);

		if(temp != bound)
			bound = temp;
		else
		{
//			cout<<"request bound 1:"<<bound<<endl;
			return bound;
		}
	}

//			cout<<"request bound 2:"<<bound<<endl;
	return deadline + 100;
}

ulong RTA_PFP_RO_SM::formula_30(Task& ti, uint p_id, ulong interval)
{
	ulong miu = 0;

	foreach(ti.get_requests(), request)
	{
		uint q = request->get_resource_id();
		Resource& resource = resources.get_resources()[q];
		if(p_id == resource.get_locality())
		{
			miu += request->get_num_requests() * request->get_max_length();
		}
	}

	foreach_task_except(tasks.get_tasks(), ti, tj)
	{
		foreach(tj->get_requests(), request_v)
		{
			uint v = request_v->get_resource_id();
			Resource& resource_v = resources.get_resources()[v];
			if(p_id == resource_v.get_locality())
			{
				miu += CS_workload(*tj, v, interval);
			}
		}
	}
	return miu;
}

ulong RTA_PFP_RO_SM::angent_exec_bound(Task& ti, uint p_id, ulong interval)
{
	ulong deadline = ti.get_deadline();
	ulong lambda = 0;
	foreach(ti.get_requests(), request)
	{
		uint q = request->get_resource_id();
		Resource& resource = resources.get_resources()[q];

		if(p_id == resource.get_locality())
		{
			uint N_i_q = request->get_num_requests();
			ulong r_b = request_bound(ti, q);
			if(deadline < r_b)
			{
//				cout<<"exceed."<<endl;
				return r_b;
			}
			lambda += N_i_q*r_b;
		}		
	}

	ulong miu = formula_30(ti, p_id, interval);

	return min(lambda, miu);
}

ulong RTA_PFP_RO_SM::NCS_workload(Task& ti, ulong interval)
{
	ulong e = ti.get_wcet_non_critical_sections();
	ulong p = ti.get_period();
	ulong r = ti.get_response_time();
	return ceiling((interval + r - e), p) * e;
}

ulong RTA_PFP_RO_SM::CS_workload(Task& ti, uint resource_id, ulong interval)
{
	ulong p = ti.get_period();
	ulong r = ti.get_response_time();
	Request& request = ti.get_request_by_id(resource_id);
	ulong agent_length = request.get_num_requests() * request.get_max_length();
	return ceiling((interval + r - agent_length), p) * agent_length;
}

ulong RTA_PFP_RO_SM::response_time_AP(Task& ti)
{
	uint p_id = ti.get_partition();
	ulong test_bound = ti.get_deadline();
	ulong test_start = ti.get_wcet_non_critical_sections();
	ulong response_time = test_start;

#if RTA_DEBUG == 1
	cout<<"AP "<<"Task"<<ti.get_id()<<" priority:"<<ti.get_priority()<<": partition:"<<ti.get_partition()<<endl;
	cout<<"ncs-wcet:"<<ti.get_wcet_non_critical_sections()<<" cs-wcet:"<<ti.get_wcet_critical_sections()<<" wcet:"<<ti.get_wcet()<<" response time:"<<ti.get_response_time()<<" deadline:"<<ti.get_deadline()<<" period:"<<ti.get_period()<<" utilization:"<<ti.get_utilization().get_d()<<endl;
	foreach(ti.get_requests(), request)
	{
		cout<<"request"<<request->get_resource_id()<<":"<<" resource_u"<<resources.get_resources()[request->get_resource_id()].get_utilization().get_d()<<" num:"<<request->get_num_requests()<<" length:"<<request->get_max_length()<<" locality:"<<resources.get_resources()[request->get_resource_id()].get_locality()<<endl;
	}
#endif
	while(response_time <= test_bound)
	{
		ulong temp = test_start;
		ulong temp2 = temp;

		foreach_higher_priority_task(tasks.get_tasks(), ti, th)
		{
#if RTA_DEBUG == 1
			cout<<"Th:"<<th->get_id()<<" partition:"<<th->get_partition()<<" priority:"<<th->get_priority()<<endl;
#endif
			if (th->get_partition() == ti.get_partition())
			{
				temp += NCS_workload(*th, response_time);
			}
		}
#if RTA_DEBUG == 1
		cout<<"NCS workload:"<<temp-test_start<<endl;
		temp2 = temp;
#endif

		for(uint k = 0; k < processors.get_processor_num(); k++)
		{
			if(p_id == k)
				continue;
			ulong agent_bound = angent_exec_bound(ti, k, response_time);
//			cout<<"AB of processor "<<k<<":"<<agent_bound<<endl;
			temp += agent_bound;
		}
#if RTA_DEBUG == 1
		cout<<"agent exec bound:"<<temp-temp2<<endl;

		cout<<"response time:"<<temp<<endl;
#endif

		assert(temp >= response_time);

//cout<<"t"<<ti.get_id()<<": wcet:"<<ti.get_wcet()<<" deadline:"<<ti.get_deadline()<<" rt:"<<temp<<endl;

		if(temp != response_time)
		{
			response_time = temp;
		}
		else if(temp == response_time)
		{
#if RTA_DEBUG == 1
			cout<<"=============================="<<endl;
#endif
			return response_time;
		}
	}
//	cout<<"AP miss."<<endl;
#if RTA_DEBUG == 1
	cout<<"=============================="<<endl;
#endif
	return test_bound + 100;
}

ulong RTA_PFP_RO_SM::response_time_SP(Task& ti)
{
	uint p_id = ti.get_partition();
	ulong test_bound = ti.get_deadline();
	ulong test_start = ti.get_wcet_non_critical_sections();
	ulong response_time = test_start;

#if RTA_DEBUG == 1
	cout<<"SP "<<"Task"<<ti.get_id()<<": partition:"<<ti.get_partition()<<endl;
	cout<<"ncs-wcet:"<<ti.get_wcet_non_critical_sections()<<" cs-wcet:"<<ti.get_wcet_critical_sections()<<" wcet:"<<ti.get_wcet()<<" response time:"<<ti.get_response_time()<<" deadline:"<<ti.get_deadline()<<" period:"<<ti.get_period()<<" utilization:"<<ti.get_utilization().get_d()<<endl;
	foreach(ti.get_requests(), request)
	{
		cout<<"request"<<request->get_resource_id()<<":"<<" resource_u"<<resources.get_resources()[request->get_resource_id()].get_utilization().get_d()<<" num:"<<request->get_num_requests()<<" length:"<<request->get_max_length()<<" locality:"<<resources.get_resources()[request->get_resource_id()].get_locality()<<endl;
	}
#endif

	while(response_time <= test_bound)
	{
		ulong temp = test_start;
		ulong temp2 = temp;

		foreach(ti.get_requests(), request)//A
		{
			uint q = request->get_resource_id();
			Resource& resource = resources.get_resources()[q];
			if(p_id == resource.get_locality())
			{
				temp += request->get_num_requests() * request->get_max_length();
			}
		}

#if RTA_DEBUG == 1	
		cout<<"A:"<<temp-temp2<<endl;
		temp2 = temp;
#endif

		foreach_higher_priority_task(tasks.get_tasks(), ti, th)//W
		{
			if(p_id == th->get_partition())
			{
				temp += NCS_workload(*th, response_time);
			}
		}

#if RTA_DEBUG == 1
		cout<<"NCS workload:"<<temp-temp2<<endl;
		temp2 = temp;
#endif

		foreach_task_except(tasks.get_tasks(), ti, tj)
		{
			foreach(tj->get_requests(), request_v)
			{
				uint v = request_v->get_resource_id();
				Resource& resource_v = resources.get_resources()[v];
				if(p_id == resource_v.get_locality())
				{
					temp += CS_workload(*tj, v, response_time);
				}
			}
		}
#if RTA_DEBUG == 1
		cout<<"CS workload:"<<temp-temp2<<endl;
		temp2 = temp;
#endif

		for(uint k = 0; k < processors.get_processor_num(); k++)//Theata
		{
			if(p_id == k)
				continue;

			temp += angent_exec_bound(ti, k, response_time);
		}
#if RTA_DEBUG == 1
		cout<<"Theata:"<<temp-temp2<<endl;
		temp2 = temp;
		
		cout<<"response time:"<<temp<<endl;
#endif

		assert(temp >= response_time);

//		cout<<"t"<<ti.get_id()<<": wcet:"<<ti.get_wcet()<<" deadline:"<<ti.get_deadline()<<" rt:"<<temp<<endl;

		if(temp != response_time)
		{
			response_time = temp;
		}
		else if(temp == response_time)
		{
#if RTA_DEBUG == 1
			cout<<"=============================="<<endl;
#endif
			return response_time;
		}
	}
//	cout<<"SP miss."<<endl;
#if RTA_DEBUG == 1
	cout<<"=============================="<<endl;
#endif
	return test_bound + 100;
}

bool RTA_PFP_RO_SM::worst_fit_for_resources(uint active_processor_num)
{
/*
foreach(resources.get_resources(), resource)
{
	cout<<"task size:"<<resource->get_tasks()->get_taskset_size()<<endl;
	cout<<"resource:"<<resource->get_resource_id()<<" utilization:"<<resource->get_utilization();
}
*/
	resources.sort_by_utilization();

	foreach(resources.get_resources(), resource)
	{
		if(abs(resource->get_utilization()) <= _EPS)
			continue;
		fraction_t r_utilization = 1;
		uint assignment = 0;
		for(uint i = 0; i < active_processor_num; i++)
		{
			Processor& p_temp = processors.get_processors()[i];
			if(r_utilization > p_temp.get_resource_utilization())
			{
				r_utilization = p_temp.get_resource_utilization();
				assignment = i;
			}
		}
		Processor& processor = processors.get_processors()[assignment];
		if(processor.add_resource(resource->get_resource_id()))
		{
			resource->set_locality(assignment);
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool RTA_PFP_RO_SM::is_first_fit_for_tasks_schedulable(uint start_processor)
{
	bool schedulable;
	uint p_num = processors.get_processor_num();
//	tasks.RM_Order();
	foreach(tasks.get_tasks(), ti)
	{
		uint assignment;
		schedulable = false;
		for(uint i = start_processor; i < start_processor + p_num; i++)
		{
			assignment = i%p_num;
			Processor& processor = processors.get_processors()[assignment];

			if(processor.add_task(ti->get_id()))
			{
				ti->set_partition(assignment);
				if(alloc_schedulable(*ti))
				{
					schedulable = true;
					break;
				}
				else
				{
					ti->init();
					processor.remove_task(ti->get_id());
				}
			}
		}
		if(!schedulable)
		{
			return schedulable;
		}
	}
	return schedulable;
}

bool RTA_PFP_RO_SM::alloc_schedulable()
{
	ulong response_bound = 0;
	foreach(tasks.get_tasks(), ti)
	{
		uint p_id = ti->get_partition();
		if(MAX_INT == p_id)
			continue;
		
		Processor& processor = processors.get_processors()[p_id];
		if(0 == processor.get_resourcequeue().size())//Application Processor
		{
			response_bound = response_time_AP((*ti));
		}
		else//Synchronization Processor
		{
			response_bound = response_time_SP((*ti));
		}
		if(response_bound <= ti->get_deadline())
		{
			ti->set_response_time(response_bound);
		}
		else
		{	
			return false;
		}
	}
	return true;
}

bool RTA_PFP_RO_SM::alloc_schedulable(Task& ti)
{
	uint p_id = ti.get_partition();
	if(MAX_INT == p_id)
		return false;

	Processor& processor = processors.get_processors()[p_id];
	ulong response_bound = 0;
	if(0 == processor.get_resourcequeue().size())//Application Processor
	{
		response_bound = response_time_AP(ti);
		ti.set_response_time(response_bound);
	}
	else//Synchronization Processor
	{
		response_bound = response_time_SP(ti);
		ti.set_response_time(response_bound);
	}

	if(response_bound <= ti.get_deadline())
	{
		return true;
	}
	else
	{	
		return false;
	}
}


int RTA_PFP_RO_SM::ROP_SM_Order(Task t1, Task t2)
{
	long slack1 = t1.get_deadline();
	long slack2 = t2.get_deadline();
	slack1 -= t1.get_wcet() + t1.get_other_attr();
	slack2 -= t2.get_wcet() + t2.get_other_attr();
	return slack1 < slack2;
}

bool RTA_PFP_RO_SM::is_schedulable()
{
	bool schedulable = false;
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();

	for(uint i = 1; i <= p_num; i++)
	{
		//initialzation
		tasks.init();
		processors.init();
		resources.init();

		if(!worst_fit_for_resources(i))
			continue;


/*
cout<<"Before sorting."<<endl;
foreach(tasks.get_tasks(), task)
{
	long slack = task->get_deadline();
	slack -= task->get_wcet() + task->get_other_attr();
	cout<<"Task:"<<task->get_id()<<" priority:"<<task->get_priority()<<" Slack:"<<slack<<endl;
	cout<<"----------------------------"<<endl;
}
*/


		//ROP-SM Order
		foreach(tasks.get_tasks(), task)
		{
			ulong rr_delay = 0;
			for(uint k = 0; k < p_num; k++)
			{
				rr_delay += formula_30(*task, k, task->get_deadline());
			}
			task->set_other_attr(rr_delay);
		}

		Tasks& taskset = tasks.get_tasks();
		sort(taskset.begin(), taskset.end(), ROP_SM_Order);
		for(int t = 0; t < taskset.size(); t++)
		{
			taskset[t].set_index(t);
			//taskset[t].set_priority(t);
		}

/*
cout<<"After sorting."<<endl;
foreach(tasks.get_tasks(), task)
{
	long slack = task->get_deadline();
	slack -= task->get_wcet() + task->get_other_attr();
	cout<<"Task:"<<task->get_id()<<" priority:"<<task->get_priority()<<" Slack:"<<slack<<endl;
	cout<<"----------------------------"<<endl;
}
*/

		schedulable = is_first_fit_for_tasks_schedulable(i%p_num);
		if(schedulable)
		{

/*
			foreach(tasks.get_tasks(), task)
			{
				cout<<"Task:"<<task->get_id()<<" priority:"<<task->get_priority()<<" partition:"<<task->get_partition()<<endl;
				cout<<"ncs-wcet:"<<task->get_wcet_non_critical_sections()<<" cs-wcet:"<<task->get_wcet_critical_sections()<<" wcet:"<<task->get_wcet()<<" response time:"<<task->get_response_time()<<" deadline:"<<task->get_deadline()<<" period:"<<task->get_period()<<endl;
				foreach(task->get_requests(), request)
				{
					cout<<"request"<<request->get_resource_id()<<":"<<" num:"<<request->get_num_requests()<<" length:"<<request->get_max_length()<<" locality:"<<resources.get_resources()[request->get_resource_id()].get_locality()<<endl;
				}
				cout<<"-------------------------------------------"<<endl;
			}
*/


			return schedulable;
		}
	}
/*
	foreach(tasks.get_tasks(), task)
	{
		cout<<"Task"<<task->get_id()<<": partition:"<<task->get_partition()<<endl;
		cout<<"ncs-wcet:"<<task->get_wcet_non_critical_sections()<<" cs-wcet:"<<task->get_wcet_critical_sections()<<" wcet:"<<task->get_wcet()<<" response time:"<<task->get_response_time()<<" deadline:"<<task->get_deadline()<<" period:"<<task->get_period()<<" utilization:"<<task->get_utilization().get_d()<<endl;
		foreach(task->get_requests(), request)
		{
			cout<<"request"<<request->get_resource_id()<<":"<<" num:"<<request->get_num_requests()<<" length:"<<request->get_max_length()<<" locality:"<<resources.get_resources()[request->get_resource_id()].get_locality()<<endl;
		}
		cout<<"-------------------------------------------"<<endl;
	}
*/
	return schedulable;
}

