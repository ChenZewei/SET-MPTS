#include "resources.h"

/////////////////////////////Resource///////////////////////////////


Resource::Resource(uint id, uint locality, bool global_resource, bool processor_local_resource)
{
	this->resource_id = id;
	this->locality = locality;
	this->global_resource = global_resource;
	this->processor_local_resource = processor_local_resource;
}

uint Resource::get_resource_id() const { return resource_id; }
uint Resource::get_locality() const { return locality; }
bool Resource::is_global_resource() const { return global_resource; }
bool Resource::is_processor_local_resource() const { return processor_local_resource; }
//Request_Tasks Resource::get_tasks() const { return tasks; }

TaskQueue& Resource::get_taskqueue()
{
	return queue;
}

void Resource::add_task(Task* task)
{
	queue.push_back(task);
}

uint Resource::get_ceiling()
{
	uint ceiling = 0xffffffff;	
	list<Task*>::iterator it = queue.begin();
	for(uint i = 0; it != queue.end(); it++, i++)
	{
		if(ceiling >= (*it)->get_id())
			ceiling = (*it)->get_id();
	}
	return ceiling;
}

/////////////////////////////ResourceSet///////////////////////////////

ResourceSet::ResourceSet(){}

void ResourceSet::add_resource()
{
	bool global = false;
	if(Random_Gen::probability(0.4))
		global = true;
	resources.push_back(Resource(resources.size(), 0, global));
}

uint ResourceSet::size() const
{
	return resources.size();
}

void ResourceSet::add_task(uint resource_id, Task* task)
{
	resources[resource_id].add_task(task);
}

Resources& ResourceSet::get_resources()
{
	return resources;
}

const uint& ResourceSet::get_resourceset_size() const
{
	return resources.size();
}

/////////////////////////////Others///////////////////////////////

void resource_gen(ResourceSet *resourceset, int num)
{
	for(int i = 0; i < num; i++)
		resourceset->add_resource();
}
