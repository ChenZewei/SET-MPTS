#ifndef MGL_CHART_H
#define MGL_CHART_H

#include "types.h"
#include "mgl2/mgl.h"

typedef struct
{
	const char* name;
	mglData data;
}Chart_Data;

class Chart
{
	private:
		vector<Chart_Data> data_set;
		mglGraph graph;
	public:
		Chart(){}
		~Chart(){}

		void AddData(const char* name, double* d, int size);
		void AddData(const char* name, Result_Set r_set);
		void SetGraphSize(int width, int height);
		void SetGraphQual(int quality);
		void ExportPNG(const char* path, const char* title, double min, double max);
};


#endif
