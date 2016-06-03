#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include "tasks.h"
#include "schedulability_test.h"
#include "processors.h"
#include "mgl_chart.h"
#include "xml.h"

#define MAX_LEN 100
#define MAX_METHOD 6


using namespace std;

void requests_gen();
string output_filename(int lambda, double step, int p_num, Range u_range, Range p_range);
const char* get_method_name(int method);
void	 Scheduling_Test(ResourceSet& resourceset, int lambda, int p_num, Range p_range, Range u_range, double step, int exp_times, double probability, int num_max, Range l_range, double tlfs, Int_Set methods, Result_Set *results);
void Export_Chart(const char* path, const char* title, double min, double max, double step, const char** names, int n, ...);

int main(int argc,char** argv)
{	
	Int_Set lambdas, p_num, methods;
	Double_Set steps;
	Range_Set p_ranges, u_ranges;
	uint exp_times;
	Result_Set results[MAX_METHOD];
	Chart chart;

	//XML xml;	

	XML::LoadFile("config.xml");

	//scheduling parameter
	XML::get_method(&methods);
	exp_times = XML::get_experiment_times();
	XML::get_lambda(&lambdas);
	XML::get_processor_num(&p_num);
	XML::get_period_range(&p_ranges);
	XML::get_utilization_range(&u_ranges);
	XML::get_step(&steps);	

	//resource parameter
	ResourceSet resourceset = ResourceSet();
	Int_Set resource_num, rrns;
	Double_Set rrps, tlfs;
	Range_Set rrrs;
	XML::get_resource_num(&resource_num);
	XML::get_resource_request_probability(&rrps);
	XML::get_resource_request_num(&rrns);
	XML::get_resource_request_range(&rrrs);
	XML::get_total_len_factor(&tlfs);

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
	output_file<<"Utilization,";
	for(uint i = 0; i < methods.size(); i++)
	{
		output_file<<get_method_name(methods[i])<<" ratio,";
	}
	output_file<<"\n";

	double utilization = u_ranges[0].min;

	//while(utilization <= u_ranges[0].max)
	for(;utilization <= u_ranges[0].max ; utilization += steps[0])
	{	
		Result result;
		
		int success[MAX_METHOD] = {0, 0, 0, 0, 0, 0};
		for(int i = 0; i < exp_times; i++)
		{
			TaskSet taskset = TaskSet();
			
			ProcessorSet processorset = ProcessorSet(p_num[0]);
			
			tast_gen(taskset, resourceset, lambdas[0], p_ranges[0], utilization, rrps[0], rrns[0], rrrs[0], tlfs[0]);
			for(uint i = 0; i < methods.size(); i++)
			{
				if(is_schedulable(taskset, processorset, resourceset, methods[i], 0, 0))
				{	
					success[i]++;
				}
			}
			result.x = taskset.get_utilization_sum().get_d();
		
		}
		for(uint i = 0; i < methods.size(); i++)
		{
			fraction_t ratio(success[i], exp_times);
			result.y = ratio.get_d();
			results[i].push_back(result);
		}
		cout<<"U-MAX:"<<u_ranges[0].max<<endl;
		cout<<"utilization:"<<utilization<<endl;
		cout<<u_ranges[0].max-utilization<<endl;
		cout<<steps[0]<<endl;
		if(utilization+steps[0] == u_ranges[0].max)
			cout<<"11111111111111111111"<<endl;
	}

	//Scheduling_Test(resourceset, lambdas[0], p_num[0], p_ranges[0], u_ranges[0], steps[0], exp_times, rrps[0], rrns[0], rrrs[0], tlfs[0], methods, results);
	for(uint i = 0; i < methods.size(); i++)
		chart.AddData(get_method_name(methods[i]), results[i]);
		//results.clear();

	if(0 != methods.size())
	{
		for(uint i = 0; i < results[0].size(); i++)
		{	
			output_file<<results[0][i].x<<",";
			for( uint j = 0; j < methods.size(); j++)
			{
				output_file<<results[j][i].y<<",";
			}
			output_file<<"\n";
		}
	}	
	output_file.flush();
	output_file.close();
	
	string png_name = "results/" + output_filename(lambdas[0], steps[0], p_num[0], u_ranges[0], p_ranges[0]) + ".png";

	chart.SetGraphSize(1280,800);
	chart.SetGraphQual(3);
	chart.ExportPNG(png_name.data(), "", u_ranges[0].min, results[0][results[0].size()-1].x);

	return 0;
}

string output_filename(int lambda, double step, int p_num, Range u_range, Range p_range)
{
	stringstream buf;
	buf<<"l:"<<lambda<<"-"<<"s:"<<step<<"-"<<"P:"<<p_num<<"-"<<fixed<<setprecision(0)<<"u:["<<u_range.min<<","<<u_range.max<<"]-"<<"p:["<<p_range.min<<","<<p_range.max<<"]";
	return buf.str();
}

const char* get_method_name(int method)
{
	const char* name;
	switch(method)
	{
		default:
			name = "P-EDF";
			break;
		case 0:
			name =  "P-EDF";
			break;
		case 1:
			name =  "BCL-FTP";
			break;
		case 2:
			name =  "BCL-EDF";
			break;
		case 3:
			name = "WF-DM";
			break;
		case 4:
			name = "WF-EDF";
			break;
	}
	return name;
}

void Scheduling_Test(ResourceSet& resourceset, int lambda, int p_num, Range p_range, Range u_range, double step, int exp_times, double probability, int num_max, Range l_range, double tlfs, Int_Set methods, Result_Set *results )
{
	double utilization = u_range.min;
	do
	{	
		Result result;
		result.x = utilization;
		int success[MAX_METHOD] = {0, 0, 0, 0, 0, 0};
		for(int i = 0; i < exp_times; i++)
		{
			TaskSet taskset = TaskSet();
			
			ProcessorSet processorset = ProcessorSet(p_num);
			
			tast_gen(taskset, resourceset, lambda, p_range, utilization, probability, num_max, l_range, tlfs);
			for(uint i = 0; i < methods.size(); i++)
			{
				if(is_schedulable(taskset, processorset, resourceset, methods[i], 0, 0))
				{	
					success[i]++;
				}
			}
		
		}
		for(uint i = 0; i < methods.size(); i++)
		{
			fraction_t ratio(success[i], exp_times);
			result.y = ratio.get_d();
			results[i].push_back(result);
		}
		cout<<"utilization:"<<utilization<<endl;
	}while((utilization += step) < u_range.max);
	
}



