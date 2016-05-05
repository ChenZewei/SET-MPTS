#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "tasks.h"
#include "RMS.h"
#include "partitioned_sched.h"
#include "schedulability_test.h"
#include "processors.h"
#include "random_gen.h"
#include "xml.h"
#include "mgl2/mgl.h"

#define MAX_LEN 100


using namespace std;

string output_filename(int lambda, double step, int p_num, range u_range, range p_range);
void tast_gen(TaskSet *taskset, int lambda, range p_range, double u_ceil);

int main(int argc,char** argv)
{
	/*
	if(2 != argc)
	{
		cout<<"Usage: ./test [output file path]"<<endl;
		return 0;
	}
	
	string path = argv[1];
	*/
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
						double_set r;
						string file_name = "results/" + output_filename(lambdas[i], steps[j], processors[l], u_ranges[k], p_ranges[m]) + ".csv";
						ofstream output_file (file_name);
						fraction_t u_ceil = u_ranges[k].min;
						output_file<<"Lambda:"<<lambdas[i]<<" ";					
						output_file<<" processor number:"<<processors[l]<<" ";
						output_file<<" step:"<<steps[j]<<" ";
						output_file<<" utilization range:["<<u_ranges[k].min<<","<<u_ranges[k].max<<"] ";
						output_file<<"period range:["<<p_ranges[m].min<<","<<p_ranges[m].max<<"]\n";
						output_file.flush();
						do
						{
							int x = 0;
							uint success = 0;
							while(x++ < exp_t)
							{
								TaskSet taskset = TaskSet();
								ProcessorSet processorset = ProcessorSet(processors[l]);
								
								tast_gen(&taskset, lambdas[i], p_ranges[m], u_ceil.get_d());
					
								//if(is_schedulable(taskset, processorset,BCL_EDF))
									//success++;
								if(is_Partitioned_EDF_Schedulable(taskset, processorset))
									success++;
							}
							fraction_t ratio(success, exp_t);
							//cout<<"Lambda:"<<lambdas[i]<<" processor number:"<<processors[l]<<" step:"<<steps[j]<<" utilization range:["<<u_ranges[k].min<<","<<u_ranges[k].max<<"] period range:["<<p_ranges[m].min<<","<<p_ranges[m].max<<"] U:"<<u_ceil.get_d()<<" ratio:"<<ratio<<endl; 
							
							output_file<<"Utilization:"<<u_ceil.get_d()<<",";
							output_file<<"Ratio:"<<ratio<<"\n";
							output_file.flush();
							r.push_back(ratio.get_d());

						}while((u_ceil += steps[j]) < u_ranges[k].max);

						output_file.close();
						
						mglGraph gr;
						mglData y(r.size());
						for(int i = 0; i < r.size(); i++)
						{
							y.a[i] = r[i];
						}
						
						//gr.Rotate(50,60);
						//gr.Light(false);
						//gr.Surf(dat);
						//gr.Cont(dat,"y");
						gr.Title("Partitioned EDF");
						gr.SetOrigin(0,0);
						gr.SetRange('x', u_ranges[k].min, u_ranges[k].max);
						gr.SetRange('y', 0, 1);
						gr.Label('x',"x: TaskSet Utilization", 0);
						gr.Label('y',"y: Ratio", 0);
						gr.Plot(y,"b");
						gr.Axis();
						string png_name = "results/" + output_filename(lambdas[i], steps[j], processors[l], u_ranges[k], p_ranges[m]) + ".png";
						gr.WriteFrame(png_name.data());
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

void tast_gen(TaskSet *taskset, int lambda, range p_range, double u_ceil)
{
	while(taskset->get_utilization_sum() < u_ceil)//generate tasks
	{
		long period = uniform_integral_gen(long(p_range.min),long(p_range.max));
		fraction_t u = exponential_gen(lambda);
		long wcet = long(period*u.get_d());
		if(0 == wcet)
			wcet++;
		else if(wcet > period)
			wcet = period;
		fraction_t temp(wcet, period);
		if(taskset->get_utilization_sum() + temp > u_ceil)
		{
			temp = u_ceil - taskset->get_utilization_sum();
			wcet = floor(temp.get_d()*period);
			taskset->add_task(wcet, period);
			break;
		}
		taskset->add_task(wcet,period);	
	}
}
