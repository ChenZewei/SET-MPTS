#ifndef GEDF_NON_PREEMPT_H
#define GEDF_NON_PREEMPT_H

//Global EDF Non-Preemptive Scheduling
#include <iostream>
#include <assert.h>
#include "types.h"
#include <tasks.h>
#include <resources.h>
#include <processors.h>

set<ulong> delta(Task& ti, TaskSet& tasks)
{
	ulong p_i = ti.get_period();
	set<ulong> delta_set;
	foreach_higher_priority_task(tasks.get_tasks(), ti, tx)
	{
		ulong p_x = tx->get_period();
		for(uint i = 1; i <= p_i/p_x; i++)
		{
			if((i*p_x + 1) < p_i)
			{
				delta_set.insert(i*p_x + 1);
			}
		}
	}
	return delta_set;
}

set<ulong> delta_2(TaskSet& tasks)
{
	ulong p_max = tasks.get_tasks()[tasks.get_taskset_size() - 1].get_period();
	set<ulong> delta_set;
	foreach(tasks.get_tasks(), tx)
	{
		ulong p_x = tx->get_period();
		ulong sum = p_x/3;
		while(sum < (p_max + 1))
		{
			delta_set.insert(sum);
			sum += p_x;
		}
	}
	return delta_set;
}

ulong B(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint resource_id, ulong interval)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	ulong result = 0;
	foreach(tasks.get_tasks(), tl)
	{
		ulong p_l = tl->get_period();
		if(tl->is_request_exist(resource_id))
		{
			if(p_l > (3*interval))
			{
				ulong A = tl->get_wcet_critical_sections();//only one cirtical section
				ulong temp = double(3*p_num*A)/(4*p_num + 3*r_num);
				if(result < temp)
					result = temp;
			}
		}
	}
	return result;
}

ulong H(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint resource_id, ulong interval)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	ulong result = 0;
	foreach(tasks.get_tasks(), ti)
	{
		if(ti->is_request_exist(resource_id))
		{
			ulong p_i = ti->get_period();
			ulong A = ti->get_wcet_critical_sections();//only one cirtical section
			ulong temp = max(0, int((interval - double(p_i)/3)/p_i + 1));
			result += temp*(double(3*p_num*A)/(4*p_num + 3*r_num));
		}
	}
	return result;
}

ulong gedf_non_preempt_dbf(Task& ti, Task& tk, ProcessorSet& processors, ResourceSet& resources)
{
	ulong p_i = ti.get_period(), p_k = tk.get_period();
	ulong e_k = tk.get_wcet_non_critical_sections();
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
/*
if(floor(double(p_i)/(3*p_k) - (1.0/3)) > -1)
{
cout<<"test:"<<floor(double(p_i)/(3*p_k) - (1.0/3))<<endl;
cout<<"dbf_i_k:"<<(floor(double(p_i)/(3*p_k) - (1.0/3)) + 1)*(double(4*p_num + 3*r_num)*e_k)/(2*p_num)<<endl;
cout<<"p_num:"<<p_num<<endl;
cout<<"r_num:"<<r_num<<endl;
cout<<"p_i:"<<p_i<<endl;
cout<<"p_k:"<<p_k<<endl;
cout<<"e_k:"<<e_k<<endl;
}
*/
	return (floor(double(p_i)/(3*p_k) - (1.0/3)) + 1)*(double(4*p_num + 3*r_num)*e_k)/(2*p_num);
}

ulong gedf_non_preempt_psi(Task& ti, Task& tk, ProcessorSet& processors, ResourceSet& resources, ulong interval)
{
	ulong dbf_i_k = gedf_non_preempt_dbf(ti, tk, processors, resources);
//cout<<"dbf_i_k:"<<dbf_i_k<<endl;
	ulong p_i = ti.get_period();
	ulong e_k = tk.get_wcet_non_critical_sections();
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();

	ulong element1 = double((4*p_num + 3*r_num)*e_k)/(2*p_num);
	ulong element2 = interval - dbf_i_k*(double(2*p_num*p_i)/((4*p_num + 3*r_num)*e_k));
	if(element2 < 0)
		element2 = 0;
//cout<<"element1:"<<element1<<endl;

	return dbf_i_k + min(element1, element2);
}

ulong gedf_non_preempt_N(Task& tk, ProcessorSet& processors, ResourceSet& resources, ulong interval)
{
	ulong p_k = tk.get_period();
	ulong e_k = tk.get_wcet_non_critical_sections();
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();

	return (interval + double(p_k)/3 - (double((4*p_num + 3*r_num)*e_k)/(2*p_num)))/p_k;
}

ulong gedf_non_preempt_xi(Task& tk, ProcessorSet& processors, ResourceSet& resources, ulong interval)
{
	ulong p_k = tk.get_period();
	ulong e_k = tk.get_wcet_non_critical_sections();
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	ulong N_k = gedf_non_preempt_N(tk, processors, resources, interval);

	ulong element1 = (double(4*p_num + 3*r_num)*e_k)/(2*p_num);
	ulong element2 = interval + double(p_k)/3 - (double((4*p_num + 3*r_num)*e_k)/(2*p_num)) - N_k*p_k;

	return N_k*double((4*p_num + 3*r_num)*e_k)/(2*p_num) + min(element1, element2);
}

ulong gedf_non_preempt_interference(Task& ti, Task& tk, ProcessorSet& processors, ResourceSet& resources, ulong interval)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	ulong p_i = ti.get_period();
	ulong e_i = ti.get_wcet_non_critical_sections();
	ulong xi_k = gedf_non_preempt_xi(tk, processors, resources, interval);
	ulong psi_i_k = gedf_non_preempt_psi(ti, tk, processors, resources, p_i/3);
	
//cout<<"xi_k:"<<xi_k<<endl;
//cout<<"psi_i_k:"<<psi_i_k<<endl;
	ulong element = min(double(psi_i_k), interval - double((4*p_num + 3*r_num)*e_i)/(2*p_num) + 1);
	
	return min(xi_k, element);
}

ulong gedf_non_preempt_response_time(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong p_i = ti.get_period();
	ulong e = ti.get_wcet();
	ulong e_i = ti.get_wcet_non_critical_sections();
	ulong e_c_i = ti.get_wcet_critical_sections();
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
//cout<<"e:"<<e<<endl;
//cout<<"c*:"<<e_i<<endl;
//cout<<"c:"<<e_c_i<<endl;
	ulong test_start = double((4*p_num + 3*r_num)*e_i)/(2*p_num);
	double test_end = double(p_i)/3;
	ulong response_time = test_start;

	while(response_time < test_end)
	{
		test_start = double((4*p_num + 3*r_num)*e_i)/(2*p_num);

//cout<<"test_start:"<<test_start<<endl;
//cout<<"e:"<<e<<endl;
//cout<<"c*:"<<e_i<<endl;
//cout<<"c:"<<e_c_i<<endl;

		//foreach_higher_priority_task(tasks.get_tasks(), ti, tx)
		foreach_task_except(tasks.get_tasks(), ti, tx)
		{
			test_start += gedf_non_preempt_interference(ti, *tx, processors, resources, response_time)/p_num;
		}

//		if(test_start < response_time)
//			cout<<"test:"<<test_start<<" response_time:"<<response_time<<endl;
		assert(test_start >= response_time);

		if(test_start > response_time)
			response_time = test_start;
		else if(test_start == response_time)
			return response_time;
		
	}

	return response_time;
}

bool is_gedf_non_preempt_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	uint t_num = tasks.get_taskset_size();
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	
	//constraint 1
	foreach(tasks.get_tasks(), ti)
	{
		ulong response_time = gedf_non_preempt_response_time(*ti, tasks, processors, resources);
		if(response_time > double(ti->get_period())/3)
		{
//			cout<<"1 ";
			return false;
		}
	}

	//constrant 2
	foreach(resources.get_resources(), resource)
	{
		double sum = 0;
		uint q = resource->get_resource_id();
		foreach(resource->get_taskqueue(), ti_q)
		{
			//(a)
			ulong pi_q = ((Task*)(*ti_q))->get_period();
			ulong ai_q = ((Task*)(*ti_q))->get_wcet_critical_sections();
			sum += double(ai_q)/pi_q;

			//(b)
			set<ulong> delta_set = delta(*((Task*)(*ti_q)), tasks);
			foreach(delta_set, delta)
			{
				ulong L = *delta;
/*
				double test_start = double(4*p_num + 3*r_num)*ai_q/(3*p_num);

				foreach_higher_priority_task(tasks.get_tasks(), (*((Task*)(*ti_q))), tj_q)
				{
					if(!tj_q->is_request_exist(q))
						continue;
					ulong pj_q = tj_q->get_period();
					ulong aj_q = tj_q->get_wcet_critical_sections();
					test_start += ((L - 1)/pj_q)*(double(4*p_num + 3*r_num)*aj_q/(3*p_num));
				}
*/

				ulong sum = B(tasks, processors, resources, q, L) + H(tasks, processors, resources, q, L);
				
				if(L < sum)
				{
//					cout<<"2-b ";
					return false;
				}
			}
		}
		if(sum > double(4*p_num + 3*r_num)/(3*p_num))
		{
//			cout<<"2-a ";
			return false;
		}
	}
		
	return true;
}





























#endif
