#include <iostream>
#include <vector>
#include "../include/tasks.h"

using namespace std;

int main()
{
	fraction_t u,d;
	Task t1 = Task(1,2);
	cout<<"WCET:"<<t1.get_wcet()<<endl;
	cout<<"Deadline:"<<t1.get_deadline()<<endl;
	cout<<"Period:"<<t1.get_period()<<endl;
	cout<<"Utilization:"<<t1.get_utilization()<<endl;
	cout<<"Density:"<<t1.get_density()<<endl;
	t1.get_utilization(u);
	t1.get_density(d);
	cout<<"Utilization:"<<u<<endl;
	cout<<"Density:"<<d<<endl;
	return 0;
}
