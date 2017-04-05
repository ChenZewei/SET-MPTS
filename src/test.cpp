#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <ctime>
#include "dag_gedf.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"
#include "xml.h"
#include "param.h"
#include "output.h"
#include "random_gen.h"
#include "iteration-helper.h"

#define MAX_LEN 100
#define MAX_METHOD 8

#define typeof(x) __typeof(x)

using namespace std;

void config(Param &parameters);

int main(int argc,char** argv)
{
	floating_t u = 0;

	if(1 < argc)
		u = floating_t(argv[1]);


	Param parameters;
	config(parameters);
	Output output(parameters);
	Random_Gen::uniform_integral_gen(0,10);

	DAG_TaskSet tasks = DAG_TaskSet();
	ProcessorSet processorset = ProcessorSet(parameters);
	ResourceSet resourceset = ResourceSet();
	resource_gen(&resourceset, parameters);
	dag_task_gen(tasks, resourceset, parameters, u.get_d());

/*
	foreach(tasks.get_tasks(), task)
	{
		cout<<"/////////// task "<<task->get_id()<<" ///////////"<<endl;
		cout<<"wcet:"<<task->get_wcet()<<" deadline:"<<task->get_deadline()<<" period:"<<task->get_period()<<endl;
		task->display_vertices();
		task->display_arcs();
		for(uint j = 0; j < task->get_vnode_num(); j++)
		{
			task->display_follow_ups(j);
			task->display_precedences(j);
		}
		cout<<"vol:"<<task->get_vol()<<endl;
		cout<<"len:"<<task->get_len()<<endl;
		cout<<"utilization:"<<task->get_utilization()<<endl;
		cout<<"density:"<<task->get_density()<<endl;
	}
*/

	if(dag_schedulability_test(tasks, 4, 0.1))
		cout<<"Schedulable!"<<endl;
	else
		cout<<"Unschedulable!"<<endl;

	return 0;
}













void config(Param &parameters)
{
	Int_Set lambdas, p_num, methods;
	Double_Set steps;
	Range_Set p_ranges, u_ranges, d_ranges;
	Test_Attribute_Set test_attributes;
	uint exp_times;
	Result_Set results[MAX_METHOD];

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
	Int_Set resource_num, rrns, mcsns;
	Double_Set rrps, tlfs;
	Range_Set rrrs;
	XML::get_resource_num(&resource_num);
	XML::get_resource_request_probability(&rrps);
	XML::get_resource_request_num(&rrns);
	XML::get_resource_request_range(&rrrs);
	XML::get_total_len_factor(&tlfs);
	XML::get_integers(&mcsns, "mcsn");

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
	parameters.mcsn = mcsns[0];
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
}
