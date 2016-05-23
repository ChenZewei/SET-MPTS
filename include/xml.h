#ifndef XML_H
#define XML_H

#include <string>
#include <iostream>
#include "tinyxml2.h"
#include "types.h"

using namespace tinyxml2;
using namespace std;


class XML
{
	private:
		static XMLDocument config;
	public:
		XML();
		
		static void LoadFile(const char* path);		
		
		static void get_method(Int_Set *i_set);

		static uint get_experiment_times();

		static void get_lambda(Int_Set *i_set);

		static void get_period_range(Range_Set *r_set);

		static void get_utilization_range(Range_Set *r_set);

		static void get_step(Double_Set *d_set);

		static void get_processor_num(Int_Set *i_set);

		//resource

		static void get_resource_num(Int_Set *i_set);

		//resource request

		static void get_resource_request_probability(Double_Set *d_set);

		static void get_resource_request_num(Int_Set *i_set);

		static void get_resource_request_range(Range_Set *r_set);

		static void get_total_len_factor(Double_Set *d_set);
};




#endif
