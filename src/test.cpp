#include <iostream>
#include <vector>
#include "../include/tasks.h"
#include "../include/SchedTest/RMS.h"
#include "../include/processors.h"

using namespace std;

int main(int argc,char** argv)
{
	if(3 != argc)
	{
		cout<<"Usage: ./test [positive processor number] [positive task number]"<<endl;
		return 0;
	}
	
	uint processor_num = atoi(argv[1]);
	uint task_num = atoi(argv[2]);

	if(0 >= task_num || 0 >= processor_num)
	{
		cout<<"Usage: ./test [positive processor number] [positive task number]"<<endl;
		return 0; 
	}

	ProcessorSet processorset = ProcessorSet(processor_num);
	TaskSet taskset = TaskSet();
	RMS rms = RMS(1);
	for(int i = 1; i <= task_num; i++)
	{
		taskset.add_task(1,i+2);	
	}
	
	cout<<"Implicit:"<<taskset.is_implicit_deadline()<<endl;
	cout<<"Constraint:"<<taskset.is_constraint_deadline()<<endl;
	cout<<"Arbitary:"<<taskset.is_arbitary_deadline()<<endl;
	cout<<"Utilization_sum:"<<taskset.get_utilization_sum()<<"\t"<<taskset.get_utilization_sum().get_d()<<endl;
	cout<<"Utilization_max:"<<taskset.get_utilization_max()<<"\t"<<taskset.get_utilization_max().get_d()<<endl;
	cout<<"Density_sum:"<<taskset.get_density_sum()<<"\t"<<taskset.get_density_sum().get_d()<<endl;
	cout<<"Density_max:"<<taskset.get_density_max()<<"\t"<<taskset.get_density_max().get_d()<<endl;

	cout<<"Processor number:"<<processorset.get_processor_num()<<endl;

	cout<<"Utilization bound:"<<rms.get_utilization_bound(taskset)<<"\t"<<rms.get_utilization_bound(taskset).get_d()<<endl;
	cout<<"Uniprocessor Rate Monotonic Schedulability:"<<rms.is_RM_schedulable(taskset)<<endl;
	return 0;
}
