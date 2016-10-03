#ifndef SORT_H
#define SORT_H

#include "tasks.h"

#define PRIORITY 0

#define INCREASE 0
#define DECREASE 1

class Task;
class DAG_Task;

template <typename TaskModel>
int period_increase(TaskModel t1, TaskModel t2)
{
	return t1.get_period() < t2.get_period();
}

template <typename TaskModel>
int utilization_decrease(TaskModel t1, TaskModel t2)
{
	return t1.get_utilization() > t2.get_utilization();
}

#endif
