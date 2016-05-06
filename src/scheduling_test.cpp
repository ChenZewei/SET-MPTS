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
//#include "random_gen.h"
#include "mgl_chart.h"
#include "xml.h"

#define MAX_LEN 100


using namespace std;

<<<<<<< HEAD
string output_filename(int lambda, double step, int p_num, range u_range, range p_range);
void task_gen(TaskSet *taskset, int lambda, range p_range, double u_ceil);
void requests_gen();
=======
string output_filename(int lambda, double step, int p_num, Range u_range, Range p_range);
void tast_gen(TaskSet *taskset, int lambda, Range p_range, double u_ceil);
Result_Set Scheduling_Test(int lambda, int p_num, Range p_range, Range u_range, double step, int exp_times, int TEST_METHOD = 0);
void Export_Chart(const char* path, const char* title, double min, double max, double step, const char** names, int n, ...);
>>>>>>> modulate each sections.

int main(int argc,char** argv)
{
	Int_Set lambdas, p_num;
	Double_Set steps;
	Range_Set p_ranges, u_ranges;
	int exp_times;
	Result_Set results_1, results_2;
	Chart chart;
	config.LoadFile("config.xml");

	exp_times = get_experiment_times();
	get_lambda(&lambdas);
	get_processor_num(&p_num);
	get_period_range(&p_ranges);
	get_utilization_range(&u_ranges);
	get_step(&steps);

	string file_name = "results/" + output_filename(lambdas[0], steps[0], p_num[0], u_ranges[0], p_ranges[0]) + ".csv";
	ofstream output_file (file_name);
	fraction_t u_ceil = u_ranges[0].min;
	output_file<<"Lambda:"<<lambdas[0]<<",";					
	output_file<<" processor number:"<<p_num[0]<<",";
	output_file<<" step:"<<steps[0]<<",";
	output_file<<" utilization range:["<<u_ranges[0].min<<"-"<<u_ranges[0].max<<"] ";
	output_file<<"period range:["<<p_ranges[0].min<<"-"<<p_ranges[0].max<<"]\n";
	output_file<<"Utilization,"<<"P_EDF_ratio,"<<"G_EDF_ratio\n";
	
	results_1 = Scheduling_Test(lambdas[0], p_num[0], p_ranges[0], u_ranges[0], steps[0], exp_times, 0);
	results_2 = Scheduling_Test(lambdas[0], p_num[0], p_ranges[0], u_ranges[0], steps[0], exp_times, 1);
	for(int i = 0; i < results_1.size(); i++)
	{
<<<<<<< HEAD
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
								
								task_gen(&taskset, lambdas[i], p_ranges[m], u_ceil.get_d());
					
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
=======
		output_file<<results_1[i].x<<","<<results_1[i].y<<","<<results_2[i].y<<"\n";
>>>>>>> modulate each sections.
	}
	output_file.flush();
	output_file.close();
	string png_name = "results/" + output_filename(lambdas[0], steps[0], p_num[0], u_ranges[0], p_ranges[0]) + ".png";
	const char *name[] = {"P-EDF","G-EDF"};

	chart.AddData(name[0], results_1);
	chart.AddData(name[1], results_2);
	chart.SetGraphSize(1280,800);
	chart.SetGraphQual(3);
	chart.ExportPNG(png_name.data(), "P-EDF VS G-EDF", u_ranges[0].min, u_ranges[0].max);
	
	//Export_Chart(png_name.data(), "P-EDF VS G-EDF", u_ranges[0].min, u_ranges[0].max, steps[0], name, 2, results_1, results_2);
	return 0;
}

string output_filename(int lambda, double step, int p_num, Range u_range, Range p_range)
{
	stringstream buf;
	buf<<"l:"<<lambda<<"-"<<"s:"<<step<<"-"<<"P:"<<p_num<<"-"<<"u:["<<u_range.min<<","<<u_range.max<<"]-"<<"p:["<<p_range.min<<","<<p_range.max<<"]-";
	return buf.str();
}

<<<<<<< HEAD
<<<<<<< HEAD
void task_gen(TaskSet *taskset, int lambda, range p_range, double u_ceil)
=======
void tast_gen(TaskSet *taskset, int lambda, Range p_range, double u_ceil)
>>>>>>> modulate each sections.
{
	while(taskset->get_utilization_sum() < u_ceil)//generate tasks
	{
		long period = uniform_integral_gen(long(p_range.min),long(p_range.max));
		fraction_t u = exponential_gen(lambda);
		requests_gen();
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

<<<<<<< HEAD
void requests_gen()
{

}
=======
=======
>>>>>>> implement class Chart to draw line plot using mathGL.
Result_Set Scheduling_Test(int lambda, int p_num, Range p_range, Range u_range, double step, int exp_times, int TEST_METHOD)
{
	Result_Set results;
	double utilization = u_range.min;
	
	do
	{	
		Result result;
		result.x = utilization;
		int success = 0;
		for(int i = 0; i < exp_times; i++)
		{
			TaskSet taskset = TaskSet();
			
			ProcessorSet processorset = ProcessorSet(p_num);
			
			tast_gen(&taskset, lambda, p_range, utilization);

			//if(is_schedulable(taskset, processorset,BCL_EDF))
				//success++;
			switch(TEST_METHOD)
			{
				case 0:
					if(is_Partitioned_EDF_Schedulable(taskset, processorset))
						success++;
					break;
				case 1:
					if(is_schedulable(taskset, processorset, 1))
						success++;
					break;
			}
			
		}

		fraction_t ratio(success, exp_times);
		result.y = ratio.get_d();
		results.push_back(result);
	}while((utilization += step) < u_range.max);
	
	return results;
}

/*
void Export_Chart(const char* path, const char* title, double min, double max, double step, const char** names, int n, ...)//up to 6
{
	va_list arg_ptr;
	va_start (arg_ptr,n);
	vector<Result_Set> result_sets;
	for(int i = 0; i < n; i++)
	{	
		Result_Set temp = va_arg(arg_ptr,Result_Set);
		result_sets.push_back(temp);
	}
	va_end(arg_ptr);
	const char *line_style[] = {"r*","bo","g+","cs","yd","m^"};
	mglGraph gr;	
	gr.SetSize(1280,800);
	gr.SetQuality(3);
	mglData y[2];
	y[0] = mglData(result_sets[0].size());
	y[1] = mglData(result_sets[1].size());
	char color[] = {'r','g','b'};
	char dot[] = {'o','+','x'};
	gr.Title(title);
	gr.SetOrigin(0,0,0);
	gr.SetRange('x', min, max + 0.2);
	gr.SetTicks('x', 1, 4);
	gr.SetRange('y', 0, 1.2);	
	
	for(int i = 0; i < result_sets.size(); i++)
	{
		for(int j = 0; j < result_sets[i].size(); j++)
		{
			y[i].a[j] = result_sets[i][j].y;
		}
		gr.Plot(y[i],line_style[i]);
		gr.AddLegend(names[i],line_style[i]);
	}

	gr.Box();
	gr.Label('x',"x: TaskSet Utilization", 0);
	gr.Label('y',"y: Ratio", 0);
	gr.Legend();
	gr.Axis("xy");
	gr.WritePNG(path);
}
*/















<<<<<<< HEAD

>>>>>>> modulate each sections.
=======
>>>>>>> implement class Chart to draw line plot using mathGL.
