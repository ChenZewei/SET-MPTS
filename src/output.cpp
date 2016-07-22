#include "output.h"

Output::Output(const char* path)
{
	this->path = path;
}

Output::Output(Param param)
{
	if(0 == access(string("results/" + param.output_filename()), 0))
		printf("results folder exsists.\n");
	else
	{
		printf("results folder does not exsist.\n");
		if(0 == mkdir(string("results/" + param.output_filename(), S_IRWXU))
			printf("results folder has been created.\n");
		else
			return 0;
	}
	this->path = "results/" + param.output_filename()) + "/";
}

void Output::add_set()
{
	Result_Set results;
	result_sets.push_back(results);
}

uint Output::get_sets_num() const
{
	return result_sets.size();
}

void Output::add_result(uint index, double x, double y)
{
	Result result;
	result.x = x;
	result.y = y;
	result_sets[index].push_back(result);
}

uint Output::get_results_num(uint index) const
{
	return result_sets[index].size();
}

//export to csv
void Output::export_csv()
{
	string file_name = path + "result.csv";
	ofstream output_file(file_name);

	output_file<<"Lambda:"<<param.lambda<<",";					
	output_file<<" processor number:"<<param.p_num<<",";
	output_file<<" step:"<<param.step<<",";
	output_file<<" utilization range:["<<param.u_range.min<<"-"<<param.u_range.max<<"],";
	output_file<<setprecision(0)<<" period range:["<<param.p_range.min<<"-"<<param.p_range.max<<"]\n"<<setprecision(3);
	output_file<<"Utilization,";
	for(uint i = 0; i < test_attributes.size(); i++)
	{
		output_file<<get_method_name(test_attributes[i])<<" ratio,";
	}
	output_file<<"\n";

	double utilization = param.u_range.min;
	
	output_file<<utilization<<",";
	do
	{
		output_file<<","<<utilization;
		
	}
	while(utilization < param.u_range.max || fabs(param.u_range.max - utilization) < EPS);
	output_file<<utilization<<"\n";
	
	for(uint i = 0; i < result_sets.size(); i++)
	{
		for(uint j = 0; j < result_sets[i].size(); j++)
		{
			output_file<<result_sets[i][j].y<<",";
		}
	}
	output_file<<"\n";
	output_file.flush();
	utilization += param.step;
}

//export to graph
void Output::SetGraphSize(int width, int height)
{
	chart.SetGraphSize(width, height);
}

void Output::SetGraphQual(int quality)
{
	chart.SetGraphQual(quality);
}

void Output::export_line_chart(int format)
{
	for(uint i = 0; i < get_sets_num(); i++)
	{
		
	}
}
