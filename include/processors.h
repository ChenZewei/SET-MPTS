#ifndef PROCESSORS_H
#define PROCESSORS_H

#include "types.h"

using namespace std;

class Task;
class TaskSet;
class DAG_Task;
class DAG_TaskSet;
class Param;

class Processor
{
	private:
		uint processor_id;
		fraction_t speedfactor;
		fraction_t utilization;
		fraction_t density;
		bool tryed_assign;
		TaskQueue queue;
	public:
		Processor(uint id, fraction_t speedfactor = 1);
		~Processor();
		uint get_processor_id() const;
		fraction_t get_speedfactor() const;
		fraction_t get_utilization() const;
		fraction_t get_density() const;
		bool get_tryed_assign() const;
		TaskQueue& get_taskqueue();
		bool add_task(void* taskptr);
		bool remove_task(void* taskptr);
		void init();	
};

typedef vector<Processor> Processors;

class ProcessorSet
{
	private:
		Processors processors;
	public:
		ProcessorSet(Param param);//for identical multiprocessor platform
		uint get_processor_num() const;
		Processors& get_processors();
		void init();
		
};
#endif
