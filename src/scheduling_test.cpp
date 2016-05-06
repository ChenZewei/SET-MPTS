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

void requests_gen();
string output_filename(int lambda, double step, int p_num, Range u_range, Range p_range);
Result_Set Scheduling_Test(int lambda, int p_num, Range p_range, Range u_range, double step, int exp_times, int TEST_METHOD = 0);
void Export_Chart(const char* path, const char* title, double min, double max, double step, const char** names, int n, ...);

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
		output_file<<results_1[i].x<<","<<results_1[i].y<<","<<results_2[i].y<<"\n";
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
	buf<<"l:"<<lambda<<"-"<<"s:"<<step<<"-"<<"P:"<<p_num<<"-"<<"u:["<<u_range.min<<","<<u_range.max<<"]-"<<"p:["<<p_range.min<<","<<p_range.max<<"]";
	return buf.str();
}

void requests_gen()
{

}

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



