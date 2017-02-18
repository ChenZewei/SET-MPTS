#ifndef RTA_BC_H
#define RTA_BC_H

/*
** Bertogna&Cirinei's RTA for global scheduling
** 
** RTSS 2009 Nan Guan [New Response Time Bounds for Fixed priority Multiprocessor Scheudling]
*/

#include "g_shced.h"

class Task;
class TaskSet;
class Request;
class ProcessorSet;
class ResourceSet;

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
		bool is_schedulable(TaskSet& tasks, ProcessorSet& processors, ResoruceSet& resoruces);
	
};

#endif
