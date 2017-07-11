#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include "tasks.h"
#include "processors.h"
#include "resources.h"
#include "xml.h"
#include "param.h"
#include "output.h"
#include "random_gen.h"
#include "iteration-helper.h"
#include "toolkit.h"
#include "network.h"

#define MAX_LEN 100
#define MAX_METHOD 8
#define MAXBUFFER 100
#define BACKLOG 64

#define typeof(x) __typeof(x)


#define BUFFER_SIZE 1024

using namespace std;

vector<Param> get_parameters();

int main(int argc,char** argv)
{	
//Experiment parameters
	vector<Param> parameters = get_parameters();
	Param* param = &(parameters[0]);
	Result_Set results[MAX_METHOD];
	Output output(*param);
	SchedResultSet srs;
	XML::SaveConfig((output.get_path() + "config.xml").data());
	output.export_param();

//Network parameters
	int maxi, maxfd;
	int nready;	
	int listenfd, connectfd;
	int sin_size = sizeof(struct sockaddr_in);
	int recvlen;
	ssize_t n;
	fd_set rset,allset;
	string sendbuffer;
	struct sockaddr_in server,client_addr;
	list<NetWork> clients;
	list<NetWork*> idle, busy;
	if((listenfd=socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
		cout<<"Create socket failed."<<endl;
		exit(EXIT_SUCCESS);
	}
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(parameters[0].port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listenfd, (struct sockaddr *)&server, sizeof(struct sockaddr))==-1)
	{
		cout<<"Bind error."<<endl;
		exit(EXIT_SUCCESS);
	}

	if(listen(listenfd,BACKLOG)==-1)
	{
		printf("Listen error.");
		exit(EXIT_SUCCESS);	
	}
	maxfd = listenfd;
	maxi = -1;

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	double utilization = param->u_range.min;
	time_t start, end;
	start = time(NULL);
cout<<endl<<"Strat at:"<<ctime(&start)<<endl;


	int pre_exp_time = 0;
	do
	{	
		//Experiment parameter
		int exp_time = pre_exp_time;
		pre_exp_time = 0;



		//Network
		rset = allset;
		nready = select(maxfd+1, &rset,NULL,NULL,NULL);
		if(FD_ISSET(listenfd,&rset))
		{
			if((connectfd=accept(listenfd,(struct sockaddr *)&client_addr, &sin_size))==-1)
			{
				printf("Accept error.");
				continue;
			}
			
			if(clients.size() < FD_SETSIZE)
			{
				NetWork client(connectfd, client_addr);
				clients.push_back(client);
				
				cout<<"Connection from [ip:"<<inet_ntoa(client_addr.sin_addr)<<"] has already established."<<endl;
/*
				foreach(clients, c)
				{
					cout<<"clien:"<<c->get_socket()<<endl;
					cout<<"\tstatus:"<<c->get_status()<<endl;
				}
*/
			}	

			if(clients.size() == FD_SETSIZE)
				cout<<"Too many clients."<<endl;

			FD_SET(connectfd,&allset);

			if(connectfd > maxfd)
				maxfd = connectfd;

			if(clients.size() > maxi)
				maxi = clients.size();

			if(nready <= 0)
				continue;
		}

		foreach(clients, client)
		{
			if(FD_ISSET(client->get_socket(), &rset))
			{
				string recvbuf = client->recvbuf();
				
				if(client->get_status())
				{
					cout<<"Disconnected."<<endl;
					clients.erase(client);
				}
				else
				{
					cout<<recvbuf<<endl;
				}

				vector<string> elements;

				extract_element(elements, recvbuf);


//				foreach(elements, element)
//					cout<<"element:"<<*element<<endl;


				if(0 == strcmp(elements[0].data(), "3"))//heartbeat
				{
					//do nothing
				}
				else if(0 == strcmp(elements[0].data(), "0"))//connection
				{
					sendbuffer = "1,";
					if(exp_time == param->exp_times)
					{
						sendbuffer += to_string(utilization + param->step);
						client->sendbuf(sendbuffer);
						pre_exp_time++;
					}
					else
					{
						sendbuffer += to_string(utilization);
						client->sendbuf(sendbuffer);
						exp_time++;
					}
				}
				else if(0 == strcmp(elements[0].data(), "2"))//result
				{
					cout<<"Extract result..."<<endl;

					for(uint i = 0; i < param->test_attributes.size(); i++)
					{
						string test_name;
						if(!param->test_attributes[i].rename.empty())
						{
							test_name = param->test_attributes[i].rename;
						}
						else
						{
							test_name = param->test_attributes[i].test_name;
						}

						floating_t u(elements[1]);

						output.add_result(test_name, param->test_attributes[i].style, u.get_d(), 1, atoi(elements[i+2].data()));

						stringstream buf;

						buf<<test_name;

						buf<<"\t"<<utilization<<"\t"<<1<<"\t"<<elements[i+1];

						output.append2file("result-logs.csv", buf.str());
					}

					sendbuffer = "1,";
					if(exp_time == param->exp_times)
					{
						sendbuffer += to_string(utilization + param->step);
						client->sendbuf(sendbuffer);
						pre_exp_time++;
					}
					else
					{
						sendbuffer += to_string(utilization);
						client->sendbuf(sendbuffer);
						exp_time++;
					}
				}

				if(nready <=0)
					break;
			}
		}

//		output.export_result_append(utilization);
//		output.Export(PNG);		

		if(exp_time == param->exp_times)
		{
			utilization += param->step;
		}
	}
	while(utilization < param->u_range.max || fabs(param->u_range.max - utilization) < _EPS);

	output.export_csv();
	output.Export(PNG|EPS|SVG|TGA|JSON);

	time(&end);
cout<<endl<<"Finish at:"<<ctime(&end)<<endl;

	ulong gap = difftime(end, start);
	uint hour = gap/3600;
	uint min = (gap%3600)/60;																				
	uint sec = (gap%3600)%60;

cout<<"Duration:"<<hour<<" hour "<<min<<" min "<<sec<<" sec."<<endl;
/*
	output.export_csv();
	output.Export(PNG|EPS|SVG|TGA|JSON);
*/

	return 0;
}

vector<Param> get_parameters()
{
	vector<Param> parameters;

	XML::LoadFile("config.xml");

	if(0 == access(string("results").data(), 0))
		printf("results folder exsists.\n");
	else
	{
		printf("results folder does not exsist.\n");
		if(0 == mkdir(string("results").data(), S_IRWXU))
			printf("results folder has been created.\n");
		else
			return parameters;
	}

	//Server info
	const char* ip = XML::get_server_ip();
	uint port = XML::get_server_port();

	Int_Set lambdas, p_nums;
	Double_Set steps;
	Range_Set p_ranges, u_ranges, d_ranges;
	Test_Attribute_Set test_attributes;
	uint exp_times;

	//scheduling parameter
	XML::get_method(&test_attributes);
	exp_times = XML::get_experiment_times();
	XML::get_lambda(&lambdas);
	XML::get_processor_num(&p_nums);
	XML::get_period_range(&p_ranges);
	XML::get_deadline_propotion(&d_ranges);
	XML::get_utilization_range(&u_ranges);
	XML::get_step(&steps);	

	//resource parameter
	Int_Set resource_nums, rrns, mcsns;
	Double_Set rrps, tlfs;
	Range_Set rrrs;
	XML::get_resource_num(&resource_nums);
	XML::get_resource_request_probability(&rrps);
	XML::get_resource_request_num(&rrns);
	XML::get_resource_request_range(&rrrs);
	XML::get_total_len_factor(&tlfs);
	XML::get_integers(&mcsns, "mcsn");

	//graph parameters
	Range_Set job_num_ranges;
	Range_Set arc_num_ranges;
	Int_Set is_cyclics;
	Int_Set max_indegrees;
	Int_Set max_outdegrees;
	Double_Set para_probs, cond_probs, arc_densities;
	Int_Set max_para_jobs, max_cond_branches;

	XML::get_ranges(&job_num_ranges, "dag_job_num_range");
	XML::get_ranges(&arc_num_ranges, "dag_arc_num_range");
	XML::get_integers(&is_cyclics, "is_cyclic");
	XML::get_integers(&max_indegrees, "max_indegree");
	XML::get_integers(&max_outdegrees, "max_outdegree");
	XML::get_doubles(&para_probs, "paralleled_probability");
	XML::get_doubles(&cond_probs, "conditional_probability");
	XML::get_doubles(&arc_densities, "dag_arc_density");
	XML::get_integers(&max_para_jobs, "max_paralleled_job");
	XML::get_integers(&max_cond_branches, "max_conditional_branch");

	foreach(lambdas, lambda)
	foreach(p_nums, p_num)
	foreach(steps, step)
	foreach(p_ranges, p_range)
	foreach(u_ranges, u_range)
	foreach(d_ranges, d_range)
	foreach(resource_nums, resource_num)
	foreach(rrns, rrn)
	foreach(mcsns, mcsn)
	foreach(rrps, rrp)
	foreach(tlfs, tlf)
	foreach(rrrs, rrr)
	foreach(job_num_ranges, job_num_range)
	foreach(arc_num_ranges, arc_num_range)
	foreach(max_indegrees, max_indegree)
	foreach(max_outdegrees, max_outdegree)
	foreach(para_probs, para_prob)
	foreach(cond_probs, cond_prob)
	foreach(arc_densities, arc_density)
	foreach(max_para_jobs, max_para_job)
	foreach(max_cond_branches, max_cond_branch)
	{
		Param param;
		//set parameters
		param.id = parameters.size();
		param.server_ip = ip;
		param.port = port;
		param.lambda = *lambda;
		param.p_num = *p_num;
		param.step = *step;
		param.p_range = *p_range;
		param.u_range = *u_range;
		param.d_range =  *d_range;
		param.test_attributes = test_attributes;
		param.exp_times = exp_times;
		param.resource_num = *resource_num;
		param.mcsn = *mcsn;
		param.rrn = *rrn;
		param.rrp = *rrp;
		param.tlf = *tlf;
		param.rrr = *rrr;

		param.job_num_range = *job_num_range;	
		param.arc_num_range = *arc_num_range;
	
		if(0 == is_cyclics[0])
			param.is_cyclic = false;
		else
			param.is_cyclic = true;	

		param.max_indegree = *max_indegree;	
		param.max_outdegree = *max_outdegree;
		param.para_prob = *para_prob;
		param.cond_prob = *cond_prob;
		param.arc_density = *arc_density;
		param.max_para_job = *max_para_job;
		param.max_cond_branch = *max_cond_branch;

		parameters.push_back(param);

	}
cout<<"param num:"<<parameters.size()<<endl;
	return parameters;
}

