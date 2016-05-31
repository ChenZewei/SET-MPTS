#include "xml.h"

XML::XML()
{
	
}

XMLDocument XML::config;

void XML::LoadFile(const char* path)
{
	config.LoadFile(path);
}

void XML::get_method(Int_Set *i_set)
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("schedulability_test");
	XMLElement *subtitle = title->FirstChildElement("data");
	while(subtitle)
	{
		content = subtitle->GetText();
		int transform = 0;
		if(0 == strcmp(content,"P-EDF"))
		{
			transform = 0;
		}
		else if(0 == strcmp(content,"BCL-FTP"))
		{
			transform = 1;
		}
		else if(0 == strcmp(content,"BCL-EDF"))
		{
			transform = 2;
		}
		else if(0 == strcmp(content,"WF-DM"))
		{
			transform = 3;
		}
		else if(0 == strcmp(content,"WF-EDF"))
		{
			transform = 4;
		}
		else
		{
			transform = 0;
		}
		i_set->push_back(transform);
		subtitle = subtitle->NextSiblingElement();
	}
}

uint XML::get_experiment_times()
{
	const char* content;
	XMLElement *root = config.RootElement();
	XMLElement *title = root->FirstChildElement("experiment_times");
	return atoi(title->GetText());
}

void XML::get_lambda(Int_Set *i_set)
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

void XML::get_period_range(Range_Set *r_set)
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

void XML::get_utilization_range(Range_Set *r_set)
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

void XML::get_step(Double_Set *d_set)
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

void XML::get_processor_num(Int_Set *i_set)
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

void XML::get_resource_num(Int_Set *i_set)
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

void XML::get_resource_request_probability(Double_Set *d_set)
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

void XML::get_resource_request_num(Int_Set *i_set)
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

void XML::get_resource_request_range(Range_Set *r_set)
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

void XML::get_total_len_factor(Double_Set *d_set)
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
