#ifndef RTA_GPF_H
#define RTA_GPF_H

#include <math.h>
#include <math-helper.h>
#include <algorithm>
#include <iostream>

#include "../tasks.h"
#include "../processors.h"
#include "../resources.h"

using namespace std;

ulong native_workload(Task& task, ulong interval)
{
	return task.get_wcet() * ceiling(interval, task.get_period()) + task.get_wcet();
}

ulong native_gfp_rta(TaskSet& tasks, ProcessorSet& processors, uint t_id)
{
	Task& task = tasks.get_task_by_id(t_id);
	ulong wcet = task.get_wcet();
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	uint p_num = processors.get_processor_num();
	while(response < test_end)
	{
		ulong sum = 0;
		for(uint i = 0; i < t_id; i++)
		{
			Task& task_i = tasks.get_task_by_id(i);
			sum += native_workload(task_i, response);
		}
		demand = sum/p_num + wcet;
		if(response == demand)
			return 	response;
		else
			response = demand;
	}
	return test_end + 100;
}

ulong bc_workload(Task& task, ulong interval)
{
	ulong a = min(task.get_wcet(), (interval + task.get_response_time() - task.get_wcet()%task.get_period()));
	return task.get_wcet() * floor((interval + task.get_response_time() - task.get_wcet())/task.get_period()) + a;
}

ulong bc_interference(Task& task_k, Task& task_i, ulong interval)
{
	return min(interval - task_k.get_wcet() + 1, max((ulong)0, bc_workload(task_i, interval)));
}

ulong bc_gfp_rta(TaskSet& tasks, ProcessorSet& processors, uint t_id)
{
	Task& task = tasks.get_task_by_id(t_id);
	ulong wcet = task.get_wcet();
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	uint p_num = processors.get_processor_num();
	while(response < test_end)
	{
		ulong sum = 0;
		for(uint i = 0; i < t_id; i++)
		{
			Task& task_i = tasks.get_task_by_id(i);
			sum += bc_interference(task, task_i, response);
		}
		demand = floor(sum/p_num) + wcet;
		if(response == demand)
			return 	response;
		else
			response = demand;
	}
	return test_end + 100;
}

typedef struct
{
	uint id;
	ulong gap;
}Carry_in;

int interference_gap_decrease(Carry_in t1, Carry_in t2)
{
	return t1.gap > t2.gap;
}

ulong gn_gfp_workload_nc(Task& task, long interval)
{
	long C_i = task.get_wcet();
	long T_i = task.get_period();
	return floor(interval/T_i)*C_i + min(C_i, interval%T_i);
}

ulong gn_gfp_workload_ci(Task& task, long interval)
{
	long C_i = task.get_wcet();
	long T_i = task.get_period();
	long R_i = task.get_response_time();
	long zero = 0;
	ulong a = min(C_i - 1, max(zero, max(zero, ((interval - C_i)))%T_i - (T_i - R_i)));
	return floor(max(zero, interval - C_i)/T_i)*C_i + C_i + a;
}

ulong gn_gfp_interference_nc(Task& task_k, Task& task_i, ulong interval)
{
	return min(interval - task_k.get_wcet() + 1, max((ulong)0, gn_gfp_workload_nc(task_i,interval)));
}

ulong gn_gfp_interference_ci(Task& task_k, Task& task_i, ulong interval)
{
	return min(interval - task_k.get_wcet() + 1, max((ulong)0, gn_gfp_workload_ci(task_i,interval)));
}

ulong gn_gfp_omega(TaskSet& tasks, uint t_id, ProcessorSet& processors, ulong interval)
{
	Task& task_k = tasks.get_task_by_id(t_id);
	ulong I_nc_sum = 0, I_ci_sum = 0;
	uint k = t_id;
	uint p_num = processors.get_processor_num();
	uint ci_num;
	vector<Carry_in> carry_in;
	
	for(uint i = 0; i < t_id; i++)
	{
		Task& task_i = tasks.get_task_by_id(i);
		Carry_in temp;
		temp.id = i;
		temp.gap = gn_gfp_interference_ci(task_k, task_i, interval) - gn_gfp_interference_nc(task_k, task_i, interval);
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

	for(uint i = 0; i < t_id; i++)
	{
		Task& task_i = tasks.get_task_by_id(i);
		
		if(task_i.is_carry_in())
		{
			I_ci_sum += gn_gfp_interference_ci(task_k, task_i, interval);
//			cout<<"carry_in id:"<<task_i.get_id()<<endl;
		}		
		else
			I_nc_sum += gn_gfp_interference_nc(task_k, task_i, interval);
	}
	for(uint i = 0; i < ci_num; i++)
	{
		uint id = carry_in[i].id;
		Task& task = tasks.get_task_by_id(id);
		task.clear_carry_in();
	}
	return I_nc_sum + I_ci_sum;
}

ulong gn_gfp_rta(TaskSet& tasks, ProcessorSet& processors, uint t_id)
{
	Task& task = tasks.get_task_by_id(t_id);
	ulong wcet = task.get_wcet();
	ulong test_end = task.get_deadline();
	ulong test_start = task.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	ulong omega = 0;
	uint p_num = processors.get_processor_num();
//cout<<"id:"<<t_id<<endl;
//tasks.display();
	while(response < test_end)
	{
		omega = gn_gfp_omega(tasks, t_id, processors, response);
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

bool is_gfp_rta_schedulable(TaskSet& tasks, ProcessorSet& processors, uint TEST_TYPE)
{
	ulong response_bound;
//	bool update = true;
//	while(update)
//	{
//	update = false;
	for (uint t_id = 0; t_id < tasks.get_taskset_size(); t_id ++)
	{
		Task& task = tasks.get_task_by_id(t_id);
		ulong original_bound = task.get_response_time();
		switch (TEST_TYPE)
		{
			case 0://native-rta
				response_bound = native_gfp_rta(tasks, processors, t_id);
				//cout<<"standard"<<endl;
				break;
			case 1://bc-rta
				response_bound = bc_gfp_rta(tasks, processors, t_id);
				//cout<<"spin"<<endl;
				break;
			case 2://gn-rta
				response_bound = gn_gfp_rta(tasks, processors, t_id);
				break;
			default://native-rta
				response_bound = native_gfp_rta(tasks, processors, t_id);
				break;

		}
	
		if (response_bound > task.get_deadline())
			return false;
		if(response_bound > original_bound)
		{
			task.set_response_time(response_bound);
//			update = true;
		}
	}
//	}
	return true;
}

#endif
