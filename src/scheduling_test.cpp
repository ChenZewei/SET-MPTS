#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "tasks.h"
#include "RMS.h"
#include "bcl.h"
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
Result_Set Scheduling_Test(ResourceSet* resourceset, int lambda, int p_num, Range p_range, Range u_range, double step, int exp_times, double probability, int num_max, Range l_range, double tlfs, int TEST_METHOD = 0);
void Export_Chart(const char* path, const char* title, double min, double max, double step, const char** names, int n, ...);

int main(int argc,char** argv)
{	
	Int_Set lambdas, p_num;
	Double_Set steps;
	Range_Set p_ranges, u_ranges;
	uint exp_times;
	Result_Set results_1, results_2, results_3, results_4;
	Chart chart;
	config.LoadFile("config.xml");
	
	/*
	probability(1.25);

	for(int i = 0; i < 100; i++)
	{
		cout<<uniform_integral_gen(0,10)<<" ";
	}
	*/
	//scheduling parameter
	exp_times = get_experiment_times();
	get_lambda(&lambdas);
	get_processor_num(&p_num);
	get_period_range(&p_ranges);
	get_utilization_range(&u_ranges);
	get_step(&steps);
	
	//Random_Gen  r;
	cout<<"r1:"<<endl;
	for(int i = 0; i < 20; i++)
	{
		cout<<Random_Gen::uniform_integral_gen(1,10)<<" ";
	}
	cout<<endl;
	cout<<"r1-2:"<<endl;
	for(int i = 0; i < 20; i++)
	{
		cout<<Random_Gen::uniform_integral_gen(1,10)<<" ";
	}
	cout<<endl;
	cout<<"r2:"<<endl;
	for(int i = 0; i < 20; i++)
	{
		cout<<Random_Gen::uniform_integral_gen(1,10)<<" ";
	}
	cout<<endl;
	
	
	//resource parameter
	ResourceSet resourceset = ResourceSet();
	Int_Set resource_num, rrns;
	Double_Set rrps, tlfs;
	Range_Set rrrs;
	get_resource_num(&resource_num);
	get_resource_request_probability(&rrps);
	get_resource_request_num(&rrns);
	get_resource_request_range(&rrrs);
	get_total_len_factor(&tlfs);

	resource_gen(&resourceset, resource_num[0]);

	//system("mkdir results");

	string file_name = "results/" + output_filename(lambdas[0], steps[0], p_num[0], u_ranges[0], p_ranges[0]) + ".csv";
	ofstream output_file (file_name);
	fraction_t u_ceil = u_ranges[0].min;
	output_file<<"Lambda:"<<lambdas[0]<<",";					
	output_file<<" processor number:"<<p_num[0]<<",";
	output_file<<" step:"<<steps[0]<<",";
	output_file<<" utilization range:["<<u_ranges[0].min<<"-"<<u_ranges[0].max<<"] ";
	output_file<<"period range:["<<p_ranges[0].min<<"-"<<p_ranges[0].max<<"]\n";
	output_file<<"Utilization,"<<"P_EDF_ratio,"<<"G_EDF_ratio\n";

	results_1 = Scheduling_Test(&resourceset, lambdas[0], 2, p_ranges[0], u_ranges[0], steps[0], exp_times, rrps[0], rrns[0], rrrs[0], tlfs[0], 0);
	results_2 = Scheduling_Test(&resourceset, lambdas[0], 2, p_ranges[0], u_ranges[0], steps[0], exp_times, rrps[0], rrns[0], rrrs[0], tlfs[0], 1);
	//results_3 = Scheduling_Test(lambdas[0], 8, p_ranges[0], u_ranges[0], steps[0], exp_times, 0);
	//results_4 = Scheduling_Test(lambdas[0], 16, p_ranges[0], u_ranges[0], steps[0], exp_times, 0);



	for(int i = 0; i < results_1.size(); i++)
	{
		output_file<<results_1[i].x<<","<<results_1[i].y<<","<<results_2[i].y<<"\n";
	}
	output_file.flush();
	output_file.close();
	string png_name = "results/" + output_filename(lambdas[0], steps[0], p_num[0], u_ranges[0], p_ranges[0]) + ".png";
	const char *name[] = {"P-EDF","G-EDF", "8 processors", "16 processors"};

	chart.AddData(name[0], results_1);
	chart.AddData(name[1], results_2);
	//chart.AddData(name[2], results_3);
	//chart.AddData(name[3], results_4);
	chart.SetGraphSize(1280,800);
	chart.SetGraphQual(3);
	chart.ExportPNG(png_name.data(), "P-EDF VS G-EDF based on BCL", u_ranges[0].min, u_ranges[0].max);
	
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

Result_Set Scheduling_Test(ResourceSet* resourceset, int lambda, int p_num, Range p_range, Range u_range, double step, int exp_times, double probability, int num_max, Range l_range, double tlfs, int TEST_METHOD )
{
	Result_Set results;
	double utilization = u_range.min;
	BCL_Test b_test = BCL_Test(5);
	//Schedulability_Test *s_test = &b_test;

	do
	{	
		Result result;
		result.x = utilization;
		int success = 0;
		for(int i = 0; i < exp_times; i++)
		{
			TaskSet taskset = TaskSet();
			
			ProcessorSet processorset = ProcessorSet(p_num);
			
			tast_gen(&taskset, resourceset, lambda, p_range, utilization, probability, num_max, l_range, tlfs);

			//tast_gen(&taskset, lambda, p_range, utilization);
			//if(is_schedulable(taskset, processorset,BCL_EDF))
				//success++;
			switch(TEST_METHOD)
			{
				case 0:
					if(is_Partitioned_EDF_Schedulable(taskset, processorset))
					//if(b_test.is_Schedulable(taskset, processorset, TEST_METHOD))
					//if(is_RM_schedulable(taskset))
						success++;
					break;
				case 1:
					//if(is_EDF_schedulable(taskset))
					//if(is_schedulable(taskset, processorset, 1))
					if(b_test.is_Schedulable(taskset, processorset, TEST_METHOD))
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



