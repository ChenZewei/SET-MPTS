#include "../include/processors.h"

///////////////////////////Processor/////////////////////////////

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
