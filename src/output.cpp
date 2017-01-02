#include "output.h"

Output::Output(const char* path)
{
	this->path = path;
	chart.SetGraphSize(800,600);
	chart.SetGraphQual(3);
}

Output::Output(Param param)
{
	this->param = param;
	if(0 == access(string("results/" + output_filename()).data(), 0))
		printf("result folder exsists.\n");
	else
	{
		printf("result folder does not exsist.\n");
		if(0 == mkdir(string("results/" + output_filename()).data(), S_IRWXU))
			printf("result folder has been created.\n");
	}
	this->path = "results/" + output_filename() + "/";
	chart.SetGraphSize(1280,640);
	chart.SetGraphQual(3);
	
	
	for(uint i = 0; i < param.get_method_num(); i++)
		add_set();
	
}

string Output::get_path()
{
	return path;
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

string Output::output_filename()
{
	stringstream buf;
	buf<<"l:"<<param.lambda<<"-"<<"s:"<<param.step<<"-"<<"P:"<<param.p_num<<"-"<<fixed<<setprecision(0)<<"u:["<<param.u_range.min<<","<<param.u_range.max<<"]-"<<"p:["<<param.p_range.min<<","<<param.p_range.max<<"]";
	return buf.str();
}

string Output::get_method_name(Test_Attribute ta)
{
	string name;
	switch(ta.test_method)
	{
		default:
			name = "P-EDF";
			break;
		case 0:
			name =  "P-EDF";
			break;
		case 1:
			name =  "BCL-FTP";
			break;
		case 2:
			name =  "BCL-EDF";
			break;
		case 3:
			name = "WF-DM";
			break;
		case 4:
			name = "WF-EDF";
			break;
		case 5:
			name = "RTA-GFP";
			break;
		case 6:
			name = "FF-DM";
			break;
		case 7:
			name = "LP-PFP";
			break;
		case 8:
			name = "LP-GFP";
			break;
		case 9:
			name = "RO-PFP";
			break;
	}
	if(0 == strcmp(ta.remark.data(), ""))
		return name;
	else
		return name + "-" + ta.remark;
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
	for(uint i = 0; i < param.test_attributes.size(); i++)
	{
		output_file<<get_method_name(param.test_attributes[i])<<" ratio,";
	}
	output_file<<"\n";

	double utilization = param.u_range.min;

	for(uint i = 0; i < result_sets[0].size(); i++)
	{
		output_file<<result_sets[0][i].x<<",";
		for(uint j = 0; j < result_sets.size(); j++)
		{
			output_file<<result_sets[j][i].y<<",";
		}
		output_file<<"\n";
	}

/*
	output_file<<utilization<<",";
	do
	{
		output_file<<","<<utilization;
		
	}
	while(utilization < param.u_range.max || fabs(param.u_range.max - utilization) < _EPS);
	output_file<<utilization<<"\n";
*/
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

void Output::Export(int format)
{
	string temp, file_name = path + "result";
	for(uint i = 0; i < get_sets_num(); i++)
	{
		chart.AddData(get_method_name(param.test_attributes[i]), result_sets[i]);
	}

	if(0x0f & format)
	{
		chart.ExportLineChart(file_name, "", param.u_range.min, param.u_range.max, format);
	}
	if(0x10 & format)
	{
		temp = file_name + ".json";
		chart.ExportJSON(temp);
	}
}
