#include "lp_sched.h"

LPSched::LPSched(uint TestMethod, uint SchedMethod, uint PriorityAssignment, uint LockingProtocol = NONE, string name = "", string remark = ""): SchedTestBase(true, TestMethod, SchedMethod, PriorityAssignment, LockingProtocol, name, remark) {}
