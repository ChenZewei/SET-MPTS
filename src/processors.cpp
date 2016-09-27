#include "../include/processors.h"

///////////////////////////Processor/////////////////////////////

Processor::Processor(uint id, fraction_t speedfactor)
{
	processor_id = id;
	speedfactor = speedfactor;
	utilization = 0;
	density = 0;
	tryed_assign = false;
}

uint Processor::get_processor_id() const
{
	return processor_id;
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

TaskQueue& Processor::get_taskqueue()
{
	return queue;
}

bool Processor::add_task(Task* task)
{
	if(1 < utilization + task->get_utilization())
		return false;
	queue.push_back(task);
	utilization += task->get_utilization();
	density += task->get_density();
	return true;	
}

bool Processor::remove_task(Task* task)
{
	list<Task*>::iterator it = queue.begin();
	for(uint i = 0; it != queue.end(); it++, i++)
	{
		if(task == *it)
		{
			queue.remove(*it);
			utilization -= task->get_utilization();
			density -= task->get_density();
			return true;
		}
	}
	return false;
}

void Processor::init()
{
	queue.clear();
	utilization = 0;
	density = 0;
	tryed_assign = false;
}
///////////////////////////ProcessorSet/////////////////////////////

ProcessorSet::ProcessorSet(Param param)//for identical multiprocessor platform
{
	for(uint i = 0; i < param.p_num; i++)
		processors.push_back(Processor(i));
}

uint ProcessorSet::get_processor_num() const 
{
	return processors.size();
}

Processors& ProcessorSet::get_processors()
{
	return processors;
}

void ProcessorSet::init()
{
	for(uint i = 0; i < processors.size(); i++)
		processors[i].init();
}
