#ifndef PROCESSORS_H
#define PROCESSORS_H
#include <vector>
#include <list>
#include "tasks.h"
#include "types.h"

using namespace std;

typedef list<uint> TaskQueue;//之所以用list是因为list是双向链表，适合增删比较频繁的情况

class Processor
{
	private:
		TaskQueue queue;

		fraction_t speedfactor;
		fraction_t utilization;
		fraction_t density;
		bool tryed_assign;
	public:
		Processor(fraction_t speedfactor = 1);
		fraction_t get_speedfactor();
		fraction_t get_utilization() const
		{
			return utilization;
		}
		fraction_t get_density();
		bool get_tryed_assign();

		void add_task(TaskSet taskset, uint id);
		void remove_task(TaskSet taskset, uint id);
		
	
};

typedef vector<Processor> Processors;

class ProcessorSet
{
	private:
		Processors processors;
	public:
		ProcessorSet(uint num);//for identical multiprocessor platform
		uint get_processor_num() const 
		{
			return processors.size();
		}
		const Processors& get_processors() const 
		{
			return processors;
		}
};
#endif
