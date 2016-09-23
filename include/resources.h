#ifndef RESOURCES_H
#define RESOURCES_H
#include <vector>
#include <list>
#include "types.h"
#include "processors.h"
#include "param.h"

using namespace std;

//typedef vector<uint> Request_Tasks;
template <typename TaskModlePtr>
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
		Resource(uint id, uint locality = 0, bool global_resource = false, bool processor_local_resource = false);
		uint get_resource_id() const;
		uint get_locality() const;
		bool is_global_resource() const;
		bool is_processor_local_resource() const;
		//Request_Tasks get_tasks() const;
		TaskQueue& get_taskqueue();
		void add_task(TaskModlePtr);
		uint get_ceiling();
};

typedef vector<Resource> Resources;

class ResourceSet
{
	private:
		Resources resources;
	public:
		ResourceSet();
		void add_resource();
		uint size() const;
		Resources& get_resources();
		const uint& get_resourceset_size() const;
		void add_task(uint resource_id, Task* task);
};

void resource_gen(ResourceSet *resourceset, Param param);

#endif

