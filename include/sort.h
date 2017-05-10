#ifndef SORT_H
#define SORT_H

//#include "tasks.h"
//#include "types.h"

#define PRIORITY 0

#define INCREASE 0
#define DECREASE 1

//class Task;
//class DAG_Task;
//typedef struct ArcNode;

template <typename TaskModel>
int period_increase(TaskModel t1, TaskModel t2)
{
	return t1.get_period() < t2.get_period();
}

template <typename TaskModel>
int deadline_increase(TaskModel t1, TaskModel t2)
{
	return t1.get_deadline() < t2.get_deadline();
}

template <typename TaskModel>
int utilization_decrease(TaskModel t1, TaskModel t2)
{
	return t1.get_utilization() > t2.get_utilization();
}

template <typename TaskModel>
int density_decrease(TaskModel t1, TaskModel t2)
{
	return t1.get_density() > t2.get_density();
}

template <typename ANode>
int arcs_increase(ANode a1, ANode a2)
{
	if(a1.tail != a2.tail)
		return a1.tail < a2.tail;
	else
		return a1.head < a2.head;
}

template <typename Model>
int task_utilization_increase(Model m1, Model m2)
{
	return m1.get_utilization() < m2.get_utilization();
}

template <typename Model>
int task_utilization_decrease(Model m1, Model m2)
{
	return m1.get_utilization() > m2.get_utilization();
}

template <typename Model>
int task_DC_increase(Model m1, Model m2)
{
	return (m1.get_deadline() - m1.get_wcet()) < (m2.get_deadline() - m2.get_wcet());
}

template <typename Model>
int task_DCC_increase(Model m1, Model m2)
{
	return (m1.get_deadline() - m1.get_wcet())*m1.get_wcet() < (m2.get_deadline() - m2.get_wcet())*m2.get_wcet();
}

template <typename Model>
int resource_utilization_increase(Model m1, Model m2)
{
	return m1.get_utilization() < m2.get_utilization();
}

template <typename Model>
int resource_utilization_decrease(Model m1, Model m2)
{
	return m1.get_resource_utilization() > m2.get_resource_utilization();
}


#endif
