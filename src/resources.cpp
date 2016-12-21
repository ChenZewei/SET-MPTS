#include "resources.h"
#include "tasks.h"
#include "processors.h"
#include "param.h"
#include "random_gen.h"

/////////////////////////////Resource///////////////////////////////
Resource::Resource(uint id, uint locality, bool global_resource, bool processor_local_resource)
{
	this->resource_id = id;
	this->locality = locality;
	this->global_resource = global_resource;
	this->processor_local_resource = processor_local_resource;
}

Resource::~Resource() 
{
/*
	foreach(queue, element)
	{
		delete *element;
	}
*/
}

uint Resource::get_resource_id() const { return resource_id; }
void Resource::set_locality(uint locality) { this->locality = locality; }
uint Resource::get_locality() const { return locality; }

bool Resource::is_global_resource() const { return global_resource; }

bool Resource::is_processor_local_resource() const { return processor_local_resource; }
//Request_Tasks Resource::get_tasks() const { return tasks; }


TaskQueue& Resource::get_taskqueue()
{
	return queue;
}


void Resource::add_task(void* taskptr)
{
	queue.push_back(taskptr);
}

uint Resource::get_ceiling()
{
	uint ceiling = 0xffffffff;	
	list<void*>::iterator it = queue.begin();
	for(uint i = 0; it != queue.end(); it++, i++)
	{
		if(ceiling >= ((Task*)(*it))->get_id())
			ceiling = ((Task*)(*it))->get_id();
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


void ResourceSet::add_task(uint resource_id, void* taskptr)
{
	resources[resource_id].add_task(taskptr);
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

void resource_gen(ResourceSet *resourceset, Param param)
{
	for(int i = 0; i < param.resource_num; i++)
		resourceset->add_resource();
	resource_alloc(*resourceset, param.p_num);
/*
	foreach(resourceset->get_resources(), resource)
	{
		cout<<"resource:"<<resource->get_resource_id()<<" locate at processor:"<<resource->get_locality()<<endl;
	}
*/
}

void resource_alloc(ResourceSet& resources, uint p_num)
{
	uint p_idx = 0;

	foreach(resources.get_resources(), resource)
	{
		p_idx = p_idx % p_num;
		resource->set_locality(p_idx);
		p_idx++;
	}
}


