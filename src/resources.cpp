#include "resources.h"
#include "tasks.h"
#include "processors.h"
#include "param.h"
#include "random_gen.h"
#include "sort.h"

/////////////////////////////Resource///////////////////////////////
Resource::Resource(uint id, uint locality, bool global_resource, bool processor_local_resource)
{
	this->resource_id = id;
	this->locality = locality;
	this->global_resource = global_resource;
	this->processor_local_resource = processor_local_resource;
	this->utilization = 0;
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

void Resource::init()
{
	locality = MAX_INT;
	global_resource = false;
	processor_local_resource = false;
	//utilization = 0;
	//queue.clear();
}

uint Resource::get_resource_id() const { return resource_id; }
void Resource::set_locality(uint locality) { this->locality = locality; }
uint Resource::get_locality() const { return locality; }
fraction_t Resource::get_utilization() const { return utilization; }

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
	fraction_t u;
	switch(((Task*)taskptr)->task_model())
	{
		case TPS_TASK_MODEL:
			Request& request_tps = ((Task*)taskptr)->get_request_by_id(resource_id);
			u = request_tps.get_num_requests() * request_tps.get_max_length();
			u /= ((Task*)taskptr)->get_period();
			utilization += u;
			break;
		case DAG_TASK_MODEL:
			Request& request_dag = ((DAG_Task*)taskptr)->get_request_by_id(resource_id);
			u = request_dag.get_num_requests() * request_dag.get_max_length();
			u /= ((DAG_Task*)taskptr)->get_period();
			utilization += u;
			break;
		default:
			Request& request_def = ((Task*)taskptr)->get_request_by_id(resource_id);
			u = request_def.get_num_requests() * request_def.get_max_length();
			u /= ((Task*)taskptr)->get_period();
			utilization += u;
	}
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

void ResourceSet::init()
{
	foreach(resources, resource)
	{
		resource->init();
	}
}

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

uint ResourceSet::get_resourceset_size() const
{
	return resources.size();
}


void ResourceSet::sort_by_utilization()
{
	sort(resources.begin(), resources.end(), utilization_decrease<Resource>);
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


