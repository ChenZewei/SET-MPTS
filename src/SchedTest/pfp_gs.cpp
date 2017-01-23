#include "pfp_gs.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

ulong pfp_gs_local_blocking(Task& ti, Taskset& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong blocling = 0;
	uint p_id = ti.get_partition();
	
	foreach(Resources.get_resources(), resource)
	{
		uint q = resource->get_resource_id();
		foreach(tasks.get_tasks(), tj)
		{
			if((tj->is_request_exist(q)) && (resource->get_ceiling() < ti.get_id()) && (ti.get_id() < tj->get_id()) && (p_id == resource->get_locality()))
			{
				ulong length = tj->get_request_by_id(q).get_max_length();
				if(blocking < length)
					blocking = length;
			}
		}
	}
	return blocking;
}

ulong pfp_gs_spin_time(Task& ti, uint resource_id, Taskset& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	uint p_id = ti.get_partition();
	ulong sum = 0;
	for(uint i = 0; i < processors.get_processor_num(); i++)
	{
		if(p_id == i)
			continue;
		Processor& processor = processors.get_processors()[i];
		ulong max_spin_time = 0;
		foreach(tasks.get_tasks(), tx)
		{
			if((i = tx->get_partition()) && (tx->is_request_exist(resource_id)))
			{
				ulong length = tx->get_request_by_id(resource_id).get_max_length();
				if(max_spin_time < length)
					max_spin_time = length;
			}
		}
		sum += max_spin_time;
	}
	return sum;
}

ulong pfp_gs_remote_blocking(Task& ti, Taskset& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong blocking = 0;

	foreach(ti.get_requests(), request)
	{
		uint q = request->get_resource_id();
		blocking += request->get_num_requests()*pfp_gs_spin_time(ti, q, tasks, processors, resources);
	}
	return blocking;
}























