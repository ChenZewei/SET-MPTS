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

	//scheduling parameter
	XML::get_method(&test_attributes);
	exp_times = XML::get_experiment_times();
	XML::get_lambda(&lambdas);
	XML::get_processor_num(&p_num);
	XML::get_period_range(&p_ranges);
	XML::get_deadline_propotion(&d_ranges);
	XML::get_utilization_range(&u_ranges);
	XML::get_step(&steps);	

	//resource parameter
	Int_Set resource_num, rrns;
	Double_Set rrps, tlfs;
	Range_Set rrrs;
	XML::get_resource_num(&resource_num);
	XML::get_resource_request_probability(&rrps);
	XML::get_resource_request_num(&rrns);
	XML::get_resource_request_range(&rrrs);
	XML::get_total_len_factor(&tlfs);

	//graph parameters
	Range_Set job_num_ranges;
	Range_Set arc_num_ranges;
	Int_Set is_cyclics;
	Int_Set max_indegrees;
	Int_Set max_outdegrees;
	Double_Set para_probs, cond_probs, arc_densities;
	Int_Set max_para_jobs, max_cond_branches;
	
	XML::get_ranges(&job_num_ranges, "dag_job_num_range");
	XML::get_ranges(&arc_num_ranges, "dag_arc_num_range");
	XML::get_integers(&is_cyclics, "is_cyclic");
	XML::get_integers(&max_indegrees, "max_indegree");
	XML::get_integers(&max_outdegrees, "max_outdegree");
	XML::get_doubles(&para_probs, "paralleled_probability");
	XML::get_doubles(&cond_probs, "conditional_probability");
	XML::get_doubles(&arc_densities, "dag_arc_density");
	XML::get_integers(&max_para_jobs, "max_paralleled_job");
	XML::get_integers(&max_cond_branches, "max_conditional_branch");
	
	
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
	parameters.para_prob = para_probs[0];
	parameters.cond_prob = cond_probs[0];
	parameters.arc_density = arc_densities[0];
	parameters.max_para_job = max_para_jobs[0];
	parameters.max_cond_branch = max_cond_branches[0];
	

	Output output(parameters);

	Random_Gen::uniform_integral_gen(0,10);

///DAG test

	ResourceSet test_r = ResourceSet();

	uint r = 0;
	for(uint i = 0; i < 1000; i++)
	{
		DAG_Task dag(0, test_r, parameters, 5, 10, 8);
		//DAG_Task dag2(1, test_r, parameters, 4, 20, 16);
		DAG_TaskSet dag_taskset;
		dag_taskset.add_task(dag);
		//dag_taskset.add_task(dag2);
		if(dag_schedulability_test(dag_taskset, 4, 0.5))
			r++;
	}

	double rr = r;

	rr /= 1000;

	cout<<"rr:"<<rr<<endl;

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
			//DAG_TaskSet dag_taskset;
			ProcessorSet processorset = ProcessorSet(parameters);
			ResourceSet resourceset = ResourceSet();
			//ResourceSet2<Task> resourceset2 = ResourceSet2<Task>();
			resource_gen(&resourceset, parameters);
			//resource_gen2<Task>(&resourceset2, parameters);
			//dag_task_gen(dag_taskset, resourceset, parameters, 0.1);
			tast_gen(taskset, resourceset, parameters, utilization);
			//dag_task_gen(dag_taskset, resourceset, parameters, utilization);
			for(uint i = 0; i < parameters.get_method_num(); i++)
			{
				taskset.init();
				processorset.init();
				
				if(is_schedulable(taskset, processorset, resourceset, parameters.get_test_method(i), parameters.get_test_type(i), 0))
				//if(dag_schedulability_test(dag_taskset, 4, 0.8))
				{	
					success[i]++;
				}
			}
			//result.x = dag_taskset.get_utilization_sum().get_d();
			result.x = taskset.get_utilization_sum().get_d();
		}
		for(uint i = 0; i < test_attributes.size(); i++)
		{
			fraction_t ratio(success[i], exp_times);
			result.y = ratio.get_d();
			output.add_result(i, result.x, result.y);
		}

		utilization += steps[0];

	}
	while(utilization < u_ranges[0].max || fabs(u_ranges[0].max - utilization) < _EPS);

	XML::SaveConfig((output.get_path() + "config.xml").data());

	output.export_csv();

	output.Export(PNG|EPS|SVG|TGA|JSON);

	return 0;
}




