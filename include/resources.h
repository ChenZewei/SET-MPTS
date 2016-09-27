#ifndef RESOURCES_H
#define RESOURCES_H
#include <vector>
#include <list>
#include "types.h"
#include "processors.h"
#include "param.h"

using namespace std;

//typedef vector<uint> Request_Tasks;
//template<class TaskModlePtr>
class Resource
{
	private:
		uint resource_id;
		uint locality;
		bool global_resource;
		bool processor_local_resource;
		//Request_Tasks tasks;
		list<void*> queue;
	public:
		Resource(uint id, uint locality = 0, bool global_resource = false, bool processor_local_resource = false);
		uint get_resource_id() const;
		uint get_locality() const;
		bool is_global_resource() const;
		bool is_processor_local_resource() const;
		//Request_Tasks get_tasks() const;
		TaskQueue& get_taskqueue();
		void add_task(void* taskptr);
		uint get_ceiling();
};

typedef vector<Resource> Resources;

//template <typename TaskModlePtr>
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

//////////////////////////for multiple task models//////////////////////////////
template<typename TaskModle>
class Resource2
{
	private:
		uint resource_id;
		uint locality;
		bool global_resource;
		bool processor_local_resource;
		//Request_Tasks tasks;
		list<TaskModle*> queue;
	public:
		Resource2(uint id, uint locality = 0, bool global_resource = false, bool processor_local_resource = false);
		uint get_resource_id() const;
		uint get_locality() const;
		bool is_global_resource() const;
		bool is_processor_local_resource() const;
		//Request_Tasks get_tasks() const;
		list<TaskModle*>& get_taskqueue();
		void add_task(TaskModle* taskptr);
		uint get_ceiling();
};

template <typename TaskModle>
class ResourceSet2
{
	private:
		vector< Resource2<TaskModle> > resources;
	public:
		ResourceSet2();
		void add_resource();
		uint size() const;
		Resources& get_resources();
		const uint& get_resourceset_size() const;
		void add_task(uint resource_id, TaskModle* taskptr);
};


void resource_gen(ResourceSet *resourceset, Param param);

template <typename TaskModle>
void resource_gen2(ResourceSet2<TaskModle> *resourceset, Param param)
{
	for(int i = 0; i < param.resource_num; i++)
		resourceset->add_resource();
}

/////////////////////////////Resource2///////////////////////////////

template<typename TaskModle>
Resource2<TaskModle>::Resource2(uint id, uint locality, bool global_resource, bool processor_local_resource)
{
	this->resource_id = id;
	this->locality = locality;
	this->global_resource = global_resource;
	this->processor_local_resource = processor_local_resource;
}

template<typename TaskModle>
uint Resource2<TaskModle>::get_resource_id() const { return resource_id; }
template<typename TaskModle>
uint Resource2<TaskModle>::get_locality() const { return locality; }
template<typename TaskModle>
bool Resource2<TaskModle>::is_global_resource() const { return global_resource; }
template<typename TaskModle>
bool Resource2<TaskModle>::is_processor_local_resource() const { return processor_local_resource; }
//Request_Tasks Resource::get_tasks() const { return tasks; }

template<typename TaskModle>
list<TaskModle*>& Resource2<TaskModle>::get_taskqueue()
{
	return queue;
}

template<typename TaskModle>
void Resource2<TaskModle>::add_task(TaskModle* taskptr)
{
	queue.push_back(taskptr);
}

template<typename TaskModle>
uint Resource2<TaskModle>::get_ceiling()
{
	uint ceiling = 0xffffffff;	
	typename list<TaskModle*>::iterator it = queue.begin();
	for(uint i = 0; it != queue.end(); it++, i++)
	{
		if(ceiling >= (*it)->get_id())
			ceiling = (*it)->get_id();
	}
	return ceiling;
}

/////////////////////////////ResourceSet2///////////////////////////////

template <typename TaskModle>
ResourceSet2<TaskModle>::ResourceSet2(){}

template <typename TaskModle>
void ResourceSet2<TaskModle>::add_resource()
{
	bool global = false;
	if(Random_Gen::probability(0.4))
		global = true;
	resources.push_back(Resource2<TaskModle>(resources.size(), 0, global));
}

template <typename TaskModle>
uint ResourceSet2<TaskModle>::size() const
{
	return resources.size();
}

template <typename TaskModle>
void ResourceSet2<TaskModle>::add_task(uint resource_id, TaskModle* taskptr)
{
	resources[resource_id].add_task(taskptr);
}

template <typename TaskModle>
Resources& ResourceSet2<TaskModle>::get_resources()
{
	return resources;
}

template <typename TaskModle>
const uint& ResourceSet2<TaskModle>::get_resourceset_size() const
{
	return resources.size();
}


#endif

