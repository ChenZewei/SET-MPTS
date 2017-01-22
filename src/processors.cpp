#include "processors.h"
#include "tasks.h"
#include "resources.h"
#include "param.h"
#include "sort.h"

///////////////////////////Processor/////////////////////////////

Processor::Processor(uint id, fraction_t speedfactor)
{
	processor_id = id;
	speedfactor = speedfactor;
	utilization = 0;
	resource_utilization = 0;
	density = 0;
	tryed_assign = false;
}

Processor::~Processor()
{
	
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


fraction_t Processor::get_resource_utilization() const
{
	return resource_utilization;
}

bool Processor::get_tryed_assign() const
{
	return tryed_assign;
}

TaskQueue& Processor::get_taskqueue()
{
	return tQueue;
}

bool Processor::add_task(void* taskptr)
{
	if(1 < utilization + ((Task*)taskptr)->get_utilization())
		return false;
	tQueue.push_back(taskptr);
	utilization += ((Task*)taskptr)->get_utilization();
	density += ((Task*)taskptr)->get_density();
	return true;	
}

bool Processor::remove_task(void* taskptr)
{
	TaskQueue::iterator it = tQueue.begin();
	for(uint i = 0; it != tQueue.end(); it++, i++)
	{
		if(taskptr == *it)
		{
			tQueue.remove(*it);
			utilization -= ((Task*)taskptr)->get_utilization();
			density -= ((Task*)taskptr)->get_density();
			return true;
		}
	}
	return false;
}


ResourceQueue& Processor::get_resourcequeue()
{
	return rQueue;
}

bool Processor::add_resource(void* resourceptr)
{
	if(1 < resource_utilization + ((Resource*)resourceptr)->get_utilization())
			return false;
		rQueue.push_back(resourceptr);
		resource_utilization += ((Resource*)resourceptr)->get_utilization();
		return true;
}
bool Processor::remove_resource(void* resourceptr)
{
	ResourceQueue::iterator it = rQueue.begin();
	for(uint i = 0; it != tQueue.end(); it++, i++)
	{
		if(resourceptr == *it)
		{
			rQueue.remove(*it);
			resource_utilization -= ((Resource*)resourceptr)->get_utilization();
			return true;
		}
	}
	return false;
}

void Processor::init()
{
	utilization = 0;
	resource_utilization = 0;
	density = 0;
	tryed_assign = false;
	tQueue.clear();
	rQueue.clear();
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

void ProcessorSet::sort_by_task_utilization(uint dir)
{
	switch(dir)
	{
		case INCREASE://For worst fit
			sort(processors.begin(), processors.end(), task_utilization_increase<Processor>);
			break;
		case DECREASE://For best fit
			sort(processors.begin(), processors.end(), task_utilization_decrease<Processor>);
			break;
		default:
			sort(processors.begin(), processors.end(), task_utilization_increase<Processor>);
			break;
	}
}

void ProcessorSet::sort_by_resource_utilization(uint dir)
{
	switch(dir)
	{
		case INCREASE://For worst fit
			sort(processors.begin(), processors.end(), resource_utilization_increase<Processor>);
			break;
		case DECREASE://For best fit
			sort(processors.begin(), processors.end(), resource_utilization_decrease<Processor>);
			break;
		default:
			sort(processors.begin(), processors.end(), resource_utilization_increase<Processor>);
			break;
	}
}








