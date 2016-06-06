#ifndef TASKS_H
#define TASKS_H

#include <vector>
#include <algorithm>
#include <iostream>
#include "math-helper.h"
#include "iteration-helper.h"
#include "types.h"
#include "random_gen.h"
#include "resources.h"
#include "sort.h"
#include "processors.h"

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
			ResourceSet& resourceset,
			double probability,
			int num_max,
			Range l_range,
			double tlfs,
			ulong wcet, 
			ulong period,
			ulong deadline = 0,
			uint priority = 0);
		
		void init();

		uint get_id() const;
		void set_id(uint id);
		ulong get_wcet() const;
		ulong get_deadline() const;
		ulong get_period() const;
		bool is_feasible() const;

		Resource_Requests& get_requests();
		Request& get_request_by_id(uint id);

		ulong get_wcet_critical_sections() const;
		void set_wcet_critical_sections(ulong csl);
		ulong get_wcet_non_critical_sections() const;
		void set_wcet_non_critical_sections(ulong ncsl);
		ulong get_spin() const;
		void set_spin(ulong spining);
		ulong get_local_blocking() const;
		void set_local_blocking(ulong lb);
		ulong get_total_blocking() const;
		void set_total_blocking(ulong tb);
		ulong get_self_suspension() const;
		void set_self_suspension(ulong ss);
		ulong get_jitter() const;
		void set_jitter(ulong jit);
		ulong get_response_time() const;
		void set_response_time(ulong response);
		uint get_priority() const;
		void set_priority(uint prio);
		uint get_partition() const;
		void set_partition(uint cpu);
		uint get_cluster() const;
		void set_cluster(uint clu);
		CPU_Set* get_affinity() const;
		void set_affinity(CPU_Set* affi);
		bool is_independent() const;
		void set_dependent();

		void add_request(uint res_id, uint num, ulong max_len, ulong total_len);
		
		uint get_max_num_jobs(ulong interval); //max number of jobs in an arbitrary length of interval
		ulong DBF(ulong interval);//Demand Bound Function
		void DBF();
		fraction_t get_utilization() const;
		fraction_t get_density() const;
		void get_utilization(fraction_t &utilization);
		void get_density(fraction_t &density);
};

class Job
{
	private:
		uint id;
		ulong cost;
	public:
		Job(uint id, ulong cost);
		uint get_id() const;
		ulong get_cost() const;
};

typedef vector<Job> Jobs;

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
		~TaskSet();
		
		void init();

		void calculate_spin(ResourceSet& resourceset, ProcessorSet& processorset);
		void calculate_local_blocking(ResourceSet& resourceset);
		
		void add_task(long wcet, long period, long deadline = 0);
		void add_task(ResourceSet& resourceset, double probability, int num_max, Range l_range, double tlfs, long wcet, long period, long deadline = 0);

		Tasks& get_tasks();
		Task& get_task_by_id(uint id);

		bool is_implicit_deadline();
		bool is_constraint_deadline();
		bool is_arbitary_deadline();
		uint get_taskset_size() const ;

		fraction_t get_task_utilization(uint index) const;
		fraction_t get_task_density(uint index) const;
		ulong get_task_wcet(uint index) const;
		ulong get_task_deadline(uint index) const;
		ulong get_task_period(uint index) const;
		
		fraction_t get_utilization_sum() const;
		fraction_t get_utilization_max() const;
		fraction_t get_density_sum() const;
		fraction_t get_density_max() const;
		void get_utilization_sum(fraction_t &utilization_sum) const;
		void get_utilization_max(fraction_t &utilization_max) const;
		void get_density_sum(fraction_t &density_sum) const;
		void get_density_max(fraction_t &density_max) const;

		void sort_by_period();//increase
		void sort_by_utilization();//decrease
		ulong DBF(ulong time);

		void display();



};

void tast_gen(TaskSet& taskset, ResourceSet& resourceset, int lambda, Range p_range, double utilization,double probability, int num_max, Range l_range, double tlfs);
ulong gcd(ulong a, ulong b);
ulong lcm(ulong a, ulong b);


#endif
