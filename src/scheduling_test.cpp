#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "../include/tasks.h"
#include "../include/SchedTest/RMS.h"
#include "../include/SchedTest/partitioned_sched.h"
#include "../include/SchedTest/schedulability_test.h"
#include "../include/processors.h"
#include "../include/random_gen.h"
#include "../include/xml.h"

#define MAX_LEN 100


using namespace std;

int main(int argc,char** argv)
{
	if(2 != argc)
	{
		cout<<"Usage: ./test [output file path]"<<endl;
		return 0;
	}
	
	string path = argv[1];

	int_set lambdas, processors;
	double_set steps;
	range_set p_ranges, u_ranges;
	uint exp_t;
	
	config.LoadFile("config.xml");

	exp_t = get_experiment_times();
	get_lambda(&lambdas);
	get_processor_num(&processors);
	get_period_range(&p_ranges);
	get_utilization_range(&u_ranges);
	get_step(&steps);


	for(int i = 0; i < lambdas.size(); i++)
	{
		for(int j = 0; j < steps.size(); j++)
		{
			for(int k = 0; k < u_ranges.size(); k++)
			{
				for(int l = 0; l < processors.size(); l++)
				{
					for(int m = 0; m < p_ranges.size(); m++)
					{
						fstream fs;
						fs.open(path, fstream::in|fstream::out|fstream::app);
						fraction_t u_ceil = u_ranges[k].min;

						do
						{
							int x = 0;
							uint success = 0;
							char *buffer = new char[MAX_LEN];
							while(x++ < exp_t)
							{
								TaskSet taskset = TaskSet();
								ProcessorSet processorset = ProcessorSet(processors[l]);
								
								while(taskset.get_utilization_sum() < u_ceil)//generate tasks
								{
									int period = uniform_integral_gen(int(p_ranges[m].min),int(p_ranges[m].max));
									fraction_t u = exponential_gen(lambdas[i]);
									int wcet = int(period*u.get_d());
									if(0 == wcet)
										wcet++;
									else if(wcet > period)
										wcet = period;
									fraction_t temp(wcet, period);
									if(taskset.get_utilization_sum() + temp > u_ceil)
									{
										temp = u_ceil - taskset.get_utilization_sum();
										wcet = floor(temp.get_d()*period);
										taskset.add_task(wcet, period);
										break;
									}
									taskset.add_task(wcet,period);	
								}
								if(is_Partitioned_EDF_Schedulable(taskset, processorset))
									success++;
							}
							fraction_t ratio(success, exp_t);	
							//int i = sprintf(buffer,"lambda:%d\tstep:%s\tutilization range:[%s,%s]\tprocessor number:%d\tperiod range:[%d,%d]\tschedulability ratio:%s",&lambdas[i],&steps[j].get_str(),&u_ranges[k].min.get_str(),&u_ranges[k].max.get_str(),&processors[l],&p_ranges[m].min.get_str(),&p_ranges[m].max.get_str(),&ratio.get_str());
							//cout<<lambdas[i]<<endl;
							cout<<"Lambda:"<<lambdas[i]<<" processor number:"<<processors[l]<<" step:"<<steps[j]<<" utilization range:["<<u_ranges[k].min<<","<<u_ranges[k].max<<"] period range:["<<p_ranges[m].min<<","<<p_ranges[m].max<<"] U:"<<u_ceil.get_d()<<" ratio:"<<ratio<<endl;
/*				
							stringstream buf;
							buf<<"lambda2:"<<lambdas[i];
							string str;buf >> str;							
							cout<<str<<endl;
							fs.write(buffer,i);
*/
							delete[] buffer;

						}while((u_ceil += steps[j]) < u_ranges[k].max);



						fs.close();
					}
				}
			}
		}
	}

}
