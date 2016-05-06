#ifndef RTA_FP_UNIPROCESSOR_H
#define RTA_FP_UNIPROCESSOR_H

#include <math.h>
#include <math-helper.h>
#include <algorithm>
#include <iostream>

#include "../tasks.h"

using namespace std;

ulong interf_standard(const Task &taskh, ulong interval)
{
	return taskh.get_wcet() * int(ceiling((interval + taskh.get_response_time() - taskh.get_wcet()) / task.get_period()))
}

ulong rta_standard(const TaskSet &taskset, const Task &task)
{
	test_end = task.get_deadline();
	test_start = task.get_total_blocking() + task.get_jitter() + task.get_wcet();

    # see if we find a point where the demand is satisfied
    time = sum([t.cost for t in higher_prio]) + own_demand
    while time <= test_end:
        demand = sum([fp_demand(t, time) for t in higher_prio]) \
            + own_demand
        if demand == time:
            # yep, demand will be met by time
            task.response_time = time
            return True
        else:
            # try again
            time = demand

    # if we get here, we didn't converge
    return False	
}
#endif
