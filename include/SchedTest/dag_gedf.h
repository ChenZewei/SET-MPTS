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
	ulong work;
}Instant;

vector<ulong> get_instants(DAG_Task &dag_task, ulong T)
{
	if(T < dag_task.get_deadline())
		return 0;
	ulong current_t = T;
	vector<ulong> f_times;
	for(uint i = 0; i < dag_task.get_vnode_num(); i++)
	{
		f_times.push_back(0);
	}

	for(uint i = 0; i < dag_task.get_vnode_num(); i++)
	{
		VNode vnode = dag_task.get_vnode_by_id(i);
		ulong latest_f_time = 0;
		for(uint j = 0; j < vnode.precedences.size(); j++)
		{
			uint prenode = vnode.precedences[j]->tail;
			if(latest_f_time < f_times[prenode])
				latest_f_time = f_times[prenode];
		}
		f_times[i] = latest_f_time + vnode.wcet;
	}

	ulong gap = dag_task.get_deadline() - f_times[f_times.size() - 1];
	ulong release_t;
	vector<ulong> times;
	while(current_t > 0)
	{
		release_t = current_t - dag_task.get_deadline();
		for(uint i = 0; i < f_times.size(); i++)
		{
			ulong temp = release_t + f_times[i] + gap;
			if(temp > 0)
				times.push_back(temp);
		}
		current_t -= dag_task.get_period();
	}
	sort(times.begin(), times.end());
	return times;
}

ulong precise_workload(DAG_Task &dag_task, ulong T)
{
	if(T < dag_task.get_deadline())
		return 0;
	ulong current_t = 0, current_w = dag_task.get_vol()*((T - dag_task.get_deadline())/dag_task.get_period()), current_j = 0;
	T = (T - dag_task.get_deadline())%dag_task.get_period();
	vector<ulong> f_time;
	for(uint i = 0; i < dag_task.get_vnode_num(); i++)
	{
		f_time.push_back(0);
	}
	for(uint i = 0; i < dag_task.get_vnode_num(); i++)
	{
		VNode vnode = dag_task.get_vnode_by_id(i);
		ulong latest_f_time = 0;
		for(uint j = 0; j < vnode.precedences.size(); j++)
		{
			uint prenode = vnode.precedences[j]->tail;
			if(latest_f_time < f_time[prenode])
				latest_f_time = f_time[prenode];
		}
		f_time[i] = latest_f_time + vnode.wcet;
		if(f_time[i] <= T)
		{
			if(f_time[i] > current_t)
				current_t = f_time[i];
			current_w += vnode.wcet;
		}
	}
}

ulong approximate_workload(DAG_Task &dag_task, ulong T, double e)
{
	
}

#endif
