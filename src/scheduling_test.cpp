#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include "tasks.h"
#include "processors.h"
#include "resources.h"
#include "xml.h"
#include "param.h"
#include "output.h"
#include "random_gen.h"
#include "test_model.h"
#include "sched_test_factory.h"
#include "iteration-helper.h"

#define MAX_LEN 100
#define MAX_METHOD 8

#define typeof(x) __typeof(x)

using namespace std;


int main(int argc,char** argv)
{	

	Int_Set lambdas, p_num, methods;
	Double_Set steps;
	Range_Set p_ranges, u_ranges, d_ranges;
	Test_Attribute_Set test_attributes;
	uint exp_times;
	Result_Set results[MAX_METHOD];
	Param parameters;
	SchedTestFactory STFactory;

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
	
	Output output(parameters);

	Random_Gen::uniform_integral_gen(0,10);
	double utilization = u_ranges[0].min;

	time_t start, end;
	
	start = time(NULL);

cout<<endl<<"Strat at:"<<ctime(&start)<<endl;

	do
	{	
		Result result;
cout<<"Utilization:"<<utilization<<endl;
		vector<int> success;
		vector<int> exp;
		vector<int> exc;
		for(uint i = 0; i < test_attributes.size(); i++)
		{
			exp.push_back(0);
			success.push_back(0);
			exc.push_back(0);
		}
		for(int i = 0; i < exp_times; i++)
		{
cout<<".";
cout<<flush;
			uint s_n = 0;
			uint s_i = 0;
	    	TaskSet taskset = TaskSet();
			ProcessorSet processorset = ProcessorSet(parameters);
			ResourceSet resourceset = ResourceSet();
			resource_gen(&resourceset, parameters);
			tast_gen(taskset, resourceset, parameters, utilization);
			//taskset.Leisure_Order();
			for(uint j = 0; j < parameters.get_method_num(); j++)
			{
				taskset.init();
				processorset.init();	
				resourceset.init();
				exp[j]++;

				SchedTestBase *schedTest = STFactory.createSchedTest(test_attributes[j].test_name, taskset, processorset, resourceset);
				if(NULL == schedTest)
				{
					cout<<"Incorrect test name."<<endl;
					return -1;
				}

				if(schedTest->is_schedulable())
				{
					success[j]++;
#if SORT_DEBUG
					s_n++;
					s_i = j;
					cout<<test_attributes[j].test_name<<" success!"<<endl;
#endif
				}
				
				delete(schedTest);

			}

#if SORT_DEBUG
			if(1 == s_n)
			{
				exc[s_i]++;
				cout<<"Exclusive Success TaskSet:"<<endl;
				cout<<"/////////////////"<<test_attributes[s_i].test_name<<"////////////////"<<endl;
				foreach(taskset.get_tasks(), task)
				{
					cout<<"Task "<<task->get_id()<<":"<<endl;
					cout<<"WCET:"<<task->get_wcet()<<" Deadline:"<<task->get_deadline()<<" Period:"<<task->get_period()<<" Gap:"<<task->get_deadline()-task->get_wcet()<<" Leisure:"<<taskset.leisure(task->get_id())<<endl;
					cout<<"-----------------------"<<endl;
				}
				sleep(1);
			}
#endif

			result.utilization = utilization;
		}
cout<<endl;
		for(uint i = 0; i < test_attributes.size(); i++)
		{
			fraction_t ratio(success[i], exp[i]);
			output.add_result(test_attributes[i].test_name, utilization, exp[i], success[i]);

			stringstream buf;

			if(0 == strcmp(test_attributes[i].rename.data(),""))
				buf<<test_attributes[i].test_name;
			else
				buf<<test_attributes[i].rename;

			buf<<"\t"<<utilization<<"\t"<<exp[i]<<"\t"<<success[i];

			output.append2file("result-logs.csv", buf.str());

cout<<"Method "<<i<<": exp_times("<<exp[i]<<") success times("<<success[i]<<") success ratio:"<<ratio.get_d()<<" exc_s:"<<exc[i]<<endl;
		}
		output.export_result_append(utilization);
		output.Export(PNG);
		utilization += steps[0];
	}
	while(utilization < u_ranges[0].max || fabs(u_ranges[0].max - utilization) < _EPS);

	time(&end);
	cout<<endl<<"Finish at:"<<ctime(&end)<<endl;

	ulong gap = difftime(end, start);
	uint hour = gap/3600;
	uint min = (gap%3600)/60;
	uint sec = (gap%3600)%60;

	cout<<"Duration:"<<hour<<" hour "<<min<<" min "<<sec<<" sec."<<endl;

	XML::SaveConfig((output.get_path() + "config.xml").data());

	output.export_csv();

	output.Export(PNG|EPS|SVG|TGA|JSON);

	return 0;
}




