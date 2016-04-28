#include "../include/processors.h"

///////////////////////////Processor/////////////////////////////

Processor::Processor(fraction_t speedfactor)
{
	speedfactor = speedfactor;
	utilization = 0;
	density = 0;
}

fraction_t Processor::get_speedfactor()
{
	return speedfactor;
}

fraction_t Processor::get_utilization()
{
	return utilization;
}

fraction_t Processor::get_density()
{
	return density;
}

void Processor::update(TaskSet taskset)
{	
	utilization = 0;
	list<uint>::iterator it = queue.begin();
	density = 0;
	for(; it != queue.end(); it++)
	{
		utilization += taskset.get_task_utilization(*it);
		density += taskset.get_task_density(*it);
	}
}

void Processor::add_task(TaskSet taskset, uint id)
{
	queue.push_back(id);
	update(taskset);
}

void Processor::remove_task(TaskSet taskset, uint id)
{
	list<uint>::iterator it = queue.begin();
	for(int i = 0; it != queue.end(); it++, i++)
	{
		if(id == *it)
		{
			queue.remove(i);
			break;
		}
	}
	update(taskset);
}

///////////////////////////ProcessorSet/////////////////////////////

ProcessorSet::ProcessorSet(uint num)//for identical multiprocessor platform
{
	for(int i = 0; i < num; i++)
		processors.push_back(Processor());
}

uint ProcessorSet::get_processor_num()
{
	return processors.size();
}
