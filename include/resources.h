#ifndef RESOURCES_H
#define RESOURCES_H
#include <vector>
#include <list>
#include "types.h"

using namespace std;

typedef vector<uint> Request_Tasks;
class Resource
{
	private:
		uint resource_id;
		uint locality;
		bool global_resource;
		bool processor_local_resource;
		Request_Tasks tasks;

	public:
		Resource(uint id, uint locality = 0, bool global_resource = false, bool processor_local_resource = false);
		uint get_resource_id() const;
		uint get_locality() const;
		bool is_global_resource() const;
		bool is_processor_local_resource() const;
		Request_Tasks get_tasks() const;

		void add_task(uint task_id);
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
		uint get_resourceset_size()
		{
			return resources.size();
		}
};

void resource_gen(ResourceSet *resourceset, int num);

#endif

