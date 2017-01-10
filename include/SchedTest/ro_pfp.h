#ifndef RO_PFP_H
#define RO_PFP_H

//Resource-Oriented Partitioned Scheduling
#include <iostream>
#include <assert.h>
#include "types.h"
#include <tasks.h>
#include <resources.h>
#include <processors.h>

ulong ro_workload(Task& ti, ulong interval)
{
/*
cout<<"workload C:"<<endl;
foreach(ti.get_requests(), request)
{
	uint q = request->get_resource_id();
	cout<<"request for resource "<<q<<":"<<"N"<<request->get_num_requests()<<" L:"<<request->get_max_length()<<endl;
}
cout<<ti.get_wcet()<<" "<<ti.get_wcet_critical_sections()<<" "<<ti.get_wcet_non_critical_sections()<<endl;
*/
	ulong e = ti.get_wcet_non_critical_sections();
	ulong p = ti.get_period();
	ulong r = ti.get_response_time();
	return ceiling((interval + r - e), p) * e;
}

ulong ro_agent_workload(Task& ti, uint resource_id, ulong interval)
{
	ulong p = ti.get_period();
	ulong r = ti.get_response_time();
	Request& request = ti.get_request_by_id(resource_id);
	ulong agent_length = request.get_num_requests() * request.get_max_length();
	return ceiling((interval + r - agent_length), p) * agent_length;
}

ulong ro_get_bloking(Task& ti, TaskSet& tasks, ResourceSet& resources)
{
	uint p_i = ti.get_partition();
	ulong blocking = 0;
	foreach(ti.get_requests(), request_r)
	{
		ulong b_i_r = 0;
		uint N_i_r = request_r->get_num_requests();
		Resource& resource_r = resources.get_resources()[request_r->get_resource_id()];
		uint p_r = resource_r.get_locality();
		if(p_i == p_r)
			continue;
		uint ceiling_r = resource_r.get_ceiling();
		foreach_lower_priority_task(tasks.get_tasks(), ti, tx)
		{
			foreach(tx->get_requests(), request_u)
			{
				Resource& resource_u = resources.get_resources()[request_u->get_resource_id()];
				uint p_u = resource_u.get_locality();
				uint ceiling_u = resource_u.get_ceiling();
				ulong L_x_u = request_u->get_max_length();
				if(p_u == p_r && ceiling_u < ceiling_r && L_x_u > b_i_r)
				{
					b_i_r = L_x_u;
				}
			}
		}
		blocking += b_i_r * N_i_r;
	}
	return blocking;
}

ulong ro_get_interference_R(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, ulong interval)
{
	ulong IR_i = 0;
	uint p_i = ti.get_partition();
	foreach(processors.get_processors(), processor)
	{
		uint p_k = processor->get_processor_id();
		if(p_i != p_k)
		{
			foreach(ti.get_requests(), request_r)
			{
				uint r = request_r->get_resource_id();
				if(p_k == request_r->get_locality())
				{
					foreach_higher_priority_task(tasks.get_tasks(), ti, th)
					{
						foreach(th->get_requests(), request_u)
						{
							uint u = request_u->get_resource_id();
							uint p_h_u = request_u->get_locality();
							if(p_h_u == p_k)
							{
								IR_i += ro_agent_workload(*th, u, interval);
							}
						}

					}
				}
			}
		}
	}
	return IR_i;
}

ulong ro_get_interference_AC(Task& ti, TaskSet& tasks, ResourceSet& resources, ulong interval)
{
	ulong IAC_i = 0;
	foreach_higher_priority_task(tasks.get_tasks(), ti, th)
	{
		if(th->get_partition() == ti.get_partition())
			IAC_i += ro_workload(*th, interval);
	}
	return IAC_i;
}

ulong ro_get_interference_UC(Task& ti, TaskSet& tasks, ResourceSet& resources, ulong interval)
{
	uint p_i = ti.get_partition();
	ulong IAC_i = 0;
	foreach_higher_priority_task(tasks.get_tasks(), ti, th)
	{
		if(th->get_partition() == ti.get_partition())
			IAC_i += ro_workload(*th, interval);
	}
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		foreach(tx->get_requests(), request)
		{
			uint r = request->get_resource_id();
			Resource& resource = resources.get_resources()[r];
			if(p_i == resource.get_locality())
			{
				IAC_i += ro_agent_workload(*tx, r, interval);
			}
		}
	}
	return IAC_i;
}

ulong ro_get_interference(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, ulong interval)
{
	uint p_i = ti.get_partition();
	Processor& processor = processors.get_processors()[p_i];
	if(0 == processor.get_resourcequeue().size())//Application Core
	{
		return ro_get_interference_AC(ti, tasks, resources, interval);
	}
	else//Universal Core
	{
		return ro_get_interference_UC(ti, tasks, resources, interval);
	}
}

ulong rta_ro(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong test_bound = ti.get_deadline();
	ulong test_start = ti.get_wcet() + ro_get_bloking(ti, tasks, resources);
	ulong response_time = test_start;
//cout<<"response time1:"<<response_time<<endl;
//cout<<"deadline1:"<<test_bound<<endl;
	while(response_time <= test_bound)
	{
		ulong interf_C = ro_get_interference(ti, tasks, processors, resources, response_time);
		ulong interf_R = ro_get_interference_R(ti, tasks, processors, resources, response_time);
/*
cout<<"Deadline:"<<test_bound<<endl;
cout<<"WCET:"<<ti.get_wcet_non_critical_sections()<<endl;
cout<<"interf_C:"<<interf_C<<endl;
cout<<"interf_R:"<<interf_R<<endl;
*/
		ulong temp = test_start + interf_C + interf_R;

		assert(temp >= response_time);

		if(temp != response_time)
			response_time = temp;
		else if(temp == response_time)
			return response_time;
	}
//cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
//cout<<"response time2:"<<response_time<<endl;
//cout<<"deadline2:"<<test_bound<<endl;
	return test_bound + 100;
}

bool is_ro_pfp_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong response_bound;
	for(uint t_id = 0; t_id < tasks.get_taskset_size(); t_id++)
	{
		Task& task = tasks.get_task_by_id(t_id);
//cout<<"task id:"<<t_id<<endl;
		if(task.get_partition() == MAX_INT)
			continue;
		response_bound = rta_ro(task, tasks, processors, resources);
		if(response_bound <= task.get_deadline())
		{
			task.set_response_time(response_bound);
		}
		else
		{
//cout<<"response time3:"<<response_bound<<endl;
//cout<<"deadline3:"<<task.get_deadline()<<endl;	
			return false;
		}
	}
	return true;
}

bool worst_fit_for_resources(ResourceSet& resources, ProcessorSet& processors, uint active_processor_num)
{
	resources.sort_by_utilization();

	foreach(resources.get_resources(), resource)
	{
		fraction_t r_utilization = 1;
		uint assignment = 0;
		for(uint i = 0; i < active_processor_num; i++)
		{
			Processor& processor = processors.get_processors()[i];
			if(r_utilization > processor.get_resource_utilization())
			{
				r_utilization = processor.get_resource_utilization();
				assignment = i;
			}
		}
		Processor& processor = processors.get_processors()[assignment];
		if(processor.add_resource(&(*resource)))
		{
			resource->set_locality(assignment);
		}
		else
		{
//cout<<"false"<<endl;
			return false;
		}
	}
	return true;
}

bool is_first_fit_for_tasks_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint start_processor, uint TEST_TYPE, uint ITER_BLOCKING)
{

	bool schedulable;
	uint p_num = processors.get_processor_num();
	tasks.RM_Order();
//cout<<"First-fit. Start from processor "<<start_processor<<endl;
	foreach(tasks.get_tasks(), ti)
	{
		//Processor& processor = processors.get_processors()[0];
		uint assignment;
		schedulable = false;
//cout<<"task "<<ti->get_id()<<endl;
		for(uint i = start_processor; i < start_processor + p_num; i++)
		{
			assignment = i%p_num;
//cout<<"Assignment:"<<assignment<<endl;
			Processor& processor = processors.get_processors()[assignment];
//cout<<"Processor id:"<<processor.get_processor_id()<<endl;
			if(processor.add_task(&(*ti)))
			{
				ti->set_partition(assignment);
//cout<<"get_partition:"<<ti->get_partition()<<endl;
				if(is_ro_pfp_schedulable(tasks, processors, resources))
				{
					schedulable = true;
					break;
				}
				else
				{
//cout<<"remove task "<<ti->get_id()<<endl;
					ti->init();
/*
		cout<<"task(before remove): ";
	foreach(processor.get_taskqueue(), tx)
	{
		cout<<((Task*)(*tx))->get_id()<<" ";
	}
		cout<<endl;
*/
					processor.remove_task(&(*ti));
/*
	cout<<"task(after remocve): ";
	foreach(processor.get_taskqueue(), tx)
	{
		cout<<((Task*)(*tx))->get_id()<<" ";
	}
		cout<<endl;
*/
				}
			}
			else
			{
//				cout<<"assigned failed!"<<endl;
//				cout<<"task utilization:"<<ti->get_utilization()<<endl;
//				cout<<"processor utilization:"<<processor.get_utilization()<<endl;
			}
		}

		if(!schedulable)
		{
/*
cout<<"ffffffffffffffffffffffffffffffffff."<<endl;

			for(uint i = 0; i < p_num; i++)
			{
cout<<"i:"<<i<<endl;
				Processor& p_temp_f = processors.get_processors()[i];
				cout<<"utilization of processor "<<p_temp_f.get_processor_id()<<":"<<p_temp_f.get_utilization().get_d()<<endl;
			}
*/
			return schedulable;
		}
	}

	return schedulable;
}

bool is_ro_pfp_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint TEST_TYPE, uint ITER_BLOCKING)
{
	bool schedulable;

	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	
	for(uint i = 1; i <= p_num; i++)
	{
		//initialzation
		tasks.init();
		processors.init();
		resources.init();
		
		if(!worst_fit_for_resources(resources, processors, i))
			continue;
/*
cout<<"22222"<<endl;
for(uint k = 0; k < processors.get_processor_num(); k++)
			{
				Processor& p_temp_2 = processors.get_processors()[k];
				cout<<"utilization of processor "<<p_temp_2.get_processor_id()<<":"<<p_temp_2.get_utilization().get_d()<<endl;
			}
*/
		schedulable = is_first_fit_for_tasks_schedulable(tasks, processors, resources, i%p_num, TEST_TYPE, ITER_BLOCKING);
		if(schedulable)
			return schedulable;
	}
	return schedulable;
}




#endif
