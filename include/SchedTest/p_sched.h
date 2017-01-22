#ifndef P_SCHED_H
#define P_SCHED_H

#include "types.h"
#include "sched_test_base.h"

class Task;
class TaskSet;
class Request;
class ProcessorSet;
class ResourceSet;

class PartitionedSched: public SchedTestBase
{
	private:
		
	public:
		enum
		{
			UNTEST,
			TEST
		};

		PartitionedSched();

		//Bin packing
		void BinPacking_WF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE);
		void BinPacking_BF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE);
		void BinPacking_FF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE);
		void BinPacking_NF(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE);
		
};

#endif
