#include "nc_gedf_vpr.h"
#include "iteration-helper.h"
#include "math-helper.h"

NC_GEDF_VPR::NC_GEDF_VPR(): GlobalSched(false, NC, EDF, NONE, "", "vpr") {}

NC_GEDF_VPR::NC_GEDF_VPR(TaskSet tasks, ProcessorSet processors, ResourceSet resources): GlobalSched(false, NC, EDF, NONE, "", "vpr")
{
	this->tasks = tasks;
	this->processors = processors;
	this->resources = resources;
	this->tasks.RM_Order();

}

bool NC_GEDF_VPR::is_schedulable()
{
	foreach(resources.get_resources(), resource)
	{
		uint r_id = resource->get_resource_id();

		if((!condition_1(r_id)) || (!condition_2(r_id)))
			return false;
	}
	return true;
}

bool NC_GEDF_VPR::condition_1(uint r_id)
{
	double sum = 0;
	foreach(tasks.get_tasks(), ti)
	{
		if((!ti->is_request_exist(r_id)))
			continue;

		double temp = 2*ti->get_wcet_critical_sections();
		temp /= ti->get_period();

		sum += temp;
	}

	if(sum <= 1)
		return true;
	else
		return false;
}

bool NC_GEDF_VPR::condition_2(uint r_id)
{
	foreach(tasks.get_tasks(), ti)
	{
		if((!ti->is_request_exist(r_id)))
			continue;

		ulong interval = ti->get_period()/2;

		ulong dbf = DBF(r_id, interval);
		ulong bt  = blocking_time(r_id, interval);

		if( (dbf + bt) > interval)
			return false;
	}
	return true;
}

ulong NC_GEDF_VPR::DBF(uint r_id, ulong interval)
{
	ulong sum = 0;

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		if((!ti->is_request_exist(r_id)))
			continue;

		ulong p_i = ti->get_period();
		ulong wcet_i_c = ti->get_wcet_critical_sections();
		sum += ((interval + p_i)/p_i)*(2*wcet_i_c);
	}
	return sum;
}

ulong NC_GEDF_VPR::blocking_time(uint r_id, ulong interval)
{
	ulong max = 0;

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		if((!ti->is_request_exist(r_id)) || (ti->get_deadline() <= interval))
			continue;
		
		ulong wcet_i = ti->get_wcet();
		if(max < 2*wcet_i)
			max = 2*wcet_i;
	}
	return max;
}
