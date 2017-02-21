#ifndef RTA_GFP_NATIVE_H
#define RTA_GFP_NATIVE_H

/*
** native RTA for global scheduling with constraint deadline task system
** 
** RTSS 2009 Nan Guan et al. [New Response Time Bounds for Fixed priority Multiprocessor Scheudling]
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

class RTA_GFP_native: public GlobalSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong workload(Task& task, ulong interval);
		ulong response_time(Task& ti);
	public:
		RTA_GFP_native();
		RTA_GFP_native(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources);
		bool is_schedulable();
	
};

#endif


