#include "mgl_chart.h"

void Chart::AddData(const char* name, double* d, int size)
{
	Chart_Data temp;
	temp.name = name;
	temp.data.Set(d, size);
	data_set.push_back(temp);
}

void Chart::AddData(const char* name, Result_Set r_set)
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

void Chart::SetGraphSize(int width, int height)
{
	graph.SetSize(width,height);
}

void Chart::SetGraphQual(int quality)
{
	graph.SetQuality(quality);
}

void Chart::ExportPNG(const char* path, const char* title, double min, double max)
{
	const char *line_style[] = {"r*","bo","g+","cs","yd","m^"};	
	graph.Title(title,"",-2);	
	graph.SetOrigin(0,0,0);
	graph.SetRange('x', min, max);
	//graph.SetTicks('x', 9, 1);
	graph.SetRange('y', 0, 1);	
	
	for(int i = 0; i < data_set.size(); i++)
	{
		graph.Plot(data_set[i].data,line_style[i]);
		graph.AddLegend(data_set[i].name,line_style[i]);
	}
	
	graph.Box();
	graph.Label('x',"x: TaskSet Utilization", 0);
	graph.Label('y',"y: Ratio", 0);
	graph.Legend();
	graph.Axis("xy");
	graph.WritePNG(path);
}
