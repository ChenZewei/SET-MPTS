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

/////////////////////////////TaskSet///////////////////////////////

TaskSet::TaskSet()
{
	utilization_sum = 0;
	utilization_max = 0;
	density_sum = 0;
	density_max = 0;
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

void TaskSet::sort_by_period()
{
	sort(tasks.begin(), tasks.end(), period_increase);
	for(int i = 0; i < tasks.size(); i++)
		tasks[i].set_id(i);
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









