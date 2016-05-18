#ifndef TASKS_H
#define TASKS_H

#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include "math-helper.h"
#include "iteration-helper.h"
#include "types.h"
#include "random_gen.h"
#include "resources.h"
#include "sort.h"

using namespace std;

typedef vector<fraction_t> Ratio;


class Request
{
	private:
		uint resource_id;
		uint num_requests;
		ulong max_length;
		ulong total_length;

	public:
		Request(uint resource_id,
			uint num_requests,
			ulong max_length,
			ulong total_length);

		uint get_resource_id() const;
		uint get_num_requests() const;
		ulong get_max_length() const;
		ulong get_total_length() const;
};

typedef vector<Request> Resource_Requests;
typedef vector<uint> CPU_Set;

class Task
{
	private:
		uint id;
		ulong wcet;
		ulong wcet_critical_sections;
		ulong wcet_non_critical_sections;
		ulong spin;
		ulong self_suspension;
		ulong local_blocking;
		ulong total_blocking;
		ulong jitter;
		ulong response_time;//WCET
		ulong deadline;
		ulong period;
		uint priority;
		uint partition;//0XFFFFFFFF
		uint cluster;
		CPU_Set* affinity;
		bool independent;
		fraction_t utilization;
		fraction_t density;
		Ratio ratio;//for heterogeneous platform
		Resource_Requests requests; 
	public:
		Task(	uint id,
			ulong wcet, 
			ulong period,
			ulong deadline = 0,
			uint priority = 0);

		Task(	uint id,
			ResourceSet *resourceset,
			double probability,
			int num_max,
			Range l_range,
			double tlfs,
			ulong wcet, 
			ulong period,
			ulong deadline = 0,
			uint priority = 0);
		
		uint get_id() const { return id; }
		void set_id(uint id) { this->id = id; };
		ulong get_wcet() const	{ return wcet; }
		ulong get_deadline() const { return deadline; }
		ulong get_period() const { return period; }
		const Resource_Requests& get_requests() const {	return requests; }
		bool is_feasible() const { return deadline >= wcet && period >= wcet && wcet > 0; }	

		ulong get_wcet_critical_sections() const { return wcet_critical_sections; }
		void set_wcet_critical_sections(ulong csl) { wcet_critical_sections = csl; }
		ulong get_wcet_non_critical_sections() const {	return wcet_non_critical_sections; }
		void set_wcet_non_critical_sections(ulong ncsl) { wcet_non_critical_sections = ncsl; }
		ulong get_spin() const	{ return spin; }
		void set_spin(ulong spining) { spin = spining; }
		ulong get_local_blocking() const { return local_blocking; }
		void set_local_blocking(ulong lb) { local_blocking = lb; }
		ulong get_total_blocking() const { return total_blocking; }
		void set_total_blocking(ulong tb) { total_blocking = tb; }
		ulong get_self_suspension() const { return self_suspension; }
		void set_self_suspension(ulong ss) { self_suspension = ss; }
		ulong get_jitter() const { return jitter; }
		void set_jitter(ulong jit) { jitter = jit; }
		ulong get_response_time() const { return response_time; }
		void set_response_time(ulong response) { response_time = response; }
		uint get_priority() const { return priority; }
		void set_priority(uint prio) { priority = prio; }
		uint get_partition() const { return partition; }
		void set_partition(uint cpu) { partition = cpu; }
		uint get_cluster() const { return cluster; }
		void set_cluster(uint clu) { cluster = clu; }
		CPU_Set* get_affinity() const { return affinity; }
		void set_affinity(CPU_Set* affi) { affinity = affi; }
		bool is_independent() const { return independent; }
		void set_dependent() { independent = false; }

		void add_request(uint res_id, uint num, ulong max_len, ulong total_len);
		
		uint get_max_num_jobs(ulong interval); //max number of jobs in an arbitrary length of interval
		ulong DBF(ulong interval);//Demand Bound Function
		void DBF();
		fraction_t get_utilization() const;
		fraction_t get_density() const;
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
		void add_task(long wcet, long period, long deadline = 0);

		void add_task(ResourceSet *resourceset, double probability, int num_max, Range l_range, double tlfs, long wcet, long period, long deadline = 0);

		const Tasks& get_tasks() const
		{
			return tasks;
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
		uint get_taskset_size() const 
		{
			return tasks.size();
		}

		fraction_t get_task_utilization(uint index) const
		{
			return tasks[index].get_utilization();
		}
		fraction_t get_task_density(uint index) const
		{
			return tasks[index].get_density();
		}
		ulong get_task_wcet(uint index) const
		{
			return tasks[index].get_wcet();
		}
		ulong get_task_deadline(uint index) const
		{
			return tasks[index].get_deadline();
		}
		ulong get_task_period(uint index) const
		{
			return tasks[index].get_period();
		}
		
		fraction_t get_utilization_sum() const;
		fraction_t get_utilization_max() const;
		fraction_t get_density_sum() const;
		fraction_t get_density_max() const;
		void get_utilization_sum(fraction_t &utilization_sum) const;
		void get_utilization_max(fraction_t &utilization_max) const;
		void get_density_sum(fraction_t &density_sum) const;
		void get_density_max(fraction_t &density_max) const;

		void sort_by_period();
		ulong DBF(ulong time);

		void display()
		{
			for(int i = 0; i < tasks.size(); i++)
			{
				cout<<"Task id:"<<tasks[i].get_id()<<" Task period:"<<tasks[i].get_period()<<endl;
			}
		}



};

void tast_gen(TaskSet *taskset, ResourceSet* resourceset, int lambda, Range p_range, double utilization,double probability, int num_max, Range l_range, double tlfs);
ulong gcd(ulong a, ulong b);
ulong lcm(ulong a, ulong b);


#endif
