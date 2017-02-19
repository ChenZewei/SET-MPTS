#ifndef FACTORY_BASE_H
#define FACTORY_BASE_H

#include "sched_test_base.h"

class FactoryBase
{
	public:
		virtual SchedTestBase* createSchedTest() = 0;
}

#endif
