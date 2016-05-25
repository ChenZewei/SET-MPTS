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
		fraction_t get_speedfactor() const;
		fraction_t get_utilization() const;
		fraction_t get_density() const;
		bool get_tryed_assign() const;

		bool add_task(TaskSet taskset, uint id);
		bool remove_task(TaskSet taskset, uint id);
		void clear();	
};

typedef vector<Processor> Processors;

class ProcessorSet
{
	private:
		Processors processors;
	public:
		ProcessorSet(uint num);//for identical multiprocessor platform
		uint get_processor_num() const;
		const Processors& get_processors() const;
};
#endif
