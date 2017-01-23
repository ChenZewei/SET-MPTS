#ifndef PFP_GS_H
#define PFP_GS_H

#include "types.h"

class Task;
class TaskSet;
class Processor;
class ProcessorSet;
class Resource;
class ResourceSet;

ulong pfp_gs_local_blocking(Task& ti, Taskset& tasks, ProcessorSet& processors, ResourceSet& resources);

ulong pfp_gs_spin_time(Task& ti, uint resource_id, Taskset& tasks, ProcessorSet& processors, ResourceSet& resources);

ulong pfp_gs_remote_blocking(Task& ti, Taskset& tasks, ProcessorSet& processors, ResourceSet& resources);



#endif
