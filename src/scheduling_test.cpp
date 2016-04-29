#include <iostream>
#include <vector>
#include "../include/tasks.h"
#include "../include/SchedTest/RMS.h"
#include "../include/SchedTest/partitioned_sched.h"
#include "../include/SchedTest/schedulability_test.h"
#include "../include/processors.h"
#include "../include/random_gen.h"

using namespace std;

int main(int argc,char** argv)
{
	if(4 != argc)
	{
		cout<<"Usage: ./test [positive processor number] [positive task number] [positive experiment times]"<<endl;
		return 0;
	}
	
	uint processor_num = atoi(argv[1]);
	uint task_num = atoi(argv[2]);
	uint exp_t = atoi(argv[3]);

	if(0 >= task_num || 0 >= exp_t || 0 >= processor_num)
	{
		cout<<"Usage: ./test [positive processor number] [positive task number] [positive experiment times]"<<endl;
		return 0; 
	}

	ProcessorSet processorset = ProcessorSet(processor_num);
	
	uint j = 0;
	uint RM_success = 0,PEDF_success = 0;;
	while(j++ < exp_t)
	{
		TaskSet taskset = TaskSet();
		for(int i = 1; i <= task_num; i++)
		{
			int period = uniform_integral_gen(100,500);
			fraction_t u = exponential_gen(10);
			int wcet = int(period*u.get_d());
			if(0 == wcet)
				wcet++;
			else if(wcet > period)
				wcet = period;
			taskset.add_task(wcet,period);	
		}
		if(is_RM_schedulable(taskset, processorset))
			RM_success++;
		if(is_Partitioned_EDF_Schedulable(taskset, processorset))
			PEDF_success++;
	}
	fraction_t rate1(RM_success, exp_t);
	fraction_t rate2(PEDF_success, exp_t);
	if(1 == processor_num)
	{
		cout << "RM schedulability rate:" << rate1 << endl;
		cout << "EDF schedulability rate:" << rate2 << endl;
	}
	else
	{
		cout << "Golbal RM schedulability rate:" << rate1 << endl;
		cout << "Partitioned EDF schedulability rate:" << rate2 << endl;
	}

}
