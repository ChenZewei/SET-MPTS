#ifndef TASKS_H
#define TASKS_H

#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include "types.h"
using namespace std;

typedef vector<fraction_t> Ratio;

class Task
{
	private:
		uint id;
		uint wcet;
		uint wcet_critical_sections
		uint wcet_non_critical_sections
		uint spin
		uint self_suspension
		uint local_blocking
		uint total_blocking
		uint jitter
		uint deadline;
		uint period;
		uint priority;
		fraction_t utilization;
		fraction_t density;
		Ratio ratio;//for heterogeneous platform
	public:
		Task(uint id,
			uint wcet, 
			uint period,
			uint deadline = 0,
			uint priority = 0);
		
		uint get_id()
		{
			return id;
		}
		uint get_wcet()
		{
			return wcet;
		}
		uint get_wcet_critical_sections()
		{
			return wcet_critical_sections;
		}
		uint get_wcet_non_critical_sections()
		{
			return wcet_non_critical_sections;
		}
		uint get_spin()
		{
			return spin;
		}
		uint get_local_blocking()
		{
			return local_blocking;
		}
		uint get_total_blocking()
		{
			return total_blocking;
		}
		uint get_self_suspension()
		{
			return self_suspension;
		}
		uint get_jitter()
		{
			return jitter;
		}
		uint get_deadline()
		{
			return deadline;
		}
		uint get_period()
		{
			return period;
		}
		bool is_feasible()
		{
			return deadline >= wcet && period >= wcet && wcet > 0;
		}
		
		uint DBF(uint time);//Demand Bound Function
		void DBF();
		fraction_t get_utilization();
		fraction_t get_density();
		void get_utilization(fraction_t &utilization);
		void get_density(fraction_t &density);
	
};

typedef vector<uint> Jobs;//wcet

struct Edge//job m can be excuted only if job v is completed
{
	uint v;
	uint m;
};

typedef vector<struct Edge> Edges;

typedef struct
{
	Jobs jobs;
	Edges edges;
}Graph;


class DAG_Task:public Task
{
	private:
		Graph graph;
		uint vol;//total wcet of the jobs in graph
	public:
		void update_vol();
		bool is_acyclic();
		uint DBF(uint time);//Demand Bound Function
		void DBF();
		fraction_t get_utilization();
		fraction_t get_density();
		void get_utilization(fraction_t &utilization);
		void get_density(fraction_t &density);	
};

typedef vector<Task> Tasks;

#define foreach(tasks, condition) 		\
	for(int i; i < tasks.size(); i++)	\
	{					\
		if(condition)			\
			return false;		\
	}

class TaskSet
{
	private:
		Tasks tasks;
		
		fraction_t utilization_sum;
		fraction_t utilization_max;
		fraction_t density_sum;
		fraction_t density_max;
	public:
		TaskSet();
		~TaskSet()
		{
			tasks.clear();
		}
		void add_task(uint wcet, uint period, uint deadline = 0)
		{
			fraction_t utilization_new = wcet, density_new = wcet;
			utilization_new /= period;
			if(0 == deadline)
				density_new /= period;
			else
				density_new /= min(deadline, period);
			tasks.push_back(Task(tasks.size(), wcet, period, deadline));
			utilization_sum += utilization_new;
			density_sum += density_new;
			if(utilization_max < utilization_new)
				utilization_max = utilization_new;
			if(density_max < density_new)
				density_max = density_new;
		}
		bool is_implicit_deadline()
		{
			foreach(tasks,tasks[i].get_deadline() != tasks[i].get_period());
			return true;
		}
		bool is_constraint_deadline()
		{
			foreach(tasks,tasks[i].get_deadline() > tasks[i].get_period());
			return true;
		}
		bool is_arbitary_deadline()
		{
			return !(is_implicit_deadline())&&!(is_constraint_deadline());
		}
		uint get_taskset_size()
		{
			return tasks.size();
		}

		fraction_t get_task_utilization(uint index)
		{
			return tasks[index].get_utilization();
		}
		fraction_t get_task_density(uint index)
		{
			return tasks[index].get_density();
		}
		uint get_task_wcet(uint index)
		{
			return tasks[index].get_wcet();
		}
		uint get_task_deadline(uint index)
		{
			return tasks[index].get_deadline();
		}
		uint get_task_period(uint index)
		{
			return tasks[index].get_period();
		}
		
		fraction_t get_utilization_sum();
		fraction_t get_utilization_max();
		fraction_t get_density_sum();
		fraction_t get_density_max();
		void get_utilization_sum(fraction_t &utilization_sum);
		void get_utilization_max(fraction_t &utilization_max);
		void get_density_sum(fraction_t &density_sum);
		void get_density_max(fraction_t &density_max);

		uint DBF(uint time);
};



#endif
