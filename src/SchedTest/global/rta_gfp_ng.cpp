#include "rta_gfp_ng.h"

RTA_GFP_NG::RTA_GFP_NG(): GlobalSched(false, RTA, GLOBAL, FIX_PRIORITY, NONE, "", "NG") {}

RTA_GFP_NG::RTA_GFP_NG(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources): GlobalSched(false, RTA, GLOBAL, FIX_PRIORITY, NONE, "", "NG")
{
	this->tasks = tasks;
	this->processors = processors;
	this->resources = resources;
	
	this->tasks.RM_Order();
}

int RTA_GFP_NG::interference_gap_decrease(Carry_in t1, Carry_in t2)
{
	return t1.gap > t2.gap;
}

ulong RTA_GFP_NG::workload_nc(Task& task, long interval)
{
	long C_i = task.get_wcet();
	long T_i = task.get_period();
	return floor(interval/T_i)*C_i + min(C_i, interval%T_i);
}

ulong RTA_GFP_NG::workload_ci(Task& task, long interval)
{
	long C_i = task.get_wcet();
	long T_i = task.get_period();
	long R_i = task.get_response_time();
	long zero = 0;
	ulong a = min(C_i - 1, max(zero, max(zero, ((interval - C_i)))%T_i - (T_i - R_i)));
	return floor(max(zero, interval - C_i)/T_i)*C_i + C_i + a;
}

ulong RTA_GFP_NG::interference_nc(Task& task_k, Task& task_i, ulong interval)
{
	return min(interval - task_k.get_wcet() + 1, max((ulong)0, workload_nc(task_i,interval)));
}

ulong RTA_GFP_NG::interference_ci(Task& task_k, Task& task_i, ulong interval)
{
	return min(interval - task_k.get_wcet() + 1, max((ulong)0, workload_ci(task_i,interval)));
}

ulong RTA_GFP_NG::total_interference(Task& task_k, ulong interval)
{
	//Task& task_k = tasks.get_task_by_id(t_id);
	ulong I_nc_sum = 0, I_ci_sum = 0;
	uint k = task_k.get_id();
	uint p_num = processors.get_processor_num();
	uint ci_num;
	vector<Carry_in> carry_in;
	
	for(uint i = 0; i < k; i++)
	{
		Task& task_i = tasks.get_task_by_id(i);
		Carry_in temp;
		temp.id = i;
		temp.gap = interference_ci(task_k, task_i, interval) - interference_nc(task_k, task_i, interval);
		carry_in.push_back(temp);
	}

	sort(carry_in.begin(), carry_in.end(), interference_gap_decrease);

	if(0 == k)
		ci_num = 0;
	else if(k < p_num)
	{
		ci_num = k - 1;
	}
	else
	{
		ci_num = p_num - 1;
	}

	for(uint i = 0; i < ci_num; i++)
	{
		uint id = carry_in[i].id;

		Task& task = tasks.get_task_by_id(id);
		task.set_carry_in();
	}

	for(uint i = 0; i < k; i++)
	{
		Task& task_i = tasks.get_task_by_id(i);
		
		if(task_i.is_carry_in())
		{
			I_ci_sum += interference_ci(task_k, task_i, interval);
//			cout<<"carry_in id:"<<task_i.get_id()<<endl;
		}		
		else
			I_nc_sum += interference_nc(task_k, task_i, interval);
	}
	for(uint i = 0; i < ci_num; i++)
	{
		uint id = carry_in[i].id;
		Task& task = tasks.get_task_by_id(id);
		task.clear_carry_in();
	}
	return I_nc_sum + I_ci_sum;
}
ulong RTA_GFP_NG::response_time(Task& ti)
{
	uint t_id = ti.get_id();
	ulong wcet = ti.get_wcet();
	ulong test_end = ti.get_deadline();
	ulong test_start = ti.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	ulong omega = 0;
	uint p_num = processors.get_processor_num();
//cout<<"id:"<<t_id<<endl;
//tasks.display();
	while(response < test_end)
	{
		omega = total_interference(ti, response);
//cout<<"omega:"<<omega<<endl; 
		demand = floor(omega/p_num) + wcet;
//cout<<response<<" "<<demand<<endl;
		if(response == demand)
			return 	response;
		else
			response = demand;
	}
	return test_end + 100;
}

bool RTA_GFP_NG::is_schedulable()
{
	ulong response_bound;

	//for (uint i = 0; i < tasks.get_taskset_size(); i ++)
	foreach(tasks.get_tasks(), ti)
	{
		//Task& ti = tasks.get_task_by_id(i);
		ulong original_bound = ti->get_response_time();
		response_bound = response_time((*ti));
		if (response_bound > ti->get_deadline())
			return false;
		if(response_bound > original_bound)
		{
			ti->set_response_time(response_bound);
		}
	}
	return true;
}




