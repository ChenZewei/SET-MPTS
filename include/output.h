#ifndef OUTPUT_H
#define OUTPUT_H

#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include "types.h"
#include "mgl_chart.h"
#include "param.h"

class Output
{
	private:
		string path;
		Param param;
		Result_Sets result_sets;
		Chart chart;
	public:
		Output(const char* path);
		Output(Param param);
		
		string get_path();
		void add_set();
		uint get_sets_num() const;
		void add_result(uint index, double x, double y);
		uint get_results_num(uint index) const;
		string output_filename();
		string get_method_name(Test_Attribute ta);

		//export to csv
		void export_csv();

		//export to graph
		void SetGraphSize(int width, int height);
		void SetGraphQual(int quality);
		void Export(int format = 0);
};

#endif
