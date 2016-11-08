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

	Output output(parameters);

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

		utilization += steps[0];

	}
	while(utilization < u_ranges[0].max || fabs(u_ranges[0].max - utilization) < EPS);

	output.export_csv();

	output.export_line_chart(1);

	return 0;
}




