#ifndef RTA_GFP_CZW_H
#define RTA_GFP_CZW_H

/*
** 
** 
** 
*/

#include "g_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

class RTA_GFP_CZW: public GlobalSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong workload(Task& task, ulong interval);
		ulong response_time(Task& ti);
		ulong interference(Task& tk, Task& ti, ulong interval);
	public:
		RTA_GFP_CZW();
		RTA_GFP_CZW(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		bool is_schedulable();

};

#endif
