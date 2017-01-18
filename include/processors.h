#ifndef PROCESSORS_H
#define PROCESSORS_H

#include "types.h"

using namespace std;

class Task;
class TaskSet;
class DAG_Task;
class DAG_TaskSet;
class Resource;
class ResourceSet;
class Param;

class Processor
{
	private:
		uint processor_id;
		fraction_t speedfactor;
		fraction_t utilization;
		fraction_t resource_utilization;
		fraction_t density;
		bool tryed_assign;
		TaskQueue tQueue;
		ResourceQueue rQueue;
	public:
		Processor(uint id, fraction_t speedfactor = 1);
		~Processor();
		uint get_processor_id() const;
		fraction_t get_speedfactor() const;
		fraction_t get_utilization() const;
		fraction_t get_density() const;
		fraction_t get_resource_utilization() const;
		bool get_tryed_assign() const;
		TaskQueue& get_taskqueue();
		bool add_task(void* taskptr);
		bool remove_task(void* taskptr);
		ResourceQueue& get_resourcequeue();
		bool add_resource(void* resourceptr);
		bool remove_resource(void* resourceptr);
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
		void sort_by_task_utilization(uint dir);
		void sort_by_resource_utilization(uint dir);
		
};
#endif
