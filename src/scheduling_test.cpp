#include <iostream>
#include <vector>
#include "../include/tasks.h"
#include "../include/SchedTest/RMS.h"
#include "../include/processors.h"
#include "../include/random_gen.h"

using namespace std;

int main(int argc,char** argv)
{
	if(2 != argc)
	{
		cout<<"Usage: ./test [positive task number]"<<endl;
		return 0;
	}
	
	uint task_num = atoi(argv[1]);

	if(0 >= task_num)
	{
		cout<<"Usage: ./test [positive task number]"<<endl;
		return 0; 
	}

	//ProcessorSet processorset = ProcessorSet(processor_num);
	TaskSet taskset = TaskSet();
	RMS rms = RMS(1);//uniprocessor
	
	for(int i = 1; i <= task_num; i++)
	{
		int wcet = int(exponential_gen(5)*100);
		if(0 == wcet)
			wcet++;
		int period = 500;
		taskset.add_task(wcet,period);	
	}

	for(int i = 0; i < task_num; i++)
	{
		cout << "Task " << i << ": Utilization: "<< taskset.get_task_utilization(i) << endl;
	}
	cout << fixed;
	cout << "Total utilization:" << taskset.get_utilization_sum().get_d() << endl;
	cout << "RM schedulability:" << (rms.is_RM_schedulable(taskset)?"true":"false") << endl;

}
