#include "../include/processors.h"

///////////////////////////Processor/////////////////////////////

Processor::Processor(fraction_t speedfactor)
{
	speedfactor = speedfactor;
	utilization = 0;
	density = 0;
	tryed_assign = false;
}

fraction_t Processor::get_speedfactor()
{
	return speedfactor;
}

fraction_t Processor::get_density()
{
	return density;
}

bool Processor::get_tryed_assign()
{
	return tryed_assign;
}

void Processor::add_task(TaskSet taskset, uint t_id)
{
	queue.push_back(t_id);
	utilization += taskset.get_task_utilization(t_id);
	density += taskset.get_task_density(t_id);	
}

void Processor::remove_task(TaskSet taskset, uint t_id)
{
	list<uint>::iterator it = queue.begin();
	for(uint i = 0; it != queue.end(); it++, i++)
	{
		if(t_id == *it)
		{
			queue.remove(i);
			utilization -= taskset.get_task_utilization(t_id);
			density -= taskset.get_task_density(t_id);
			break;
		}
	}
}

///////////////////////////ProcessorSet/////////////////////////////

ProcessorSet::ProcessorSet(uint num)//for identical multiprocessor platform
{
	for(uint i = 0; i < num; i++)
		processors.push_back(Processor());
}


