#include "tasks.h"

////////////////////////////Request//////////////////////////////

Request::Request(uint resource_id,
	uint num_requests,
	ulong max_length,
	ulong total_length)
{
	this->resource_id = resource_id;
	this->num_requests = num_requests;
	this->max_length = max_length;
	this->total_length = total_length;
}

uint Request::get_resource_id() const { return resource_id; }
uint Request::get_num_requests() const { return num_requests; }
ulong Request::get_max_length() const { return max_length; }
ulong Request::get_total_length() const { return total_length; }

////////////////////////////Task//////////////////////////////

Task::Task(uint id,
	ulong wcet, 
   	ulong period,
   	ulong deadline,
    	uint priority)
{
	this->id = id;
	this->wcet = wcet;
	if(0 == deadline)
		this->deadline = period;
	else
		this->deadline = deadline;
	this->period = period;
	this->priority = priority;
	utilization = this->wcet;
	utilization /= this->period;
	density = this->wcet;
	if (this->deadline <= this->period)
		density /= this->deadline;
	else
		density /= this->period;
	partition = 0XFFFFFFFF;
}

Task::Task(	uint id,
		ResourceSet* resourceset,
		double probability,
		int num_max,
		Range l_range,
		double tlfs,
		ulong wcet, 
		ulong period,
		ulong deadline,
		uint priority)
{
	this->id = id;
	this->wcet = wcet;
	if(0 == deadline)
		this->deadline = period;
	else
		this->deadline = deadline;
	this->period = period;
	this->priority = priority;
	utilization = this->wcet;
	utilization /= this->period;
	density = this->wcet;
	if (this->deadline <= this->period)
		density /= this->deadline;
	else
		density /= this->period;
	partition = MAX_INT;
	spin = 0;
	self_suspension = 0;
	local_blocking = 0;
	total_blocking = 0;
	jitter = 0;
	response_time = 0;
	cluster = MAX_INT;
	independent = true;
	wcet_non_critical_sections = this->wcet;
	wcet_critical_sections = 0;

	
	//Random_Gen r;
	for(int i = 0; i < resourceset->size(); i++)
	{
		if(Random_Gen::probability(probability))
		{
			uint num = Random_Gen::uniform_integral_gen(1, num_max);
			uint max_len = Random_Gen::uniform_integral_gen(l_range.min, l_range.max);
			add_request(i, num, max_len, tlfs*max_len);
			resourceset->add_task(i, id);
		}
	}
}

void Task::add_request(uint res_id, uint num, ulong max_len, ulong total_len)
{
	requests.push_back(Request(res_id, num, max_len, total_len));
}

ulong Task::DBF(ulong time)
{
	if(time >= deadline)
		return ((time - deadline)/period+1)*wcet;
	else 
		return 0;
}

uint Task::get_max_num_jobs(ulong interval)
{
	uint num_jobs;
	num_jobs = ceiling(interval + get_response_time(), get_period());
	return num_jobs;
}

fraction_t Task::get_utilization() const
{
	return utilization;
}

fraction_t Task::get_density() const
{
	return density;
}

void Task::get_utilization(fraction_t &utilization)
{
	utilization = wcet;
	utilization /= period;
}

void Task::get_density(fraction_t &density)
{
	density = wcet;
	density /= std::min(deadline,period);
}

uint Task::get_id() const { return id; }
void Task::set_id(uint id) { this->id = id; };
ulong Task::get_wcet() const	{ return wcet; }
ulong Task::get_deadline() const { return deadline; }
ulong Task::get_period() const { return period; }
const Resource_Requests& Task::get_requests() const {	return requests; }
bool Task::is_feasible() const { return deadline >= wcet && period >= wcet && wcet > 0; }	

ulong Task::get_wcet_critical_sections() const { return wcet_critical_sections; }
void Task::set_wcet_critical_sections(ulong csl) { wcet_critical_sections = csl; }
ulong Task::get_wcet_non_critical_sections() const {	return wcet_non_critical_sections; }
void Task::set_wcet_non_critical_sections(ulong ncsl) { wcet_non_critical_sections = ncsl; }
ulong Task::get_spin() const	{ return spin; }
void Task::set_spin(ulong spining) { spin = spining; }
ulong Task::get_local_blocking() const { return local_blocking; }
void Task::set_local_blocking(ulong lb) { local_blocking = lb; }
ulong Task::get_total_blocking() const { return total_blocking; }
void Task::set_total_blocking(ulong tb) { total_blocking = tb; }
ulong Task::get_self_suspension() const { return self_suspension; }
void Task::set_self_suspension(ulong ss) { self_suspension = ss; }
ulong Task::get_jitter() const { return jitter; }
void Task::set_jitter(ulong jit) { jitter = jit; }
ulong Task::get_response_time() const { return response_time; }
void Task::set_response_time(ulong response) { response_time = response; }
uint Task::get_priority() const { return priority; }
void Task::set_priority(uint prio) { priority = prio; }
uint Task::get_partition() const { return partition; }
void Task::set_partition(uint cpu) { partition = cpu; }
uint Task::get_cluster() const { return cluster; }
void Task::set_cluster(uint clu) { cluster = clu; }
CPU_Set* Task::get_affinity() const { return affinity; }
void Task::set_affinity(CPU_Set* affi) { affinity = affi; }
bool Task::is_independent() const { return independent; }
void Task::set_dependent() { independent = false; }

/////////////////////////////TaskSet///////////////////////////////

TaskSet::TaskSet()
{
	utilization_sum = 0;
	utilization_max = 0;
	density_sum = 0;
	density_max = 0;
}

TaskSet::~TaskSet()
{
	tasks.clear();
}

fraction_t TaskSet::get_utilization_sum() const
{
	return utilization_sum;
}

fraction_t TaskSet::get_utilization_max() const
{
	return utilization_max;
}

fraction_t TaskSet::get_density_sum() const
{
	return density_sum;
}

fraction_t TaskSet::get_density_max() const
{
	return density_max;
}

void TaskSet::add_task(long wcet, long period, long deadline)
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

void TaskSet::add_task(ResourceSet* resourceset, double probability, int num_max, Range l_range, double tlfs, long wcet, long period, long deadline)
{
	fraction_t utilization_new = wcet, density_new = wcet;
	utilization_new /= period;
	if(0 == deadline)
		density_new /= period;
	else
		density_new /= min(deadline, period);
	tasks.push_back(Task(tasks.size(),
			resourceset,
			probability,
			num_max,
			l_range,
			tlfs,
			wcet, 
			period,
			deadline));
	utilization_sum += utilization_new;
	density_sum += density_new;
	if(utilization_max < utilization_new)
		utilization_max = utilization_new;
	if(density_max < density_new)
		density_max = density_new;
}

void TaskSet::get_utilization_sum(fraction_t &utilization_sum) const
{
	fraction_t temp;
	utilization_sum = 0;
	for(int i = 0; i < tasks.size(); i++)
	{
		temp = tasks[i].get_wcet();
		temp /= tasks[i].get_period();
		utilization_sum += temp;
	}
}
void TaskSet::get_utilization_max(fraction_t &utilization_max) const
{
	utilization_max = tasks[0].get_utilization();
	for(int i = 1; i < tasks.size(); i++)
		if(tasks[i].get_utilization() > utilization_max)
			utilization_max = tasks[i].get_utilization();
}
void TaskSet::get_density_sum(fraction_t &density_sum) const
{
	fraction_t temp;
	density_sum = 0;
	for(int i = 0; i < tasks.size(); i++)
	{
		temp = tasks[i].get_wcet();
		temp /= std::min(tasks[i].get_deadline(),tasks[i].get_period());
		density_sum += temp;
	}
}
void TaskSet::get_density_max(fraction_t &density_max) const
{
	density_max = tasks[0].get_density();
	for(int i = 1; i < tasks.size(); i++)
		if(tasks[i].get_density() > density_max)
			density_max = tasks[i].get_density();
}

const Tasks& TaskSet::get_tasks() const
{
	return tasks;
}

bool TaskSet::is_implicit_deadline()
{
	foreach(tasks,tasks[i].get_deadline() != tasks[i].get_period());
	return true;
}
bool TaskSet::is_constraint_deadline()
{
	foreach(tasks,tasks[i].get_deadline() > tasks[i].get_period());
	return true;
}
bool TaskSet::is_arbitary_deadline()
{
	return !(is_implicit_deadline())&&!(is_constraint_deadline());
}
uint TaskSet::get_taskset_size() const 
{
	return tasks.size();
}

fraction_t TaskSet::get_task_utilization(uint index) const
{
	return tasks[index].get_utilization();
}
fraction_t TaskSet::get_task_density(uint index) const
{
	return tasks[index].get_density();
}
ulong TaskSet::get_task_wcet(uint index) const
{
	return tasks[index].get_wcet();
}
ulong TaskSet::get_task_deadline(uint index) const
{
	return tasks[index].get_deadline();
}
ulong TaskSet::get_task_period(uint index) const
{
	return tasks[index].get_period();
}

void TaskSet::sort_by_period()
{
	sort(tasks.begin(), tasks.end(), period_increase);
	for(int i = 0; i < tasks.size(); i++)
		tasks[i].set_id(i);
}

void TaskSet::display()
{
	for(int i = 0; i < tasks.size(); i++)
	{
		cout<<"Task id:"<<tasks[i].get_id()<<" Task period:"<<tasks[i].get_period()<<endl;
	}
}


/////////////////////////////Others///////////////////////////////

void tast_gen(TaskSet *taskset, ResourceSet* resourceset, int lambda, Range p_range, double utilization,double probability, int num_max, Range l_range, double tlfs)
{
	//Random_Gen r;
	while(taskset->get_utilization_sum() < utilization)//generate tasks
	{
		long period = Random_Gen::uniform_integral_gen(int(p_range.min),int(p_range.max));
		fraction_t u = Random_Gen::exponential_gen(lambda);
		
		long wcet = period*u.get_d();
		if(0 == wcet)
			wcet++;
		else if(wcet > period)
			wcet = period;
		fraction_t temp(wcet, period);
		if(taskset->get_utilization_sum() + temp > utilization)
		{
			temp = utilization - taskset->get_utilization_sum();			
			wcet = period*temp.get_d();
			//taskset->add_task(wcet, period);
			taskset->add_task(resourceset, probability, num_max, l_range, tlfs, wcet, period);
			break;
		}
		//taskset->add_task(wcet,period);	
		taskset->add_task(resourceset, probability, num_max, l_range, tlfs, wcet, period);
	}
	taskset->sort_by_period();
}

ulong gcd(ulong a, ulong b)
{
	ulong temp;
	while(b)
	{
		temp = a;
		a = b;
		b = temp % b;		
	}
	return a;
}

ulong lcm(ulong a, ulong b)
{
	return a*b/gcd(a,b);
}









