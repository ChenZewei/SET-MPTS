#ifndef DAG_GEDF_BON_H
#define DAG_GEDF_BON_H

/*
**
** 
**
*/

#include "g_sched.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

typedef struct Work
{
	ulong time;
	ulong workload;
	bool operator <(const Work &a)const //排序并且去重复
    {  
        if(time == a.time)  
        {  
            return workload < a.workload;   
        }  
        else return time < a.time;  
    }
}Work;

typedef struct
{
	ulong r_t;
	ulong f_t;
}Exec_Range;

template <typename Type>
Type set_member(set<Type> s, int index)
{
	typename std::set<Type>::iterator it = s.begin();
	for(int i = 0; i < index; i++)
		it++;
	return *(it);
}

class DAG_GEDF_BON: public GlobalSched
{
	private:
		DAG_TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;
		double e;

		set<Work> precise_workload(DAG_Task &dag_task, ulong T);
		ulong approximate_workload(DAG_Task &dag_task, ulong T);
		double approximation(DAG_TaskSet &dag_taskset, double e);
	public:
		DAG_GEDF_BON();
		DAG_GEDF_BON(DAG_TaskSet tasks, ProcessorSet processors, ResourceSet resources, double e = 0.1);
		bool is_schedulable();

};

#endif
