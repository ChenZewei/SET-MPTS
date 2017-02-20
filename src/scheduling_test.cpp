#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <ctime>
#include "tasks.h"
#include "schedulability_test.h"
#include "processors.h"
#include "mgl_chart.h"
#include "xml.h"
#include "param.h"
#include "output.h"
#include "random_gen.h"
#include "test_model.h"
#include "rta_native.h"
#include "sched_test_factory.h"

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

	//output.export_table_head();

	Random_Gen::uniform_integral_gen(0,10);
	double utilization = u_ranges[0].min;

/////
/*
			TaskSet testt = TaskSet();
			ProcessorSet testp = ProcessorSet(parameters);
			ResourceSet testr = ResourceSet();
			resource_gen(&testr, parameters);
			tast_gen(testt, testr, parameters, 1);

	testt.RM_Order();

	foreach(testt.get_tasks(), ti)
	{
		uint i = ti->get_id();
cout<<"Task "<<i<<":"<<endl;
cout<<ti->get_wcet()<<" "<<ti->get_wcet_critical_sections()<<" "<<ti->get_wcet_non_critical_sections()<<endl;
		foreach(ti->get_requests(), request)
		{
			uint q = request->get_resource_id();
			cout<<"request for resource "<<q<<":"<<"N"<<request->get_num_requests()<<" L:"<<request->get_max_length()<<endl;
		}
	}
*/

/////
	time_t start, end;
	
	start = time(NULL);

cout<<endl<<"Strat at:"<<ctime(&start)<<endl;

	do
	{	
		Result result;
cout<<"Utilization:"<<utilization<<endl;
		vector<int> success;
		vector<int> exp;
		for(uint i = 0; i < test_attributes.size(); i++)
		{
			exp.push_back(0);
			success.push_back(0);
		}
		for(int i = 0; i < exp_times; i++)
		{
cout<<".";
cout<<flush;

	    	TaskSet taskset = TaskSet();
			ProcessorSet processorset = ProcessorSet(parameters);
			ResourceSet resourceset = ResourceSet();
			resource_gen(&resourceset, parameters);
			tast_gen(taskset, resourceset, parameters, utilization);

/*
cout<<"Testing:"<<endl;
		SchedTestBase *testing;
		TestModel tt = TestModel();
		testing = &tt;
		if(testing->is_schedulable(taskset, processorset, resourceset))
			cout<<"true!"<<endl;
		else
			cout<<"false!"<<endl;
*/
			for(uint j = 0; j < parameters.get_method_num(); j++)
			{
				taskset.init();
				processorset.init();	
				resourceset.init();
				exp[j]++;

/*
				if(is_schedulable(taskset, processorset, resourceset, parameters.get_test_method(j), parameters.get_test_type(j), 0))
				{
					success[j]++;
				}
*/
//cout<<test_attributes[j].test_name<<endl;
				
				//RTA_native rta_native = RTA_native(taskset, processorset, resourceset);


				SchedTestBase *schedTest = STFactory.createSchedTest(test_attributes[j].test_name, taskset, processorset, resourceset);

				if(NULL == schedTest)
				{
					cout<<"Incorrect test name."<<endl;
					return -1;
				}

				if(schedTest->is_schedulable())
				{
					success[j]++;
				}
				delete(schedTest);

			}
			//result.x = taskset.get_utilization_sum().get_d();
			result.x = utilization;
		}
cout<<endl;
		for(uint i = 0; i < test_attributes.size(); i++)
		{
			fraction_t ratio(success[i], exp[i]);
			result.y = ratio.get_d();
			result.exp_num = exp[i];
			result.success_num = success[i];
//cout<<"ratio:"<<ratio.get_d()<<endl;
			output.add_result(i, result.x, result.y, result.exp_num, result.success_num);
cout<<"Method "<<i<<": exp_times("<<result.exp_num<<") success times("<<success[i]<<") success ratio:"<<ratio.get_d()<<endl;
		}
		output.export_result_append();

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




