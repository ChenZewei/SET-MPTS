#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include "tasks.h"
#include "schedulability_test.h"
#include "processors.h"
#include "mgl_chart.h"
#include "xml.h"
#include "param.h"
#include "output.h"

#define MAX_LEN 100
#define MAX_METHOD 8


using namespace std;

int main(int argc,char** argv)
{	
	Int_Set lambdas, p_num, methods;
	Double_Set steps;
	Range_Set p_ranges, u_ranges, d_ranges;
	Test_Attribute_Set test_attributes;
	uint exp_times;
	Result_Set results[MAX_METHOD];
	Chart chart;
	Param parameters;

	XML::LoadFile("config.xml");

	if(0 == access(string("results").data(), 0))
		printf("results folder exsists.\n");
	else
	{
		printf("results folder does not exsist.\n");
		if(0 == mkdir(string("results").data(), S_IRWXU))
			printf("results folder has been created.\n");
		else
			return 0;
	}
cout<<"111111111111"<<endl;
	//scheduling parameter
	XML::get_method(&test_attributes);
	exp_times = XML::get_experiment_times();
	XML::get_lambda(&lambdas);
	XML::get_processor_num(&p_num);
	XML::get_period_range(&p_ranges);
	XML::get_deadline_propotion(&d_ranges);
	XML::get_utilization_range(&u_ranges);
	XML::get_step(&steps);	
cout<<"222222222222"<<endl;
	//resource parameter
	Int_Set resource_num, rrns;
	Double_Set rrps, tlfs;
	Range_Set rrrs;
	XML::get_resource_num(&resource_num);
	XML::get_resource_request_probability(&rrps);
	XML::get_resource_request_num(&rrns);
	XML::get_resource_request_range(&rrrs);
	XML::get_total_len_factor(&tlfs);

cout<<lambdas[0]<<endl;
cout<<p_num[0]<<endl;
cout<<steps[0]<<endl;
cout<<p_ranges[0].min<<" "<<p_ranges[0].max<<endl;
cout<<u_ranges[0].min<<" "<<u_ranges[0].max<<endl;
cout<<d_ranges[0].min<<" "<<d_ranges[0].max<<endl;
cout<<exp_times<<endl;

	//set parameters
	parameters.lambda = lambdas[0];
	parameters.p_num = p_num[0];
	parameters.step = steps[0];
	parameters.p_range = p_ranges[0];
	parameters.u_range = u_ranges[0];
	parameters.d_range =  d_ranges[0];
	parameters.test_attributes = test_attributes;
	parameters.exp_times = exp_times;
	parameters.resource_num = resource_num[0];
	parameters.rrn = rrns[0];
	parameters.rrp = rrps[0];
	parameters.tlf = tlfs[0];
	parameters.rrr = rrrs[0];
cout<<"////////////////////"<<endl;
cout<<parameters.lambda<<endl;
cout<<parameters.p_num<<endl;
cout<<parameters.step<<endl;
cout<<parameters.p_range.min<<" "<<parameters.p_range.max<<endl;
cout<<parameters.u_range.min<<" "<<parameters.u_range.max<<endl;
cout<<parameters.d_range.min<<" "<<parameters.d_range.max<<endl;
cout<<parameters.exp_times<<endl;
cout<<"////////////////////"<<endl;


	Output output(parameters);

cout<<output.output_filename()<<endl;

/*
	string file_name = "results/" + parameters.output_filename() + ".csv";
	ofstream output_file(file_name);

	output_file<<"Lambda:"<<lambdas[0]<<",";					
	output_file<<" processor number:"<<p_num[0]<<",";
	output_file<<" step:"<<steps[0]<<",";
	output_file<<" utilization range:["<<u_ranges[0].min<<"-"<<u_ranges[0].max<<"],";
	output_file<<setprecision(0)<<" period range:["<<p_ranges[0].min<<"-"<<p_ranges[0].max<<"]\n"<<setprecision(3);
	output_file<<"Utilization,";
	for(uint i = 0; i < test_attributes.size(); i++)
	{
		output_file<<get_method_name(test_attributes[i])<<" ratio,";
	}
	output_file<<"\n";
*/
	
cout<<"////////////////DAG//////////////////"<<endl;
	DAG_Task dag_task(1, 4);

	dag_task.add_job(1);
	dag_task.add_job(1);
	dag_task.add_job(2);
	dag_task.add_job(1);
	dag_task.add_job(1);
	
	dag_task.add_arc(0,2);
	dag_task.add_arc(1,2);
	dag_task.add_arc(2,3);
	dag_task.add_arc(2,4);

	dag_task.refresh_relationship();

	dag_task.display_arcs();

	dag_task.display_precedences(0);
	dag_task.display_precedences(1);
	dag_task.display_precedences(2);
	dag_task.display_precedences(3);
	dag_task.display_precedences(4);

	dag_task.display_follow_ups(0);
	dag_task.display_follow_ups(1);
	dag_task.display_follow_ups(2);
	dag_task.display_follow_ups(3);
	dag_task.display_follow_ups(4);

	dag_task.update_vol();
	//dag_task.update_len();

	//cout<<"vol:"<<dag_task.get_vol()<<" "<<"len:"<<dag_task.get_len()<<endl;
	
	
	double utilization = u_ranges[0].min;
	
	do
	{	
		Result result;
		cout<<"Utilization:"<<utilization<<endl;
		vector<int> success;
		for(uint i = 0; i < test_attributes.size(); i++)
		{
			success.push_back(0);
		}
		for(int i = 0; i < exp_times; i++)
		{
			TaskSet taskset = TaskSet();
				
			ProcessorSet processorset = ProcessorSet(parameters);
			ResourceSet resourceset = ResourceSet();
			resource_gen(&resourceset, parameters);
			tast_gen(taskset, resourceset, parameters, utilization);

			for(uint i = 0; i < parameters.get_method_num(); i++)
			{
				taskset.init();
				processorset.init();
				
				if(is_schedulable(taskset, processorset, resourceset, parameters.get_test_method(i), parameters.get_test_type(i), 0))
				{	
					success[i]++;
				}
			}
			result.x = taskset.get_utilization_sum().get_d();
		}
		for(uint i = 0; i < test_attributes.size(); i++)
		{
			fraction_t ratio(success[i], exp_times);
			result.y = ratio.get_d();
			output.add_result(i, result.x, result.y);
		}

//output to csv file		
/*
		output_file<<utilization<<",";
		for(uint i = 0; i < test_attributes.size(); i++)
		{
			output_file<<results[i][results[i].size()-1].y<<",";
		}
		output_file<<"\n";
		output_file.flush();
*/
		utilization += steps[0];

	}
	while(utilization < u_ranges[0].max || fabs(u_ranges[0].max - utilization) < EPS);

	output.export_csv();

	output.export_line_chart();

/*
	for(uint i = 0; i < test_attributes.size(); i++)
	{
		string test_name = get_method_name(test_attributes[i]);
		chart.AddData(test_name, results[i]);
	}

	output_file.close();
	
	string png_name = "results/" + output_filename(lambdas[0], steps[0], p_num[0], u_ranges[0], p_ranges[0]) + ".png";

	chart.SetGraphSize(1280,640);
	chart.SetGraphQual(3);
	chart.ExportPNG(png_name.data(), "", u_ranges[0].min, results[0][results[0].size()-1].x);
*/
	return 0;
}




