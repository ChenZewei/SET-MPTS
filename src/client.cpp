#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include <glpk.h>
#include "tasks.h"
#include "processors.h"
#include "resources.h"
#include "xml.h"
#include "param.h"
#include "output.h"
#include "random_gen.h"
#include "test_model.h"
#include "sched_test_factory.h"
#include "solution.h"
#include "iteration-helper.h"
#include "toolkit.h"
//Socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_LEN 100
#define MAX_METHOD 8
#define MAXBUFFER 100

#define typeof(x) __typeof(x)

using namespace std;

struct ARG
{
	int connectfd;
	struct sockaddr_in server;
	double utilization;
	Param param;
}typedef ARG;

void getFiles(string path, string dir);
void read_line(string path, vector<string>& files);
vector<Param> get_parameters();
unsigned int alarm(unsigned int seconds);

int main(int argc,char** argv)
{	
/*
	if(2 != argc)
	{
		printf("Usage: %s [port]\n",argv[0]);
		exit(EXIT_SUCCESS);
	}
	int port = atoi(argv[1]);
*/

	vector<Param> parameters = get_parameters();
	Param* param = &(parameters[0]);
	SchedTestFactory STFactory;
#if UNDEF_ABANDON
	GLPKSolution::set_time_limit(TIME_LIMIT_INIT);
#endif
	int socketfd;
	int status,recvlen;
	int sin_size = sizeof(struct sockaddr_in);
	char recvbuffer[MAXBUFFER];
	memset(recvbuffer,0,MAXBUFFER);
	struct sockaddr_in server;

	if((socketfd=socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
		printf("Create socket failed.");
		exit(EXIT_SUCCESS);
	}
	if(inet_aton(parameters[0].server_ip, &(server.sin_addr))==0)
	{
		printf("Server ip illegal.");
		exit(EXIT_SUCCESS);
	}
	
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(parameters[0].server_ip); 
	server.sin_port = htons(parameters[0].port);

	while(connect(socketfd,(struct sockaddr *)&server,sizeof(struct sockaddr)) == -1)
	{
		cout<<"Connect failed! Reconnect in 10 second..."<<endl;
		sleep(10);
	}

	if(send(socketfd, "0", sizeof("0"), 0) < 0) 
	{
		cout<<"Send failed!"<<endl;
		//sleep(1);
	}

	while(1)
	{
		memset(recvbuffer,0,MAXBUFFER);
		if((recvlen = recv(socketfd, recvbuffer,sizeof(recvbuffer),0))==-1)
		{
			printf("Recieve error.\n");
			exit(EXIT_SUCCESS);	
		}
		else if(0 == recvlen)
		{
			printf("Disconnected!\n");
			exit(EXIT_SUCCESS);
		}

		cout<<recvbuffer<<endl;

		vector<string> elements;

		extract_element(elements, recvbuffer);

		if(0 == strcmp(elements[0].data(), "-1"))//termination
		{
			exit(EXIT_SUCCESS);
		}
		else if(0 == strcmp(elements[0].data(), "3"))//heartbeat
		{
			if(send(socketfd, "3", sizeof("3"), 0) < 0) 
			{
				cout<<"Send failed!"<<endl;
			}
		}
		else if(0 == strcmp(elements[0].data(), "1"))//work
		{
			floating_t utilization(elements[1]);

			vector<int> success;
			for(uint i = 0; i < param->test_attributes.size(); i++)
			{
				success.push_back(0);
			}

			bool abandon;
			string sendbuf;
//			do
			{
				abandon = false;
				stringstream buf;
				buf<<"2,";
				buf<<utilization.get_d();

				TaskSet taskset = TaskSet();
				ProcessorSet processorset = ProcessorSet(*param);
				ResourceSet resourceset = ResourceSet();
				resource_gen(&resourceset, *param);
				tast_gen(taskset, resourceset, *param, utilization.get_d());

				for(uint j = 0; j < param->get_method_num(); j++)
				{
					taskset.init();
					processorset.init();	
					resourceset.init();

					SchedTestBase *schedTest = STFactory.createSchedTest(param->test_attributes[j].test_name, taskset, processorset, resourceset);
					if(NULL == schedTest)
					{
						cout<<"Incorrect test name."<<endl;
						return -1;
					}
					if(schedTest->is_schedulable())
					{
						success[j]++;
						buf<<","<<1;
					}
					else
						buf<<","<<0;
		#if UNDEF_ABANDON
					if(GLP_UNDEF == schedTest->get_status())
					{
		cout<<"Abandon cause GLP_UNDEF"<<endl;
						long current_lmt = GLPKSolution::get_time_limit();
						long new_lmt = (current_lmt+TIME_LIMIT_GAP <= TIME_LIMIT_UPPER_BOUND)?current_lmt+TIME_LIMIT_GAP:TIME_LIMIT_UPPER_BOUND;
		cout<<"Set GLPK time limit to:"<<new_lmt/1000<<" s"<<endl;
						GLPKSolution::set_time_limit(new_lmt);
						abandon = true;
						delete(schedTest);
						break;
					}
		#endif				
					delete(schedTest);
				}
				sendbuf = buf.str();
			}
//			while(abandon);

			//buf<<"\n";

			if(abandon)
			{
				if(send(socketfd, "0", sizeof("0"), 0) < 0) 
				{
					cout<<"Send failed!"<<endl;
					//sleep(1);
				}
			}
			else
			{
				cout<<sendbuf<<endl;	
				if(send(socketfd, sendbuf.data(), strlen(sendbuf.data()), 0) < 0)
				{
					cout<<"Send failed!"<<endl;
					//sleep(1);
				}
			}	

		}
	}

	return 0;
}

void getFiles(string path, string dir)  
{  
	string cmd = "ls " + path + " > " + path + dir;
	system(cmd.data());
}  

void read_line(string path, vector<string>& files)
{
	string buf;
	ifstream dir(path.data(), ifstream::in);
	getline(dir, buf);
	while(getline(dir, buf))
	{
		files.push_back("config/" + buf);
//cout<<"file name:"<<buf<<endl;
	}
}

vector<Param> get_parameters()
{
	//getFiles("config/", "0");
	//read_line("config/0", configurations);
	//vector<string> configurations;
	
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

