#ifndef OUTPUT_H
#define OUTPUT_H

#include <fstream>
#include <sstream>
#include <unistd.h>
#include "types.h"
#include "mgl_chart.h"
#include "param.h"
#include "sched_result.h"

class Param;
class Chart;

class Output
{
	private:
		string path;
		Param param;
		//Result_Sets result_sets;
		SchedResultSet srs;
		Chart chart;

	public:
		Output(const char* path);
		Output(Param param);
		
		string get_path();
		//void add_set();
		uint get_sets_num() const;
		//void add_result(uint index, double x, double y, uint e_num, uint s_num);
		void add_result(string test_name, double utilization, uint e_num, uint s_num);
		uint get_results_num(uint index) const;
		string output_filename();
		string get_method_name(Test_Attribute ta);

		//output to console
		void proceeding();
		void proceeding(string test_name, double utilization, uint e_num, uint s_num);
		void finish();

		//export to csv
		void export_csv();
		void export_table_head();
		void export_result_append(double utilization);
		void append2file(string flie_name, string buffer);

		//export to graph
		void SetGraphSize(int width, int height);
		void SetGraphQual(int quality);
		void Export(int format = 0);
};

#endif
