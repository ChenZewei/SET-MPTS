#include <iostream>
#include "../include/xml.h"
#include "../include/types.h"

using namespace std;

int main()
{
	config.LoadFile("config.xml");
	int_set lambdas, processors;
	double_set steps;
	range_set u_ranges, p_ranges;
	get_lambda(&lambdas);
	for(int i = 0; i < lambdas.size(); i++)
		cout<<"lambda "<<i<<": "<<lambdas[i]<<endl;
	
	get_processor_num(&processors);
	for(int i = 0; i < processors.size(); i++)
		cout<<"processor "<<i<<": "<<processors[i]<<endl;
	
	get_period_range(&p_ranges);
	for(int i = 0; i < p_ranges.size(); i++)
		cout<<"min "<<i<<": "<<p_ranges[i].min<<" max "<<i<<": "<<p_ranges[i].max<<endl;
	

	get_utilization_range(&u_ranges);
	for(int i = 0; i < u_ranges.size(); i++)
		cout<<"min "<<i<<": "<<u_ranges[i].min<<" max "<<i<<": "<<u_ranges[i].max<<endl;

	get_step(&steps);
	for(int i = 0; i < steps.size(); i++)
		cout<<"step "<<i<<": "<<steps[i]<<endl;
}
