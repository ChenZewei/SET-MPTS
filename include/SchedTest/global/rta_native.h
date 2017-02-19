#ifndef RTA_NATIVE_H
#define RTA_NATIVE_H

/*
** native RTA for global scheduling
** 
** RTSS 2009 Nan Guan [New Response Time Bounds for Fixed priority Multiprocessor Scheudling]
*/

#include "g_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

/*
class Task;
class TaskSet;
class Request;
class ProcessorSet;
class ResourceSet;
*/

class RTA_native: public GlobalSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong workload(Task& task, ulong interval);
		ulong response_time(Task& ti, TaskSet& tasks, ProcessorSet& processors);
	public:
		RTA_native();
		RTA_native(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources);
		bool is_schedulable();
		bool is_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources);
		bool is_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint TEST_TYPE = 0, uint ITER_BLOCKING = 0);
	
};

#endif


