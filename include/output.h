#ifndef OUTPUT_H
#define OUTPUT_H

#include <fstream>
#include <sstream>
#include <string>
#include "types.h"
#include "mgl_chart.h"

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
		
		
		void add_set();
		uint get_sets_num() const;
		void add_result(uint index, double x, double y);
		uint get_results_num(uint index) const;

		//export to csv
		void export_csv();

		//export to graph
		void SetGraphSize(int width, int height);
		void SetGraphQual(int quality);
		void export_line_chart(int format);
};

#endif
