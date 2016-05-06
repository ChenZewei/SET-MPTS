#ifndef PROCESSORS_H
#define PROCESSORS_H
#include <vector>
#include <list>
#include "tasks.h"
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
		uint get_resource_id() const { return resource_id; }
		uint get_locality() const { return locality; }
		bool is_global_resource() const { return global_resource; }
		bool is_processor_local_resource() const { return processor_local_resource; }
		Request_Tasks get_tasks() const { return tasks; }

		void add_task(uint task_id);
};

typedef vector<Resource> Resources;

class ResourceSet
{
	private:
		Resources resources;
	public:

};

