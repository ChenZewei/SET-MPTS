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
		bool global_resource;
		bool processor_local_resource;
		//Request_Tasks tasks;
		TaskQueue queue;
	public:
		Resource(uint id, uint locality = MAX_INT, bool global_resource = false, bool processor_local_resource = false);
		uint get_resource_id() const;
		void set_locality(uint locality);
		uint get_locality() const;
		bool is_global_resource() const;
		bool is_processor_local_resource() const;
		//Request_Tasks get_tasks() const;
		TaskQueue& get_taskqueue();
		void add_task(void* taskptr);
		uint get_ceiling();
};

typedef vector<Resource> Resources;

class ResourceSet
{
	private:
		vector<Resource> resources;
	public:
		ResourceSet();
		void add_resource();
		uint size() const;
		Resources& get_resources();
		const uint& get_resourceset_size() const;
		void add_task(uint resource_id, void* taskptr);
};

void resource_gen(ResourceSet *resourceset, Param param);

/*
 * Using next fit allocation
 */
void resource_alloc(ResourceSet& resources, uint p_num);



#endif

