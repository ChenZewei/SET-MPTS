#ifndef RTA_GFP_BC_H
#define RTA_GFP_BC_H

/*
** Bertogna&Cirinei's RTA for global scheduling
** 
** RTSS 2009 Nan Guan [New Response Time Bounds for Fixed priority Multiprocessor Scheudling]
*/

#include "g_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

class RTA_GFP_BC: public GlobalSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong workload(Task& task, ulong interval);
		ulong response_time(Task& ti);
		ulong interference(Task& tk, Task& ti, ulong interval);
	public:
		RTA_GFP_BC();
		RTA_GFP_BC(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		bool is_schedulable();

};

#endif
