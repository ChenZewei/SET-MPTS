#include "mgl_chart.h"
#include "iteration-helper.h"
#include "math-helper.h"

Chart::Chart()
{
	color.push_back("{x000000}");//black
	color.push_back("{x5B9BD5}");//blue
	color.push_back("{xEB5B5B}");//red
	color.push_back("{xED7D31}");//orange
	color.push_back("{xFFC000}");//yellow
	color.push_back("{x70AD47}");//green
	color.push_back("{x44546A}");//dark grey
	color.push_back("{x954F72}");//pupil

	width.push_back("0");
	width.push_back("1");
	width.push_back("2");
	width.push_back("3");
	width.push_back("4");
	width.push_back("5");
	width.push_back("6");
	width.push_back("7");
	width.push_back("8");
	width.push_back("9");

	dot.push_back("*");
	dot.push_back("o");
	dot.push_back("+");
	dot.push_back("s");
	dot.push_back("d");
	dot.push_back("^");
	dot.push_back("v");
	dot.push_back("x");
}

string Chart::get_line_style(int index)
{
	string style = "";
	style += color[index];
	style += width[2];
	style += dot[index];
	return style;
}

void Chart::AddColor(string newColor)
{
	color.push_back(newColor);
}
/*
void Chart::AddData(string name, double* d, int size)
{
	Chart_Data temp;
	temp.name = name;
	temp.data.Set(d, size);
	data_set.push_back(temp);
}

void Chart::AddData(string name, Result_Set r_set)
{

	Chart_Data temp;
	temp.name = name;
	temp.data = mglData(r_set.size());
	for(int i = 0; i < r_set.size(); i++)
	{
		temp.data.a[i] = r_set[i].y;
	}
	data_set.push_back(temp);
}
*/

void Chart::AddData(SchedResultSet srs)
{
	this->srs = srs;
}

void Chart::SetGraphSize(int width, int height)
{
	graph.SetSize(width,height);
}

void Chart::SetGraphQual(int quality)
{
	graph.SetQuality(quality);
}

void Chart::ExportLineChart(string path, const char* title, double min, double max, double step, int format)
{
	graph.Clf('w');
	if(!(0 == strcmp(title, "")))
		graph.Title(title,"",-2);	
	graph.SetOrigin(0,0,0);
	graph.SetRange('x', min, max);
	graph.SetRange('y', 0, 1);	

	vector<Chart_Data> data_sets;

	vector<SchedResult>& results_set= srs.get_sched_result_set();

	foreach(results_set, results)
	{
		uint num = (max - min)/step + 1;
		Chart_Data c_data;
		c_data.name = results->get_test_name();
		c_data.data = mglData(num);
		{
			double i = min; int j = 0;
			for(; i - max < _EPS; i += step, j++)
			{

				Result r = results->get_result_by_utilization(i);
				if(r.exp_num == 0)
				{
					c_data.data.a[j] = NAN;
				}
				else
				{
					double ratio = r.success_num;
					ratio /= r.exp_num;
					c_data.data.a[j] = ratio;
				}

			}
		}

		data_sets.push_back(c_data);
	}

	for(int i = 0; i < data_sets.size(); i++)
	{
		graph.Plot(data_sets[i].data, get_line_style(i).c_str());
		graph.AddLegend(data_sets[i].name.c_str(), get_line_style(i).c_str());
	}
	
	graph.Box();
	//graph.Label('x',"x: TaskSet Utilization", 0);
	//graph.Label('y',"y: Ratio", 0);
	graph.Legend(0);
	graph.Axis("xy");

	string temp;

	if(0x01 & format)
	{
		temp = path + ".png";
		graph.WritePNG(temp.data());
	}
	if(0x02 & format)
	{
		temp = path + ".eps";
		graph.WriteEPS(temp.data());
	}
	if(0x04 & format)
	{
		temp = path + ".svg";
		graph.WriteSVG(temp.data());
	}
	if(0x08 & format)
	{
		temp = path + ".tga";
		graph.WriteTGA(temp.data());
	}

}

void Chart::ExportJSON(string path)
{
	graph.WriteJSON(path.data());
}

/*
void Chart::ExportLineChart(string path, const char* title, double min, double max, int format)
{
	graph.Clf('w');
	if("" != title)
		graph.Title(title,"",-2);	
	graph.SetOrigin(0,0,0);
	graph.SetRange('x', min, max);
	graph.SetRange('y', 0, 1);	

	for(int i = 0; i < data_set.size(); i++)
	{
		graph.Plot(data_set[i].data, get_line_style(i).c_str());
		graph.AddLegend(data_set[i].name.c_str(), get_line_style(i).c_str());
	}
	
	graph.Box();
	//graph.Label('x',"x: TaskSet Utilization", 0);
	//graph.Label('y',"y: Ratio", 0);
	graph.Legend(0);
	graph.Axis("xy");

	string temp;

	if(0x01 & format)
	{
		temp = path + ".png";
		graph.WritePNG(temp.data());
	}
	if(0x02 & format)
	{
		temp = path + ".eps";
		graph.WriteEPS(temp.data());
	}
	if(0x04 & format)
	{
		temp = path + ".svg";
		graph.WriteSVG(temp.data());
	}
	if(0x08 & format)
	{
		temp = path + ".tga";
		graph.WriteTGA(temp.data());
	}

}
*/


