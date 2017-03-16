#ifndef RTA_PFP_H
#define RTA_PFP_H

#include <math.h>
#include <math-helper.h>
#include <algorithm>
#include <iostream>

#include "../tasks.h"
#include "../processors.h"
#include "../resources.h"

using namespace std;

ulong interf_standard(Task& task, ulong interval)
{
	return task.get_wcet() * ceiling((interval + task.get_jitter()), task.get_period());
}

// Audsly et al.'s standard RTA (1993)
ulong rta_standard(TaskSet& tasks, uint t_id, uint ITER_BLOCKING)
{
	Task& task = tasks.get_task_by_id(t_id);
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_total_blocking() + task.get_wcet();
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
				// add functions to bound "totoal blocking" here
				// XXXXXXXX				
				demand = task.get_total_blocking() + task.get_wcet();
				break;
		}

		ulong interference = 0;
		for (uint th = 0; th < t_id; th ++)
		{
			Task& tsk = tasks.get_task_by_id(th);
			if (tsk.get_partition() == task.get_partition())
			{
				interference += interf_standard(tsk, response);
			}
		}
		
		demand += interference;

		if (response == demand)
			return response + task.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

ulong interf_with_spin(Task& task, ulong interval)
{
	//cout<<"Brem:"<<task.get_spin()<<endl;
	return (task.get_wcet() + task.get_spin()) * ceiling((interval + task.get_jitter()), task.get_period());
}

// Awieder and Brandenburg's RTA with spin locks (2013)
ulong rta_with_spin(TaskSet& tasks, uint t_id, uint ITER_BLOCKING)
{
	Task& task = tasks.get_task_by_id(t_id);
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_spin() + task.get_local_blocking() + task.get_wcet();
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
				demand = task.get_local_blocking() + task.get_spin() + task.get_wcet();
				break;
		}

		ulong interference = 0;
		for (uint th = 0; th < t_id; th ++)
		{
			Task& tsk = tasks.get_task_by_id(th);
			if (task.get_partition() == tsk.get_partition())
			{
				interference += interf_with_spin(tsk, response);
			}
		}
		
		demand += interference;

		if (response == demand)
			return response + task.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

ulong transitive_preemption(TaskSet& taskset, ProcessorSet& processorset, ResourceSet& resourceset, uint t_id, uint r_id)
{

	Task& task_i = taskset.get_task_by_id(t_id);
	uint cpu = task_i.get_partition();
	if(cpu == MAX_INT)
		return 0;

	Processor& processor = processorset.get_processors()[cpu];
	TaskQueue& tq = processor.get_taskqueue();
	Resource& resource = resourceset.get_resources()[r_id];
	uint ceiling_k = resource.get_ceiling();
	ulong sum = 0;
	list<void*>::iterator it = tq.begin();
	for(uint j = 0; it != tq.end(); it++, j++)
	{
		Task* task_j = (Task*)*it;
		if(t_id == task_j->get_id())
			continue;
		Resource_Requests& rr =  task_j->get_requests();
		for(uint x = 0; x < rr.size(); x++)
		{
			Request& request_x = rr[x];
			Resource& resource_x = resourceset.get_resources()[request_x.get_resource_id()];
			if(ceiling_k > resource_x.get_ceiling())
				sum += request_x.get_max_length();
		}
	}
	return sum;
}

ulong DLB(TaskSet& taskset, ProcessorSet& processorset, ResourceSet& resourceset, uint t_id)
{
	Task& task_i = taskset.get_task_by_id(t_id);
	uint cpu = task_i.get_partition();
	Processor& processor = processorset.get_processors()[cpu];
	TaskQueue& tq = processor.get_taskqueue();
	Resource_Requests& rr_i =  task_i.get_requests();
	uint num = 1;

	for(uint i = 0; i < rr_i.size(); i++)
	{
		if(resourceset.get_resources()[rr_i[i].get_resource_id()].is_global_resource())
			num++;
	}


	ulong max_length = 0;

	list<void*>::iterator it = tq.begin();

	for(uint j = 0; it != tq.end(); it++, j++)
	{
		Task* task_j = (Task*)*it;
		if(t_id >= task_j->get_id())
			continue;
		
		Resource_Requests& rr_j =  task_j->get_requests();
		for(uint l = 0; l < rr_j.size(); l++)
		{
			Request& request_l = rr_j[l];
			Resource& resource_l = resourceset.get_resources()[request_l.get_resource_id()];
			if(resource_l.is_global_resource())
				continue;
			if(t_id > resource_l.get_ceiling() && max_length < request_l.get_max_length())
			{
				max_length = request_l.get_max_length();
			}
		}
	}
	
	return num*max_length;
}

ulong DGB_l(TaskSet& taskset, ProcessorSet& processorset, ResourceSet& resourceset, uint t_id)
{
	Task& task_i = taskset.get_task_by_id(t_id);
	Resource_Requests& rr_i =  task_i.get_requests();
	uint cpu = task_i.get_partition();

	ulong sum = 0;

	for(uint k = 0; k < rr_i.size(); k++)
	{
		uint k_id = rr_i[k].get_resource_id();
		Resource& resource_k = resourceset.get_resources()[k_id];
		uint N_i_k = rr_i[k].get_num_requests();
		if(!resource_k.is_global_resource())
			continue;
		for(uint j = t_id + 1; j < taskset.get_taskset_size(); j++)
		{

			ulong max_element = 0;
			Task& task_j = taskset.get_task_by_id(j);
			if(cpu == task_j.get_partition())
				continue;
			Resource_Requests& rr_j =  task_j.get_requests();

			for(uint l = 0; l < rr_j.size(); l++)
			{

				Request& request_l = rr_j[l];
				uint l_id = request_l.get_resource_id();

				if(l_id == k_id)
				{
					ulong temp = request_l.get_max_length() + transitive_preemption(taskset, processorset, resourceset, j, l_id);
					if(temp > max_element)
						max_element = temp;
				}

			}

			sum += N_i_k*max_element;
		}
	}
	return sum;
}

ulong DGB_h(TaskSet& taskset, ProcessorSet& processorset, ResourceSet& resourceset, uint t_id)
{
	Task& task_i = taskset.get_task_by_id(t_id);
	Resource_Requests& rr_i =  task_i.get_requests();
	uint cpu = task_i.get_partition();
	ulong sum_1 = 0;
	
	for(uint k = 0; k < rr_i.size(); k++)
	{
		uint k_id = rr_i[k].get_resource_id();
		Resource& resource_k = resourceset.get_resources()[k_id];
		uint N_i_k = rr_i[k].get_num_requests();
		if(!resource_k.is_global_resource())
			continue;
		
		ulong sum_2 = 0;
		for(uint j = 0; j < t_id; j++)
		{
			Task& task_j = taskset.get_task_by_id(j);
			
			if(cpu == task_j.get_partition())
				continue;
			Resource_Requests& rr_j =  task_j.get_requests();
			for(uint l = 0; l < rr_j.size(); l++)
			{
				Request& request_l = rr_j[l];
				uint l_id = request_l.get_resource_id();
				if(l_id == k_id)
				{
					ulong temp = request_l.get_total_length() + request_l.get_num_requests()*transitive_preemption(taskset, processorset, resourceset, j, l_id);
					sum_2 += ceiling(task_i.get_response_time(), task_j.get_response_time())*temp;
				}
			}
		}
		sum_1 += sum_2;
	}
	return sum_1;
}

ulong MLI(TaskSet& taskset, ProcessorSet& processorset, ResourceSet& resourceset, uint t_id)
{
	Task& task_i = taskset.get_task_by_id(t_id);
	Resource_Requests& rr_i =  task_i.get_requests();
	uint cpu = task_i.get_partition();
	ulong sum = 0;
	uint num_i = 1;	

	for(uint i = 0; i < rr_i.size(); i++)
	{
		if(resourceset.get_resources()[rr_i[i].get_resource_id()].is_global_resource())
			num_i++;
	}

	for(uint j = t_id + 1; j < taskset.get_taskset_size(); j++)
	{
		Task& task_j = taskset.get_task_by_id(j);
		Resource_Requests& rr_j =  task_j.get_requests();
		uint num_j = 0;
		ulong max_element = 0;
		if(cpu != task_j.get_partition())
				continue;
		for(uint i = 0; i < rr_j.size(); i++)
		{
			Request& request_k = rr_j[i];
			if(resourceset.get_resources()[request_k.get_resource_id()].is_global_resource())
			{
				num_j++;
				if(max_element < request_k.get_max_length())
					max_element = request_k.get_max_length();
			}
			
		}
		sum += min(num_i, 2*num_j)*max_element;
	}
	return sum;
}

void calculate_total_blocking(TaskSet& taskset, ProcessorSet& processorset, ResourceSet& resourceset, uint t_id)
{
//cout<<"1"<<endl;
	Task& task_i = taskset.get_task_by_id(t_id);
//		cout<<"partition3:"<<task_i.get_partition()<<endl;
//cout<<"2"<<endl;
	ulong dlb = DLB(taskset, processorset, resourceset, t_id);
//cout<<"3"<<endl;
	ulong dgb_l = DGB_l(taskset, processorset, resourceset, t_id);
//cout<<"4"<<endl;
	ulong dgb_h = DGB_h(taskset, processorset, resourceset, t_id);
//cout<<"5"<<endl;
	ulong mli = MLI(taskset, processorset, resourceset, t_id);
//cout<<"6"<<endl;
	task_i.set_total_blocking(dlb + dgb_l + dgb_h + mli);
}

ulong interf_with_suspension(Task& task, ulong interval)
{
	return task.get_wcet() * ceiling((interval + task.get_response_time()), task.get_period());
}

// RTA with self-suspension (to appear in RTS journal)
ulong rta_with_suspension(TaskSet& tasks, uint t_id, uint ITER_BLOCKING)
{
	Task& task = tasks.get_task_by_id(t_id);
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_total_blocking() + task.get_wcet();
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
				demand = task.get_total_blocking() + task.get_wcet();
				break;
		}

		ulong interference = 0;
		for (uint th = 0; th < t_id; th ++)
		{
			Task& tsk = tasks.get_task_by_id(th);
			if (task.get_partition() == tsk.get_partition())
			{
				interference += interf_with_suspension(tsk, response);
			}
		}
//cout<<"interference1:"<<interference<<endl;
		demand += interference;

		if (response == demand)
			return response + task.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

ulong interf_with_suspension2(Task& task, ulong interval)
{
	ulong wcet_h = task.get_wcet();
	ulong period_h = task.get_period();
	ulong r_h = task.get_response_time();
	ulong body = 0;
	ulong carry_in = 0;
	ulong carry_out = 0;
	if(interval > r_h)
	{
		body = ((interval - r_h)/period_h)*wcet_h;
//cout<<"body:"<<body<<endl;
	carry_in = min((interval - r_h)%period_h, wcet_h);
//cout<<"carry_in:"<<carry_in<<endl;
	}
	carry_out = wcet_h;
//cout<<"carry_out:"<<carry_out<<endl;
	
	return carry_in + body + carry_out;
}

// test
ulong rta_with_suspension2(TaskSet& tasks, uint t_id, uint ITER_BLOCKING)
{
	Task& task = tasks.get_task_by_id(t_id);
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_total_blocking() + task.get_wcet();
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
				demand = task.get_total_blocking() + task.get_wcet();
				break;
		}

		ulong interference = 0;
		for (uint th = 0; th < t_id; th ++)
		{
			Task& tsk = tasks.get_task_by_id(th);
			if (task.get_partition() == tsk.get_partition())
			{
				interference += interf_with_suspension2(tsk, response);
			}
		}
//cout<<"interference2:"<<interference<<endl;
		demand += interference;

		if (response == demand)
			return response + task.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

ulong interf_with_distributed_scheduling(Task& task, ulong interval)
{
	return task.get_wcet_non_critical_sections() * ceiling((interval + task.get_response_time()), task.get_period());
}

ulong rta_with_distributed_scheduling(TaskSet& tasks, ResourceSet& resources, uint t_id, uint ITER_BLOCKING)
{
	Task& task = tasks.get_task_by_id(t_id);
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_total_blocking() + task.get_wcet();
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
				demand = task.get_total_blocking() + task.get_wcet();
				break;
		}

		ulong interference = 0;
		for (uint th = 0; th < t_id; th ++)
		{
			Task& tsk = tasks.get_task_by_id(th);
			if (task.get_partition() == tsk.get_partition())
			{
				interference += interf_with_distributed_scheduling(tsk, response);
			}
		}
		for (uint res = 0; res < resources.get_resourceset_size(); res ++)
		{
			const Resource& resource = resources.get_resources()[res];
			if (resource.get_locality() == task.get_partition())
			{
				// add functions to bound the interference from resource agents
				// XXXXXXXX
			}
		}
		
		demand += interference;

		if (response == demand)
			return response + task.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

bool is_pfp_rta_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint TEST_TYPE, uint ITER_BLOCKING)
{
	ulong response_bound;
	for (uint t_id = 0; t_id < tasks.get_taskset_size(); t_id ++)
	{
		Task& task = tasks.get_task_by_id(t_id);
//		cout<<"partition1:"<<task.get_partition()<<endl;
		if (task.get_partition() == 0XFFFFFFFF)
			continue;
//		cout<<"partition2:"<<task.get_partition()<<endl;
		switch (TEST_TYPE)
		{
			case 0:
				response_bound = rta_standard(tasks, t_id, ITER_BLOCKING);
				break;
			case 1:
				tasks.calculate_spin(resources, processors);
				tasks.calculate_local_blocking(resources);
				response_bound = rta_with_spin(tasks, t_id, ITER_BLOCKING);
				break;
			case 2:
				calculate_total_blocking(tasks, processors, resources, t_id);
				response_bound = rta_with_suspension(tasks, t_id, ITER_BLOCKING);
				break;
			case 3:
				response_bound = rta_with_distributed_scheduling(tasks, resources, t_id, ITER_BLOCKING);
				break;
			case 8:
				calculate_total_blocking(tasks, processors, resources, t_id);
				response_bound = rta_with_suspension2(tasks, t_id, ITER_BLOCKING);
				break;

		}
		
		if (response_bound <= task.get_deadline())
			task.set_response_time(response_bound);
		else
			return false;
	}
	return true;
}
#endif
