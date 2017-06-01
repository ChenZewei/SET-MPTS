#ifndef RESOURCES_H
#define RESOURCES_H

#include "types.h"

using namespace std;

class Task;
class TaskSet;
class DAG_Task;
class DAG_TaskSet;
class Processor;
class ProcessorSet;
class Param;
class Random_Gen;

class Resource
{
	private:
		uint resource_id;
		uint locality;
		fraction_t utilization;
		bool global_resource;
		bool processor_local_resource;
		//Request_Tasks tasks;
		TaskQueue queue;
		vector<uint> tasks;
	public:
		Resource(uint id, uint locality = MAX_INT, bool global_resource = false, bool processor_local_resource = false);
		~Resource();
		void init();
		uint get_resource_id() const;
		void set_locality(uint locality);
		uint get_locality() const;
		fraction_t get_utilization() const;
		bool is_global_resource();
		bool is_processor_local_resource() const;
		//Request_Tasks get_tasks() const;
		TaskQueue& get_taskqueue();
		void add_task(void* taskptr);
		uint get_ceiling();

		//for debug
		void display_task_queue();
};

typedef vector<Resource> Resources;

class ResourceSet
{
	private:
		vector<Resource> resources;
	public:
		ResourceSet();
		void init();
		void add_resource();
		uint size() const;
		Resources& get_resources();
		uint get_resourceset_size() const;
		void add_task(uint resource_id, void* taskptr);
		void sort_by_utilization();
};

void resource_gen(ResourceSet *resourceset, Param param);

/*
 * Using next fit allocation
 */
void resource_alloc(ResourceSet& resources, uint p_num);



#endif

