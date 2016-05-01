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

string output_filename(int lambda, double step, int p_num, range u_range, range p_range);

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
						string file_name = "results/" + output_filename(lambdas[i], steps[j], processors[l], u_ranges[k], p_ranges[m]) + path;
						fs.open(file_name, fstream::in|fstream::out|fstream::app);
						fraction_t u_ceil = u_ranges[k].min;

						do
						{
							int x = 0;
							uint success = 0;
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
							//cout<<"Lambda:"<<lambdas[i]<<" processor number:"<<processors[l]<<" step:"<<steps[j]<<" utilization range:["<<u_ranges[k].min<<","<<u_ranges[k].max<<"] period range:["<<p_ranges[m].min<<","<<p_ranges[m].max<<"] U:"<<u_ceil.get_d()<<" ratio:"<<ratio<<endl; 
							stringstream buf;
							string str;
							buf<<"Lambda:"<<lambdas[i]<<" ";					
							buf<<" processor number:"<<processors[l]<<" ";
							buf<<" step:"<<steps[j]<<" ";
							buf<<" utilization range:["<<u_ranges[k].min<<","<<u_ranges[k].max<<"] ";
							buf<<"period range:["<<p_ranges[m].min<<","<<p_ranges[m].max<<"] ";
							buf<<"U:"<<u_ceil.get_d()<<" ";	
							buf<<" ratio:"<<ratio<<"\n";					
							fs.write(buf.str().data(),buf.str().length());
							buf.flush();


						}while((u_ceil += steps[j]) < u_ranges[k].max);

						fs.close();
					}
				}
			}
		}
	}

}

string output_filename(int lambda, double step, int p_num, range u_range, range p_range)
{
	stringstream buf;
	buf<<"l:"<<lambda<<"-"<<"s:"<<step<<"-"<<"P:"<<p_num<<"-"<<"u:["<<u_range.min<<","<<u_range.max<<"]-"<<"p:["<<p_range.min<<","<<p_range.max<<"]-";
	return buf.str();
}
