#ifndef XML_H
#define XML_H

#include <string>
#include <iostream>
#include <vector>
#include "tinyxml2.h"
#include "types.h"

using namespace tinyxml2;
using namespace std;

typedef struct
{
	double min;
	double max;
}range;

typedef vector<int> int_set;
typedef vector<double> double_set;
typedef vector<range> range_set;


XMLDocument config;

uint get_experiment_times()
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("experiment_times");
	return atoi(title->GetText());
}

void get_lambda(int_set *i_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("lambda");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		content = subtitle->GetText();
		i_set->push_back(atoi(content));
		subtitle = subtitle->NextSiblingElement();
	}
}

void get_period_range(range_set *r_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("period_range");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		range temp;
		XMLElement *SSubtitle = subtitle->FirstChildElement("min");
		content = SSubtitle->GetText();
		fraction_t data(content);
		temp.min = data.get_d();
		SSubtitle = subtitle->FirstChildElement("max");
		content = SSubtitle->GetText();
		data = content;
		temp.max = data.get_d();
		r_set->push_back(temp);
		subtitle = subtitle->NextSiblingElement();
	}
}

void get_utilization_range(range_set *r_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("init_utilization_range");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		range temp;
		XMLElement *SSubtitle = subtitle->FirstChildElement("min");
		content = SSubtitle->GetText();
		floating_t data(content);
		temp.min = data.get_d();
		SSubtitle = subtitle->FirstChildElement("max");
		content = SSubtitle->GetText();
		data = content;
		temp.max = data.get_d();
		r_set->push_back(temp);
		subtitle = subtitle->NextSiblingElement();
	}
}

void get_step(double_set *d_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("step");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		content = subtitle->GetText();
		floating_t temp(content);
		d_set->push_back(temp.get_d());
		subtitle = subtitle->NextSiblingElement();
	}
}

void get_processor_num(int_set *i_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("processor_num");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		content = subtitle->GetText();
		i_set->push_back(atoi(content));
		subtitle = subtitle->NextSiblingElement();
	}
}


#endif
