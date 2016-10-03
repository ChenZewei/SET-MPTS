#ifndef PARAM_H
#define PARAM_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "types.h"


class Param
{
	public:
		//basic parameters
		int lambda;
		int p_num;
		double step;
		Range p_range;
		Range u_range;
		Range d_range;
		Test_Attribute_Set test_attributes;
		uint exp_times;
		//resource parameters
		int resource_num;
		int rrn;
		double rrp;
		double tlf;
		Range rrr;
		//graph task model parameters
		Range job_num_range;
		Range arc_num_range;
		bool cyclic;
		int max_indegree;
		int max_outdegree;
	
	public:
		void set_lambda(int lambda);
		int get_lambda() const;
		void set_processor_num(int num);
		int get_processor_num() const;
		void set_step(double step);
		double get_step() const;
		void set_period_range(Range p_range);
		Range get_period_range() const;
		void set_utilization_range(Range u_range);
		Range get_utilization_range() const;
		void set_deadline_range(Range d_range);
		Range get_deadline_range() const;
		void set_test_attribute_set(Test_Attribute_Set test_attributes);
		Test_Attribute_Set get_test_attribute_set() const;
		void set_experiment_times(uint times);
		int get_experiment_times() const;

		void set_resource_num(int resource_num);
		int get_resource_num() const;
		void set_request_num(int rrn);
		int get_request_num() const;
		void set_request_probability(double rrp);
		double get_request_probability() const;
		void set_total_length_factor(double tlf);
		double get_total_length_factor() const;
		void set_request_range(Range rrr);
		Range get_request_range() const;

		void set_job_num_range(Range job_num_range);
		Range get_job_num_range() const;
		void set_arc_num_range(Range arc_num_range);
		Range get_arc_num_range() const;
		void set_cyclic(bool is_cyclic);
		bool get_cyclic() const;
		void set_max_indegree(int max_indegree);
		int get_max_indegree() const;
		void set_max_outdegree(int max_outdegree);
		int get_ax_outdegree() const;
		
		uint get_method_num();
		int get_test_method(uint index);
		int get_test_type(uint index);

};

#endif
