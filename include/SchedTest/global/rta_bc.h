#ifndef RTA_BC_H
#define RTA_BC_H

/*
** Bertogna&Cirinei's RTA for global scheduling
** 
** RTSS 2009 Nan Guan [New Response Time Bounds for Fixed priority Multiprocessor Scheudling]
*/

#include "g_shced.h"
#include "factory_base.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

class RTA_BC: public GlobalSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong workload(Task& task, ulong interval);
		ulong response_time(TaskSet& tasks, ProcessorSet& processors, uint t_id);
		ulong interference(Task& tk, Task& ti, ulong interval);
	public:
		RTA_BC(TaskSet& tasks, ProcessorSet& processors, ResoruceSet& resoruces);
		bool is_schedulable();
		bool is_schedulable(TaskSet& tasks, ProcessorSet& processors, ResoruceSet& resoruces);
		bool is_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint TEST_TYPE = 0, uint ITER_BLOCKING = 0);

};

class Factory_RTA_BC: public FactoryBase
{
	public:
		RTA_BC* creativeSchedTest();
};

#endif
