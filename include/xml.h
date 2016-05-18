#ifndef XML_H
#define XML_H

#include <string>
#include <iostream>
#include "tinyxml2.h"
#include "types.h"

using namespace tinyxml2;
using namespace std;


XMLDocument config;

void get_method(Int_Set *i_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("schedulability_test");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		content = subtitle->GetText();
		cout<<content<<endl;
		int transform = 0;
		if(content == "P-EDF")
		{
			cout<<"0"<<endl;
			transform = 0;
		}
		else if(content == "BCL-FTP")
		{
			cout<<"1"<<endl;
			transform = 1;
		}
		else if(content == "BCL-EDF")
		{
			cout<<"2"<<endl;
			transform = 2;
		}
		else
		{
			cout<<"4"<<endl;
			transform = 0;
		}
		i_set->push_back(transform);
		subtitle = subtitle->NextSiblingElement();
	}
}

uint get_experiment_times()
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("experiment_times");
	return atoi(title->GetText());
}

void get_lambda(Int_Set *i_set)
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

void get_period_range(Range_Set *r_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("period_range");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		Range temp;
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

void get_utilization_range(Range_Set *r_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("init_utilization_range");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		Range temp;
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

void get_step(Double_Set *d_set)
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

void get_processor_num(Int_Set *i_set)
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

//resource

void get_resource_num(Int_Set *i_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("resource_num");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		content = subtitle->GetText();
		i_set->push_back(atoi(content));
		subtitle = subtitle->NextSiblingElement();
	}
}

//resource request

void get_resource_request_probability(Double_Set *d_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("rrp");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		content = subtitle->GetText();
		floating_t temp(content);
		d_set->push_back(temp.get_d());
		subtitle = subtitle->NextSiblingElement();
	}
}

void get_resource_request_num(Int_Set *i_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("rrn");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		content = subtitle->GetText();
		i_set->push_back(atoi(content));
		subtitle = subtitle->NextSiblingElement();
	}
}

void get_resource_request_range(Range_Set *r_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("rrr");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		Range temp;
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

void get_total_len_factor(Double_Set *d_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("total_len_factor");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		content = subtitle->GetText();
		floating_t temp(content);
		d_set->push_back(temp.get_d());
		subtitle = subtitle->NextSiblingElement();
	}
}


#endif
