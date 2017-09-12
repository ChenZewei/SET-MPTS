#ifndef RTA_GDC_NATIVE_H
#define RTA_GDC_NATIVE_H

/*
** 
** 
** 
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

class RTA_GDC_native: public GlobalSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong workload(Task& task, ulong interval);
		ulong response_time(Task& ti);
	public:
		RTA_GDC_native();
		RTA_GDC_native(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		bool is_schedulable();
	
};

#endif

