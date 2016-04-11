#include <iostream>
#include <vector>
#include "../include/tasks.h"

using namespace std;

int main()
{
	Task t1 = Task(1,2,3);
	cout<<"WCET:"<<t1.get_wcet()<<endl;
	cout<<"Deadline:"<<t1.get_deadline()<<endl;
	cout<<"Period:"<<t1.get_period()<<endl;
	cout<<"Utilization:"<<t1.get_utilization()<<endl;
	cout<<"Density:"<<t1.get_density()<<endl;
	return 0;
}
