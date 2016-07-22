#include <iostream>
#include "../include/xml.h"

int main()
{
	XML xml;
	Int_Set lambdas, p_num, methods;
	Double_Set steps;
	Range_Set p_ranges, u_ranges;
	uint exp_times;

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
	Int_Set resource_num, rrns;
	Double_Set rrps, tlfs;
	Range_Set rrrs;
	XML::get_resource_num(&resource_num);
	XML::get_resource_request_probability(&rrps);
	XML::get_resource_request_num(&rrns);
	XML::get_resource_request_range(&rrrs);
	XML::get_total_len_factor(&tlfs);

	xml.initialization();
	xml.add_element("parameters");
	cout<<"1"<<endl;
	xml.add_element("parameters", "init_utilization_range");
	cout<<"2"<<endl;
	xml.add_range("init_utilization_range", u_ranges[0]);
	xml.add_range("init_utilization_range", u_ranges[1]);
	xml.add_element("parameters", "period_range");
	xml.add_range("period_range", p_ranges[0]);
	xml.add_range("period_range", p_ranges[1]);
	cout<<"3"<<endl;
	cout<<"4"<<endl;
	xml.save_file("test.xml");
	cout<<"5"<<endl;
	
	
	return 0;
}
