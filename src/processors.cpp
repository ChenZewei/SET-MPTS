#include "../include/processors.h"

///////////////////////////Processor/////////////////////////////

Processor::Processor(fraction_t speedfactor)
{
	speedfactor = speedfactor;
	utilization = 0;
	density = 0;
	tryed_assign = false;
}

fraction_t Processor::get_speedfactor() const
{
	return speedfactor;
}

fraction_t Processor::get_utilization() const
{
	return utilization;
}

fraction_t Processor::get_density() const
{
	return density;
}

bool Processor::get_tryed_assign() const
{
	return tryed_assign;
}

bool Processor::add_task(TaskSet taskset, uint t_id)
{
	if(1 < utilization + taskset.get_task_utilization(t_id))
		return false;
	queue.push_back(t_id);
	utilization += taskset.get_task_utilization(t_id);
	density += taskset.get_task_density(t_id);
	return true;	
}

bool Processor::remove_task(TaskSet taskset, uint t_id)
{
	list<uint>::iterator it = queue.begin();
	for(uint i = 0; it != queue.end(); it++, i++)
	{
		if(t_id == *it)
		{
			queue.remove(i);
			utilization -= taskset.get_task_utilization(t_id);
			density -= taskset.get_task_density(t_id);
			return true;
		}
	}
	return false;
}

void Processor::clear()
{
	queue.clear();
	utilization = 0;
	density = 0;
	tryed_assign = false;
}
///////////////////////////ProcessorSet/////////////////////////////

ProcessorSet::ProcessorSet(uint num)//for identical multiprocessor platform
{
	for(uint i = 0; i < num; i++)
		processors.push_back(Processor());
}

uint ProcessorSet::get_processor_num() const 
{
	return processors.size();
}

const Processors& ProcessorSet::get_processors() const 
{
	return processors;
}
