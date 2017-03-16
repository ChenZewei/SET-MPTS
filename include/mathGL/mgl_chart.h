#ifndef MGL_CHART_H
#define MGL_CHART_H

#include <iostream>
#include <string>
#include "types.h"
#include "mgl2/mgl.h"

typedef struct
{
	string name;
	mglData data;
}Chart_Data;

typedef vector<string> Color;
typedef vector<string> Width;
typedef vector<string> Dot;

class Chart
{
	private:
		vector<Chart_Data> data_set;
		mglGraph graph;
		Color color;//already has 8 colors
		Width width;//from 0-9
		Dot dot;//already has 8 dots
		string get_line_style(int index);
	public:
		Chart();
		~Chart(){}

		void AddColor(string color);
		void AddData(string name, double* d, int size);
		void AddData(string name, Result_Set r_set);
		void SetGraphSize(int width, int height);
		void SetGraphQual(int quality);
		void ExportLineChart(string path, const char* title, double min, double max, int format = 0);
		void ExportJSON(string path);
};


#endif
