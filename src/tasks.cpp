#include "../include/tasks.h"
#include "math-helper.h"

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

fraction_t Task::get_utilization()
{
	return utilization;
}

fraction_t Task::get_density()
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

fraction_t TaskSet::get_utilization_sum()
{
	return utilization_sum;
}

fraction_t TaskSet::get_utilization_max()
{
	return utilization_max;
}

fraction_t TaskSet::get_density_sum()
{
	return density_sum;
}

fraction_t TaskSet::get_density_max()
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

void TaskSet::get_utilization_sum(fraction_t &utilization_sum)
{
	fraction_t temp;
	utilization_sum = 0;
	for(int i; i < tasks.size(); i++)
	{
		temp = tasks[i].get_wcet();
		temp /= tasks[i].get_period();
		utilization_sum += temp;
	}
}
void TaskSet::get_utilization_max(fraction_t &utilization_max)
{
	utilization_max = tasks[0].get_utilization();
	for(int i = 1; i < tasks.size(); i++)
		if(tasks[i].get_utilization() > utilization_max)
			utilization_max = tasks[i].get_utilization();
}
void TaskSet::get_density_sum(fraction_t &density_sum)
{
	fraction_t temp;
	density_sum = 0;
	for(int i; i < tasks.size(); i++)
	{
		temp = tasks[i].get_wcet();
		temp /= std::min(tasks[i].get_deadline(),tasks[i].get_period());
		density_sum += temp;
	}
}
void TaskSet::get_density_max(fraction_t &density_max)
{
	density_max = tasks[0].get_density();
	for(int i = 1; i < tasks.size(); i++)
		if(tasks[i].get_density() > density_max)
			density_max = tasks[i].get_density();
}

/////////////////////////////Others///////////////////////////////

void tast_gen(TaskSet *taskset, int lambda, Range p_range, double u_ceil)
{
	while(taskset->get_utilization_sum() < u_ceil)//generate tasks
	{
		long period = uniform_integral_gen(int(p_range.min),int(p_range.max));
		fraction_t u = exponential_gen(lambda);
		
		long wcet = period*u.get_d();
		if(0 == wcet)
			wcet++;
		else if(wcet > period)
			wcet = period;
		fraction_t temp(wcet, period);
		if(taskset->get_utilization_sum() + temp > u_ceil)
		{
			temp = u_ceil - taskset->get_utilization_sum();			
			wcet = period*temp.get_d();
			taskset->add_task(wcet, period);
			break;
		}
		taskset->add_task(wcet,period);	
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









