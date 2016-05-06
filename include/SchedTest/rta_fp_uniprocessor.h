#ifndef RTA_FP_UNIPROCESSOR_H
#define RTA_FP_UNIPROCESSOR_H

#include <math.h>
#include <math-helper.h>
#include <algorithm>
#include <iostream>

#include "../tasks.h"

using namespace std;

ulong interf_standard(const TaskSet tasks, uint t_id, ulong interval)
{
	return tasks[t_id].get_wcet() * int(ceiling((interval + tasks[t_id].get_response_time() - tasks[t_id].get_wcet()) / tasks[t_id].get_period()))
}

ulong rta_standard(const TaskSet tasks, uint t_id)
{
	test_end = tasks[t_id].get_deadline();
	test_start = tasks[t_id].get_total_blocking() + tasks[t_id].get_jitter() + tasks[t_id].get_wcet();

}
#endif
