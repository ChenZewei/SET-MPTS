#include "processors.h"
#include "tasks.h"
#include "resources.h"
#include "param.h"
#include "sort.h"
#include "iteration-helper.h"
#include "math-helper.h"

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
	tQueue.clear();
	rQueue.clear();
}

uint Processor::get_processor_id() const
{
	return processor_id;
}

fraction_t Processor::get_speedfactor() const
{
	return speedfactor;
}

fraction_t Processor::get_utilization()
{
	utilization = 0;
	if(0 == tQueue.size())
		return utilization;
	foreach(tQueue, t_id)
	{
		Task& task = tasks->get_task_by_id(*t_id);
		utilization += task.get_utilization();
	}
	return utilization;
}

fraction_t Processor::get_density()
{
	density = 0;
	if(0 == tQueue.size())
		return density;
	foreach(tQueue, t_id)
	{
		Task& task = tasks->get_task_by_id(*t_id);
		density += task.get_density();
	}
	return density;
}


fraction_t Processor::get_resource_utilization()
{
	resource_utilization = 0;
	if(0 == rQueue.size())
		return resource_utilization;
	foreach(rQueue, r_id)
	{
		Resource& resource = resources->get_resources()[*r_id];
		resource_utilization += resource.get_utilization();
	}
	return resource_utilization;
}

bool Processor::get_tryed_assign() const
{
	return tryed_assign;
}

const set<uint>& Processor::get_taskqueue()
{
	return tQueue;
}

bool Processor::add_task(uint t_id)
{
	Task& task = tasks->get_task_by_id(t_id);
//cout<<"utilization_sum:"<<utilization.get_d()<<" u_t:"<<task.get_utilization().get_d()<<endl;
	if(1 < get_utilization() + task.get_utilization())
		return false;

	tQueue.insert(t_id);
	return true;	
}

bool Processor::remove_task(uint t_id)
{
	tQueue.erase(t_id);
	return true;
}


const set<uint>& Processor::get_resourcequeue()
{
	return rQueue;
}

bool Processor::add_resource(uint r_id)
{
	Resource& resource = resources->get_resources()[r_id];
	if(1 < resource_utilization + resource.get_utilization())
			return false;

	rQueue.insert(r_id);
	return true;
}
bool Processor::remove_resource(uint r_id)
{
	rQueue.erase(r_id);
	return true;
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


void Processor::update(const TaskSet* tasks, const ResourceSet* resources)
{
	this->tasks = tasks;
	this->resources = resources;
}

///////////////////////////ProcessorSet/////////////////////////////

ProcessorSet::ProcessorSet() {}

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

void ProcessorSet::update(const TaskSet* tasks, const ResourceSet* resources)
{
	foreach(processors, processor)
		processor->update(tasks, resources);
}






