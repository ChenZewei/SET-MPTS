#ifndef P_SCHED_H
#define P_SCHED_H

#include "types.h"

class PartitionedSched: public SchedTestBase
{
	private:
		
	public:
		enum
		{
			UNTEST,
			TEST
		};
		//Bin packing
		void BinPacking_WF(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE);
		void BinPacking_BF(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE);
		void BinPacking_FF(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE);
		void BinPacking_NF(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, uint MODE);
		
};

#endif
