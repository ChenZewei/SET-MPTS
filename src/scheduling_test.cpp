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

	//graph parameters
	Range_Set job_num_ranges;
	Range_Set arc_num_ranges;
	Int_Set is_cyclics;
	Int_Set max_indegrees;
	Int_Set max_outdegrees;
	
	XML::get_ranges(&job_num_ranges, "dag_job_num_range");
	XML::get_ranges(&arc_num_ranges, "dag_arc_num_range");
	XML::get_integers(&is_cyclics, "is_cyclic");
	XML::get_integers(&max_indegrees, "max_indegree");
	XML::get_integers(&max_outdegrees, "max_outdegree");
	

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
	
	parameters.job_num_range = job_num_ranges[0];	
	parameters.arc_num_range = arc_num_ranges[0];	
	if(0 == is_cyclics[0])
		parameters.is_cyclic = false;
	else
		parameters.is_cyclic = true;	
	parameters.max_indegree = max_indegrees[0];	
	parameters.max_outdegree = max_outdegrees[0];


cout<<"////////////////////"<<endl;
cout<<parameters.lambda<<endl;
cout<<parameters.p_num<<endl;
cout<<parameters.step<<endl;
cout<<parameters.p_range.min<<" "<<parameters.p_range.max<<endl;
cout<<parameters.u_range.min<<" "<<parameters.u_range.max<<endl;
cout<<parameters.d_range.min<<" "<<parameters.d_range.max<<endl;
cout<<parameters.exp_times<<endl;
cout<<parameters.job_num_range.min<<" "<<parameters.job_num_range.max<<endl;
cout<<parameters.arc_num_range.min<<" "<<parameters.arc_num_range.max<<endl;
if(parameters.is_cyclic)
	cout<<"true."<<endl;
else
	cout<<"false."<<endl;
cout<<parameters.max_indegree<<endl;
cout<<parameters.max_indegree<<endl;
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
cout<<"1"<<endl;
	dag_task.add_job(1);
	dag_task.add_job(2);
	dag_task.add_job(1);
	dag_task.add_job(1);
	dag_task.add_job(3);
	dag_task.add_job(1);
	dag_task.add_job(2);
	dag_task.add_job(1);
cout<<"2"<<endl;	
	dag_task.add_arc(0,3);
	dag_task.add_arc(1,3);
	dag_task.add_arc(2,4);
	dag_task.add_arc(3,5);
	dag_task.add_arc(3,6);
	dag_task.add_arc(4,6);
	dag_task.add_arc(6,7);
cout<<"3"<<endl;
	dag_task.refresh_relationship();
cout<<"4"<<endl;
	dag_task.display_arcs();
cout<<"5"<<endl;
	for(uint i = 0; i < dag_task.get_vnode_num(); i++)
		dag_task.display_precedences(i);
	
	for(uint i = 0; i < dag_task.get_vnode_num(); i++)
		dag_task.display_follow_ups(i);

	dag_task.update_vol();
	dag_task.update_len();

	cout<<"vol:"<<dag_task.get_vol()<<" "<<"len:"<<dag_task.get_len()<<endl;
	
	
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
			DAG_TaskSet dag_taskset = DAG_TaskSet();
			ProcessorSet processorset = ProcessorSet(parameters);
			ResourceSet resourceset = ResourceSet();
			//ResourceSet2<Task> resourceset2 = ResourceSet2<Task>();
			resource_gen(&resourceset, parameters);
			//resource_gen2<Task>(&resourceset2, parameters);

			tast_gen(taskset, resourceset, parameters, utilization);
			dag_task_gen(dag_taskset, resourceset, parameters, utilization);
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




