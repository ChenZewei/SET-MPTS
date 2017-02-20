#ifndef XML_H
#define XML_H

#include <string>
#include <iostream>
#include "tinyxml2.h"
#include "types.h"
#include "string.h"

using namespace tinyxml2;
using namespace std;


class XML
{
	private:
		static XMLDocument config;
		XMLDocument *output;
	public:
		XML();
		
		static void LoadFile(const char* path);		

		static void SaveConfig(const char* path);
		
		static void get_method(Test_Attribute_Set *t_set);

		static uint get_experiment_times();

		static void get_lambda(Int_Set *i_set);

		static void get_period_range(Range_Set *r_set);

		static void get_deadline_propotion(Range_Set *r_set);

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

		//output

		static void get_output(Int_Set *i_set);

		//defalut

		static void get_integers(Int_Set *i_set, const char* element_name);

		static void get_doubles(Double_Set *d_set, const char* element_name);

		static void get_ranges(Range_Set *r_set, const char* element_name);

		//xml construction
		void initialization();
		void add_element(const char* name);
		void add_element(const char* parent, const char* name, const char* text = "");
		void add_range(const char* parent, Range range);
		void set_text(const char* parent, int index1, const char* element, int index2,const char* text);
		void save_file(const char* path);
		void clear();
};




#endif
