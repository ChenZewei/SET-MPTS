#include "g_sched.h"

GlobalSched::GlobalSched(uint TestMethod, uint PriorityAssignment, uint LockingProtocol, string name, string remark): SchedTestBase(false, TestMethod, GLOBAL, PriorityAssignment, LockingProtocol, name, remark)
{
	
}
