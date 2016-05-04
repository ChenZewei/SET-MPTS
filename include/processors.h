#ifndef PROCESSORS_H
#define PROCESSORS_H
#include <list>
#include "tasks.h"
#include "types.h"

using namespace std;

typedef list<unsigned int> TaskQueue;//之所以用list是因为list是双向链表，适合增删比较频繁的情况

class Processor
{
	private:
		TaskQueue queue;

		fraction_t speedfactor;
		fraction_t utilization;
		fraction_t density;
	public:
		Processor(fraction_t speedfactor = 1);
		fraction_t get_speedfactor();
		fraction_t get_utilization();
		fraction_t get_density();
		void update(TaskSet taskset);

		void add_task(TaskSet taskset, unsigned int id);
		void remove_task(TaskSet taskset, unsigned int id);
		
	
};

typedef vector<Processor> Processors;

class ProcessorSet
{
	private:
		Processors processors;
	public:
		ProcessorSet(unsigned int num);//for identical multiprocessor platform
		unsigned int get_processor_num();
};
#endif
