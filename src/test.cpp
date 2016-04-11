#include <iostream>
#include <vector>
#include "../include/tasks.h"

using namespace std;

int main()
{
	fraction_t u,d;
	TaskSet taskset = TaskSet();
	
	taskset.add_task(1,2);
	taskset.add_task(2,3);
	taskset.add_task(1,5,3);
	taskset.add_task(1,4,3);
	taskset.add_task(5,10,9);
	taskset.add_task(1,2);
	
	cout<<"Implicit:"<<taskset.is_implicit_deadline()<<endl;
	cout<<"Constraint:"<<taskset.is_constraint_deadline()<<endl;
	cout<<"Arbitary:"<<taskset.is_arbitary_deadline()<<endl;
	cout<<"Utilization_sum:"<<taskset.get_utilization_sum()<<endl;
	cout<<"Utilization_max:"<<taskset.get_utilization_max()<<endl;
	cout<<"Density_sum:"<<taskset.get_density_sum()<<endl;
	cout<<"Density_max:"<<taskset.get_density_max()<<endl;
	return 0;
}
