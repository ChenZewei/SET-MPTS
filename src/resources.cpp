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
Request_Tasks Resource::get_tasks() const { return tasks; }

void Resource::add_task(uint task_id)
{
	tasks.push_back(task_id);
}

/////////////////////////////ResourceSet///////////////////////////////

ResourceSet::ResourceSet(){}

void ResourceSet::add_resource()
{
	resources.push_back(Resource(resources.size()));
}

uint ResourceSet::size() const
{
	return resources.size();
}

void ResourceSet::add_task(uint resource_id, uint task_id)
{
	resources[resource_id].add_task(task_id);
}


/////////////////////////////Others///////////////////////////////

void resource_gen(ResourceSet *resourceset, int num)
{
	for(int i = 0; i < num; i++)
		resourceset->add_resource();
}
