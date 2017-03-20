#include "rta_pfp_wf_semaphore.h"
#include "iteration-helper.h"
#include "math-helper.h"

RTA_PFP_WF_semaphore::RTA_PFP_WF_semaphore():PartitionedSched(false, RTA, FIX_PRIORITY, MPCP, "", "semaphore") {}

RTA_PFP_WF_semaphore::RTA_PFP_WF_semaphore(TaskSet tasks, ProcessorSet processors, ResourceSet resources):PartitionedSched(false, RTA, FIX_PRIORITY, MPCP, "", "semaphore")
{
	this->tasks = tasks;
	this->processors = processors;
	this->resources = resources;
	
	this->tasks.RM_Order();
	this->processors.init();
}

ulong RTA_PFP_WF_semaphore::transitive_preemption(Task& task_i, uint r_id)
{
	uint t_id = task_i.get_id();
	uint cpu = task_i.get_partition();
	if(cpu == MAX_INT)
		return 0;

	Processor& processor = processors.get_processors()[cpu];
	TaskQueue& tq = processor.get_taskqueue();
	Resource& resource = resources.get_resources()[r_id];
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
			Resource& resource_x = resources.get_resources()[request_x.get_resource_id()];
			if(ceiling_k > resource_x.get_ceiling())
				sum += request_x.get_max_length();
		}
	}
	return sum;
}

ulong RTA_PFP_WF_semaphore::DLB(Task& task_i)
{
	uint t_id = task_i.get_id();
	uint cpu = task_i.get_partition();
	Processor& processor = processors.get_processors()[cpu];
	TaskQueue& tq = processor.get_taskqueue();
	Resource_Requests& rr_i =  task_i.get_requests();
	uint num = 1;

	for(uint i = 0; i < rr_i.size(); i++)
	{
		if(resources.get_resources()[rr_i[i].get_resource_id()].is_global_resource())
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
			Resource& resource_l = resources.get_resources()[request_l.get_resource_id()];
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

ulong RTA_PFP_WF_semaphore::DGB_l(Task& task_i)
{
	uint t_id = task_i.get_id();
	Resource_Requests& rr_i =  task_i.get_requests();
	uint cpu = task_i.get_partition();

	ulong sum = 0;

	for(uint k = 0; k < rr_i.size(); k++)
	{
		uint k_id = rr_i[k].get_resource_id();
		Resource& resource_k = resources.get_resources()[k_id];
		uint N_i_k = rr_i[k].get_num_requests();
		if(!resource_k.is_global_resource())
			continue;
		for(uint j = t_id + 1; j < tasks.get_taskset_size(); j++)
		{

			ulong max_element = 0;
			Task& task_j = tasks.get_task_by_id(j);
			if(cpu == task_j.get_partition())
				continue;
			Resource_Requests& rr_j =  task_j.get_requests();

			for(uint l = 0; l < rr_j.size(); l++)
			{

				Request& request_l = rr_j[l];
				uint l_id = request_l.get_resource_id();

				if(l_id == k_id)
				{
					ulong temp = request_l.get_max_length() + transitive_preemption(task_j, l_id);
					if(temp > max_element)
						max_element = temp;
				}

			}

			sum += N_i_k*max_element;
		}
	}
	return sum;
}

ulong RTA_PFP_WF_semaphore::DGB_h(Task& task_i)
{
	uint t_id = task_i.get_id();
	Resource_Requests& rr_i =  task_i.get_requests();
	uint cpu = task_i.get_partition();
	ulong sum_1 = 0;
	
	for(uint k = 0; k < rr_i.size(); k++)
	{
		uint k_id = rr_i[k].get_resource_id();
		Resource& resource_k = resources.get_resources()[k_id];
		uint N_i_k = rr_i[k].get_num_requests();
		if(!resource_k.is_global_resource())
			continue;
		
		ulong sum_2 = 0;
		for(uint j = 0; j < t_id; j++)
		{
			Task& task_j = tasks.get_task_by_id(j);
			
			if(cpu == task_j.get_partition())
				continue;
			Resource_Requests& rr_j =  task_j.get_requests();
			for(uint l = 0; l < rr_j.size(); l++)
			{
				Request& request_l = rr_j[l];
				uint l_id = request_l.get_resource_id();
				if(l_id == k_id)
				{
					ulong temp = request_l.get_total_length() + request_l.get_num_requests()*transitive_preemption(task_j, l_id);
					sum_2 += ceiling(task_i.get_response_time(), task_j.get_response_time())*temp;
				}
			}
		}
		sum_1 += sum_2;
	}
	return sum_1;
}

ulong RTA_PFP_WF_semaphore::MLI(Task& task_i)
{
	uint t_id = task_i.get_id();
	Resource_Requests& rr_i =  task_i.get_requests();
	uint cpu = task_i.get_partition();
	ulong sum = 0;
	uint num_i = 1;	

	for(uint i = 0; i < rr_i.size(); i++)
	{
		if(resources.get_resources()[rr_i[i].get_resource_id()].is_global_resource())
			num_i++;
	}

	for(uint j = t_id + 1; j < tasks.get_taskset_size(); j++)
	{
		Task& task_j = tasks.get_task_by_id(j);
		Resource_Requests& rr_j =  task_j.get_requests();
		uint num_j = 0;
		ulong max_element = 0;
		if(cpu != task_j.get_partition())
				continue;
		for(uint i = 0; i < rr_j.size(); i++)
		{
			Request& request_k = rr_j[i];
			if(resources.get_resources()[request_k.get_resource_id()].is_global_resource())
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

void RTA_PFP_WF_semaphore::calculate_total_blocking(Task& task_i)
{
	ulong dlb = DLB(task_i);

	ulong dgb_l = DGB_l(task_i);

	ulong dgb_h = DGB_h(task_i);

	ulong mli = MLI(task_i);

	task_i.set_total_blocking(dlb + dgb_l + dgb_h + mli);
}

ulong RTA_PFP_WF_semaphore::interference(Task& task, ulong interval)
{
	return task.get_wcet() * ceiling((interval + task.get_response_time()), task.get_period());
}

ulong RTA_PFP_WF_semaphore::response_time(Task& ti)
{
	//uint t_id = ti.get_id();
	ulong test_end = ti.get_deadline();
	ulong test_start = ti.get_total_blocking() + ti.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	while (response <= test_end)
	{
		demand = test_start;

		ulong total_interference = 0;

		foreach_higher_priority_task(tasks.get_tasks(), ti, th)
		{
			if (th->get_partition() == ti.get_partition())
			{
				total_interference += interference((*th), response);
			}
		}
		
		demand += total_interference;

		if (response == demand)
			return response + ti.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

bool RTA_PFP_WF_semaphore::alloc_schedulable()
{
	ulong response_bound;

	for (uint t_id = 0; t_id < tasks.get_taskset_size(); t_id ++)
	foreach(tasks.get_tasks(), ti)
	{

		if (ti->get_partition() == 0XFFFFFFFF)
			continue;

		calculate_total_blocking((*ti));

		response_bound = response_time((*ti));
		
		if (response_bound <= ti->get_deadline())
			ti->set_response_time(response_bound);
		else
			return false;
	}
	return true;
}

bool RTA_PFP_WF_semaphore::is_schedulable()
{
	foreach(tasks.get_tasks(), ti)
	{
		if(!BinPacking_WF((*ti), tasks, processors, resources, TEST))
			return false;
	}
	return true;
}
