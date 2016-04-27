#include <iostream>
#include <vector>
#include "../include/tasks.h"
#include "../include/SchedTest/RMS.h"
#include "../include/processors.h"
#include "../include/random_gen.h"

using namespace std;

int main(int argc,char** argv)
{
	if(3 != argc)
	{
		cout<<"Usage: ./test [positive task number] [positive experiment times]"<<endl;
		return 0;
	}
	
	uint task_num = atoi(argv[1]);
	uint exp_t = atoi(argv[2]);

	if(0 >= task_num || 0 >= exp_t)
	{
		cout<<"Usage: ./test [positive task number] [positive experiment times]"<<endl;
		return 0; 
	}

	//ProcessorSet processorset = ProcessorSet(processor_num);
	
	RMS rms = RMS(1);//uniprocessor
	uint j = 0;
	uint success = 0;
	while(j++ < exp_t)
	{
		TaskSet taskset = TaskSet();
		for(int i = 1; i <= task_num; i++)
		{
			int wcet = int(exponential_gen(5)*100);
			if(0 == wcet)
				wcet++;
			int period = 500;
			taskset.add_task(wcet,period);	
		}
		if(rms.is_RM_schedulable(taskset))
			success++;
	}
	fraction_t rate(success, exp_t);
	
	cout << "RM schedulability rate:" << rate << endl;

}
