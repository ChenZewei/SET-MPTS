#ifndef RTA_GFP_NG_H
#define RTA_GFP_NG_H

/*
** Nan Guan's Method RTA for global scheduling with constraint deadline task system
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

class RTA_GFP_NG: public GlobalSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		typedef struct
		{
			uint id;
			ulong gap;
		}Carry_in;
		
		static int interference_gap_decrease(Carry_in t1, Carry_in t2);
		ulong workload_nc(Task& task, long interval);
		ulong workload_ci(Task& task, long interval);
		ulong interference_nc(Task& task_k, Task& task_i, ulong interval);
		ulong interference_ci(Task& task_k, Task& task_i, ulong interval);
		ulong total_interference(Task& task_k, ulong interval);
		ulong response_time(Task& ti);

	public:
		RTA_GFP_NG();
		RTA_GFP_NG(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources);
		bool is_schedulable();
	
};

#endif


