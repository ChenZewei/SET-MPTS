#ifndef DAG_GEDF_H
#define DAG_GEDF_H

#include <math.h>
#include <math-helper.h>
#include <algorithm>
#include <iostream>

#include "../tasks.h"
#include "../processors.h"
#include "../resources.h"

typedef struct
{
	ulong time;
	ulong workload;
}Work;

typedef struct
{
	ulong r_t;
	ulong f_t;
}Exec_Range;

set<Work> precise_workload(DAG_Task &dag_task, ulong T)
{
	ulong deadline = dag_task.get_deadline();
	ulong period = dag_task.get_period();
	ulong vol = dag_task.get_vol();
	if(T < deadline)
		return 0;
	ulong current_t = T;
	vector<ulong> f_times;
	vector<Exec_Range> jobs;
	VNode vnode;
	for(uint i = 0; i < dag_task.get_vnode_num(); i++)
	{
		jobs.push_back(0);
	}

	for(uint i = 0; i < dag_task.get_vnode_num(); i++)
	{
		vnode = dag_task.get_vnode_by_id(i);
		ulong latest_f_time = 0;
		for(uint j = 0; j < vnode.precedences.size(); j++)
		{
			uint prenode = vnode.precedences[j]->tail;
			if(latest_f_time < jobs[prenode])
				latest_f_time = jobs[prenode];
		}
		jobs[i].r_t = latest_f_time;
		jobs[i].f_t = latest_f_time + vnode.wcet;
	}

	set<ulong> time_set;
	time_set.insert(0);
	for(uint i = 0; i < jobs.size(); i ++)
	{
		time_set.insert(jobs[i].f_t);
	}
	time_set.insert(deadline);

	sort(time_set.begin(), time_set.end());
	set<Work> workload_set;
	Work work;
	work.time = 0;
	work.workload = 0;
	workload_set.insert(work);
	
	for(uint i = 1; i < time_set.size(); i++)
	{
		ulong t = time_set[i];
		ulong exec_t = t - time_set[i - 1];
		ulong workload = 0;
		for(uint j = 0; j < jobs.size(); j++)
		{
			if((jobs[j].r_t < t) && (jobs[j].f_t <= t))
			{
				workload += exec;
			}
		}
		work.time = t;
		work.workload = workload;
		workload_set.insert(work);
	}

	uint num = T/period;
	uint l = T%period;
	ulong release;
	set<Work> workload_set_total;
	work.time = 0;
	work.workload = 0;
	workload_set_total.insert(work);
	if(l >= deadline)
	{
		num++;
		release = l - deadline;
		for(uint i = 0; i < num; i++)
		{
			for(uint j = 0; j < workload_set.size(); j++)
			{
				work.time = workload_set[j].time + release + i * period;
				work.workload = workload_set[j].workload + i * vol;
				workload_set_total.insert(work);
			}
		}
		cout<<"T:"<<T<<"Last deadline:"<<workload_set_total[workload_set_total.size() - 1].time<<endl;
	}
	else
	{
		ulong cutting = deadline - l;
		release = period - deadline + l;
		uint begin;
		ulong cutting_workload;
		ulong cutting_time;
		for(uint i = 0; i < workload_set.size() - 1; i++)
		{
			if((workload_set[i].time < cutting) && (workload_set[i + 1].time >= cutting))
			{
				begin = i + 1;
				cutting_workload;
				cutting_time = cutting - workload_set[i].time;
				cutting_workload = (cutting_time/(workload_set[i + 1].time - workload_set[i].time))*(workload_set[i + 1].workload - workload_set[i].workload);
				break;
			}
		}
		for(uint i = begin; i < workload_set.size(); i++)
		{
			work.time = workload_set[i].time - cutting;
			work.workload = workload_set[i].workload - cutting_workload;
			workload_set_total.insert(work);
		}

		ulong carry_in_workload = workload_set_total[workload_set_total.size() - 1].workload;
		
		for(uint i = 0; i < num; i++)
		{
			for(uint j = 0; j < workload_set.size(); j++)
			{
				work.time = workload_set[j].time + release + i * period;
				work.workload = workload_set[j].workload + carry_in_workload + i * vol;
				workload_set_total.insert(work);
			}
		}
		cout<<"T:"<<T<<"Last deadline:"<<workload_set_total[workload_set_total.size() - 1].time<<endl;
	}
	
	return workload_set_total;
}

ulong approximate_workload(DAG_Task &dag_task, ulong T, double e)
{
	
}

#endif
