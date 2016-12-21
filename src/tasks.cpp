#include "tasks.h"
#include "resources.h"
#include "processors.h"
#include "sort.h"
#include "random_gen.h"
#include "param.h"

////////////////////////////Request//////////////////////////////

Request::Request(uint resource_id,
	uint num_requests,
	ulong max_length,
	ulong total_length,
	uint locality)
{
	this->resource_id = resource_id;
	this->num_requests = num_requests;
	this->max_length = max_length;
	this->total_length = total_length;
	this->locality = locality;
}

uint Request::get_resource_id() const { return resource_id; }
uint Request::get_num_requests() const { return num_requests; }
ulong Request::get_max_length() const { return max_length; }
ulong Request::get_total_length() const { return total_length; }
ulong Request::get_locality() const { return locality; }

////////////////////////////Task//////////////////////////////

Task::Task(uint id,
		ulong wcet, 
	   	ulong period,
	   	ulong deadline,
		uint priority)
{
	this->id = id;
	this->wcet = wcet;
	if(0 == deadline)
		this->deadline = period;
	else
		this->deadline = deadline;
	this->period = period;
	this->priority = priority;
	utilization = this->wcet;
	utilization /= this->period;
	density = this->wcet;
	if (this->deadline <= this->period)
		density /= this->deadline;
	else
		density /= this->period;
	partition = 0XFFFFFFFF;
}

Task::Task(	uint id,
		ResourceSet& resourceset,
		Param param,
		ulong wcet, 
		ulong period,
		ulong deadline,
		uint priority)
{
	this->id = id;
	this->wcet = wcet;
	if(0 == deadline)
		this->deadline = period;
	else
		this->deadline = deadline;
	this->period = period;
	this->priority = priority;
	utilization = this->wcet;
	utilization /= this->period;
	density = this->wcet;
	if (this->deadline <= this->period)
		density /= this->deadline;
	else
		density /= this->period;
	partition = MAX_INT;
	spin = 0;
	self_suspension = 0;
	local_blocking = 0;
	total_blocking = 0;
	jitter = 0;
	response_time = wcet;
	cluster = MAX_INT;
	independent = true;
	wcet_non_critical_sections = this->wcet;
	wcet_critical_sections = 0;
	carry_in = false;

	for(int i = 0; i < resourceset.size(); i++)
	{
		if(Random_Gen::probability(param.rrp))
		{
			uint num = Random_Gen::uniform_integral_gen(1, param.rrn);
			uint max_len = Random_Gen::uniform_integral_gen(param.rrr.min, param.rrr.max);
			add_request(i, num, max_len, param.tlf*max_len, resourceset.get_resources()[i].get_locality());
			resourceset.add_task(i, this);
		}
	}
}

Task::~Task()
{
//	if(affinity)
	//	delete affinity;
	requests.clear();
}

void Task::init()
{
	partition = MAX_INT;
	spin = 0;
	self_suspension = 0;
	local_blocking = 0;
	total_blocking = 0;
	jitter = 0;
	response_time = wcet;
	cluster = MAX_INT;
	independent = true;
	wcet_non_critical_sections = this->wcet;
	wcet_critical_sections = 0;
	carry_in = false;
}

void Task::add_request(uint res_id, uint num, ulong max_len, ulong total_len, uint locality)
{
	requests.push_back(Request(res_id, num, max_len, total_len, locality));
}

ulong Task::DBF(ulong time)
{
	if(time >= deadline)
		return ((time - deadline)/period+1)*wcet;
	else 
		return 0;
}

uint Task::get_max_num_jobs(ulong interval)
{
	uint num_jobs;
	num_jobs = ceiling(interval + get_response_time(), get_period());
	return num_jobs;
}

fraction_t Task::get_utilization() const
{
	return utilization;
}

fraction_t Task::get_density() const
{
	return density;
}

void Task::get_utilization(fraction_t &utilization)
{
	utilization = wcet;
	utilization /= period;
}

void Task::get_density(fraction_t &density)
{
	density = wcet;
	density /= std::min(deadline,period);
}

uint Task::get_id() const { return id; }
void Task::set_id(uint id) { this->id = id; };
ulong Task::get_wcet() const	{ return wcet; }
ulong Task::get_deadline() const { return deadline; }
ulong Task::get_period() const { return period; }
bool Task::is_feasible() const { return deadline >= wcet && period >= wcet && wcet > 0; }	

Resource_Requests& Task::get_requests() {	return requests; }
Request& Task::get_request_by_id(uint id)
{
	Request *result = NULL;
	for(uint i = 0; i < requests.size(); i++)
	{
		if(id == requests[i].get_resource_id())
			return requests[i];
	}
	return *result;
}
ulong Task::get_wcet_critical_sections() const { return wcet_critical_sections; }
void Task::set_wcet_critical_sections(ulong csl) { wcet_critical_sections = csl; }
ulong Task::get_wcet_non_critical_sections() const {	return wcet_non_critical_sections; }
void Task::set_wcet_non_critical_sections(ulong ncsl) { wcet_non_critical_sections = ncsl; }
ulong Task::get_spin() const { return spin; }
void Task::set_spin(ulong spining) { spin = spining; }
ulong Task::get_local_blocking() const { return local_blocking; }
void Task::set_local_blocking(ulong lb) { local_blocking = lb; }
ulong Task::get_remote_blocking() const { return remote_blocking; }
void Task::set_remote_blocking(ulong rb) { remote_blocking = rb; }
ulong Task::get_total_blocking() const { return total_blocking; }
void Task::set_total_blocking(ulong tb) { total_blocking = tb; }
ulong Task::get_self_suspension() const { return self_suspension; }
void Task::set_self_suspension(ulong ss) { self_suspension = ss; }
ulong Task::get_jitter() const { return jitter; }
void Task::set_jitter(ulong jit) { jitter = jit; }
ulong Task::get_response_time() const { return response_time; }
void Task::set_response_time(ulong response) { response_time = response; }
uint Task::get_priority() const { return priority; }
void Task::set_priority(uint prio) { priority = prio; }
uint Task::get_partition() const { return partition; }
void Task::set_partition(uint cpu) { partition = cpu; }
uint Task::get_cluster() const { return cluster; }
void Task::set_cluster(uint clu) { cluster = clu; }
CPU_Set* Task::get_affinity() const { return affinity; }
void Task::set_affinity(CPU_Set* affi) { affinity = affi; }
bool Task::is_independent() const { return independent; }
void Task::set_dependent() { independent = false; }
bool Task::is_carry_in() const { return carry_in; }
void Task::set_carry_in() { carry_in = true; }
void Task::clear_carry_in() { carry_in = false; }

/////////////////////////////TaskSet///////////////////////////////

TaskSet::TaskSet()
{
	utilization_sum = 0;
	utilization_max = 0;
	density_sum = 0;
	density_max = 0;
}

TaskSet::~TaskSet()
{
	tasks.clear();
}

void TaskSet::init()
{
	for(uint i = 0; i < tasks.size(); i++)
		tasks[i].init();
}

fraction_t TaskSet::get_utilization_sum() const
{
	return utilization_sum;
}

fraction_t TaskSet::get_utilization_max() const
{
	return utilization_max;
}

fraction_t TaskSet::get_density_sum() const
{
	return density_sum;
}

fraction_t TaskSet::get_density_max() const
{
	return density_max;
}

void TaskSet::add_task(long wcet, long period, long deadline)
{
	fraction_t utilization_new = wcet, density_new = wcet;
	utilization_new /= period;
	if(0 == deadline)
		density_new /= period;
	else
		density_new /= min(deadline, period);
	tasks.push_back(Task(tasks.size(), wcet, period, deadline));
	utilization_sum += utilization_new;
	density_sum += density_new;
	if(utilization_max < utilization_new)
		utilization_max = utilization_new;
	if(density_max < density_new)
		density_max = density_new;
}

void TaskSet::add_task(ResourceSet& resourceset, Param param, long wcet, long period, long deadline)
{
	fraction_t utilization_new = wcet, density_new = wcet;
	utilization_new /= period;
	if(0 == deadline)
		density_new /= period;
	else
		density_new /= min(deadline, period);
	tasks.push_back(Task(tasks.size(),
			resourceset,
			param,
			wcet, 
			period,
			deadline));
	utilization_sum += utilization_new;
	density_sum += density_new;
	if(utilization_max < utilization_new)
		utilization_max = utilization_new;
	if(density_max < density_new)
		density_max = density_new;
}

void TaskSet::calculate_spin(ResourceSet& resourceset, ProcessorSet& processorset)
{
	ulong spinning = 0;
	for(uint i = 0; i < tasks.size(); i++)
	{
		Task &task_i = tasks[i];
//		cout<<"request num:"<<task_i.get_requests().size()<<endl;
		for(uint j = 0; j < task_i.get_requests().size(); j++)
		{
			Request &request = task_i.get_requests()[j];
			uint id = request.get_resource_id();
			uint num = request.get_num_requests();
			ulong Sum = 0;
			for(uint processor_id = 0; processor_id < processorset.get_processor_num(); processor_id++)
			{
				if(processor_id != task_i.get_partition())
				{
					Processor &processor = processorset.get_processors()[processor_id];
					TaskQueue &queue = processor.get_taskqueue();
					list<void*>::iterator it = queue.begin();
					ulong max_length = 0;
					for(uint k = 0; it != queue.end(); it++, k++)
					{
						Task* task_k = (Task*)*it;
						Request &request_k = task_k->get_request_by_id(id);
						if(&request_k)
						{	
							if(max_length < request_k.get_max_length())
								max_length = request_k.get_max_length();
						}
					}
					Sum += max_length;
				}
			}
			spinning += num*Sum;
		}
		task_i.set_spin(spinning);
	}
}

void TaskSet::calculate_local_blocking(ResourceSet& resourceset)
{
	for(uint i = 0; i < tasks.size(); i++)
	{
		Task &task_i = tasks[i];
		ulong lb = 0;
		for(uint j = 0; j < task_i.get_requests().size(); j++)
		{
			Request &request_i = task_i.get_requests()[j];
			uint id = request_i.get_resource_id();
			if(resourceset.get_resources()[id].get_ceiling() <= i)
			{
				for(uint k = task_i.get_id() + 1; k < tasks.size(); k++)
				{
					Task &task_k = tasks[k];
					Request &request_k = task_k.get_request_by_id(id);
					if(&request_k)
					{
						lb = max(lb, request_k.get_max_length());
					}
				}
			}
		}
		task_i.set_local_blocking(lb);
	}
}

void TaskSet::get_utilization_sum(fraction_t &utilization_sum) const
{
	fraction_t temp;
	utilization_sum = 0;
	for(int i = 0; i < tasks.size(); i++)
	{
		temp = tasks[i].get_wcet();
		temp /= tasks[i].get_period();
		utilization_sum += temp;
	}
}
void TaskSet::get_utilization_max(fraction_t &utilization_max) const
{
	utilization_max = tasks[0].get_utilization();
	for(int i = 1; i < tasks.size(); i++)
		if(tasks[i].get_utilization() > utilization_max)
			utilization_max = tasks[i].get_utilization();
}
void TaskSet::get_density_sum(fraction_t &density_sum) const
{
	fraction_t temp;
	density_sum = 0;
	for(int i = 0; i < tasks.size(); i++)
	{
		temp = tasks[i].get_wcet();
		temp /= std::min(tasks[i].get_deadline(),tasks[i].get_period());
		density_sum += temp;
	}
}
void TaskSet::get_density_max(fraction_t &density_max) const
{
	density_max = tasks[0].get_density();
	for(int i = 1; i < tasks.size(); i++)
		if(tasks[i].get_density() > density_max)
			density_max = tasks[i].get_density();
}

Tasks& TaskSet::get_tasks()
{
	return tasks;
}

Task& TaskSet::get_task_by_id(uint id)
{
	return tasks[id];
}

bool TaskSet::is_implicit_deadline()
{
	foreach_condition(tasks,tasks[i].get_deadline() != tasks[i].get_period());
	return true;
}
bool TaskSet::is_constrained_deadline()
{
	foreach_condition(tasks,tasks[i].get_deadline() > tasks[i].get_period());
	return true;
}
bool TaskSet::is_arbitary_deadline()
{
	return !(is_implicit_deadline())&&!(is_constrained_deadline());
}
uint TaskSet::get_taskset_size() const 
{
	return tasks.size();
}

fraction_t TaskSet::get_task_utilization(uint index) const
{
	return tasks[index].get_utilization();
}
fraction_t TaskSet::get_task_density(uint index) const
{
	return tasks[index].get_density();
}
ulong TaskSet::get_task_wcet(uint index) const
{
	return tasks[index].get_wcet();
}
ulong TaskSet::get_task_deadline(uint index) const
{
	return tasks[index].get_deadline();
}
ulong TaskSet::get_task_period(uint index) const
{
	return tasks[index].get_period();
}

void TaskSet::sort_by_period()
{
	sort(tasks.begin(), tasks.end(), period_increase<Task>);
	for(int i = 0; i < tasks.size(); i++)
		tasks[i].set_id(i);
}

void TaskSet::sort_by_utilization()
{
	sort(tasks.begin(), tasks.end(), utilization_decrease<Task>);
	for(int i = 0; i < tasks.size(); i++)
		tasks[i].set_id(i);
}

void TaskSet::RM_Order()
{
	sort_by_period();
	for(uint i = 0; i < tasks.size(); i++)
	{
		tasks[i].set_priority(i);
	}
}

void TaskSet::display()
{
	for(int i = 0; i < tasks.size(); i++)
	{
		cout<<"Task id:"<<tasks[i].get_id()<<" Task period:"<<tasks[i].get_period()<<endl;
	}
}

////////////////////////////DAG Tasks//////////////////////////////

DAG_Task::DAG_Task(uint task_id, ulong period, ulong deadline, uint priority):Task(task_id, 0, period, deadline, priority)
{
	len = 0;
	vol = 0;
	if(0 == deadline)
		this->deadline = period;
	this->period = period;
	vexnum = 0;
	arcnum = 0;
	spin = 0;
	self_suspension = 0;
	local_blocking = 0;
	remote_blocking = 0;
	total_blocking = 0;
	jitter = 0;
	response_time = 0;
	priority = priority;
	partition = 0XFFFFFFFF;
	utilization = 0;
	density = 0;

}

DAG_Task::DAG_Task(	uint task_id,
					ResourceSet& resourceset,
					Param param,
					ulong vol, 
					ulong period,
					ulong deadline,
					uint priority):Task(	task_id,
											resourceset,
											param,
											vol, 
											period,
											deadline,
											priority)
{
	this->len = 0;
	this->vol = vol;
	if(0 == deadline)
		this->deadline = period;
	else
		this->deadline = deadline;
	this->period = period;
	vexnum = 0;
	arcnum = 0;
	spin = 0;
	self_suspension = 0;
	local_blocking = 0;
	remote_blocking = 0;
	total_blocking = 0;
	jitter = 0;
	response_time = 0;
	priority = 0;
	partition = 0XFFFFFFFF;
	utilization = vol;
	utilization /= period;
	density = 0;

	//Generate basic graph
	uint JobNode_num = Random_Gen::uniform_integral_gen(int(max(1, int(param.job_num_range.min))),int(param.job_num_range.max));
	if(JobNode_num > vol)
		JobNode_num = vol;
	graph_gen(vnodes, arcnodes, param, JobNode_num);

	//Insert conditional branches
	for(uint i = 1; i < vnodes.size() - 1; i++)
	{
		if(Random_Gen::probability(param.cond_prob))
		{
			uint cond_job_num = Random_Gen::uniform_integral_gen(2,max(2,param.max_cond_branch));
			vector<VNode> v;
			vector<ArcNode> a;
			sub_graph_gen(v, a, cond_job_num, G_TYPE_C);
			graph_insert(v, a, i);
			break;
		}
	}
	
	//Alocate wcet
	uint job_node_num = 0;
	for(uint i = 0; i < vnodes.size(); i++)
	{
		if(J_NODE == vnodes[i].type)
			job_node_num++;
	}
	vector<ulong> wcets;
	wcets.push_back(0);
	for(uint i = 1; i < job_node_num; i++)
	{
		bool test;
		ulong temp;
		do
		{
			test = false;
			temp = Random_Gen::uniform_ulong_gen(1,vol - 1);
			for(uint j = 0; j < wcets.size(); j++)
				if(temp == wcets[j])
					test = true;
		}
		while(test);
		wcets.push_back(temp);
	}
	wcets.push_back(vol);
	sort(wcets.begin(), wcets.end());
	for(uint i = 0, j = 0; i < vnodes.size(); i++)
	{
		if(J_NODE == vnodes[i].type)
		{
			vnodes[i].wcet = wcets[j + 1] - wcets[j];
			j++;
		}
		vnodes[i].deadline = deadline;
	}
//	display_vertices();
//	display_arcs();
	refresh_relationship();
	update_vol();
	update_len();
	density = len;
	density /= deadline;
}

void DAG_Task::graph_gen(vector<VNode> &v, vector<ArcNode> &a, Param param, uint n_num, double arc_density)
{
	v.clear();
	a.clear();
//creating vnodes
	VNode polar_start, polar_end;
	polar_start.job_id = 0;
	polar_end.job_id = n_num + 1;
	polar_start.type = P_NODE|S_NODE;
	polar_end.type = P_NODE|E_NODE;
	polar_start.pair = polar_end.job_id;
	polar_end.pair = polar_start.job_id;
	polar_start.wcet = 0;
	polar_end.wcet = 0;

	v.push_back(polar_start);
	for(uint i = 0; i < n_num; i++)
	{
		VNode temp_node;
		temp_node.job_id = v.size();
		temp_node.type = J_NODE;
		temp_node.pair = MAX_INT;
		temp_node.wcet = 0;
		v.push_back(temp_node);
	}
	v.push_back(polar_end);
//creating arcs
	uint ArcNode_num;	
	if(param.is_cyclic)//cyclic graph
		ArcNode_num = Random_Gen::uniform_integral_gen(0, n_num * (n_num - 1));
	else//acyclic graph
		ArcNode_num = Random_Gen::uniform_integral_gen(0, (n_num * (n_num - 1)) / 2);
	for(uint i = 0; i < ArcNode_num; i++)
	{
		uint tail, head, temp;	
		do
		{
			if(param.is_cyclic)//cyclic graph
			{
				tail = Random_Gen::uniform_integral_gen(1, n_num);
				head = Random_Gen::uniform_integral_gen(1, n_num);
				if(tail == head)
					continue;
			}
			else//acyclic graph
			{
				tail = Random_Gen::uniform_integral_gen(1, n_num);
				head = Random_Gen::uniform_integral_gen(1, n_num);
				if(tail == head)
					continue;
				else if(tail > head)
				{
					temp = tail;
					tail = head;
					head = temp;
				}
			}

			if(is_arc_exist(a, tail, head))
			{
				continue;
			}
			break;
		}
		while(true);
		add_arc(a, tail, head);	
	}
	refresh_relationship(v, a);
	for(uint i = 1; i <= n_num; i++)
	{
		if(0 == v[i].precedences.size())
			add_arc(a, 0, i);
		if(0 == v[i].follow_ups.size())
			add_arc(a, i, n_num + 1);
	}
	refresh_relationship(v, a);
}

void DAG_Task::sub_graph_gen(vector<VNode> &v, vector<ArcNode> &a, uint n_num, int G_TYPE)
{
//cout<<"Insert sub graph."<<endl;
	v.clear();
	a.clear();

//creating vnodes
	VNode polar_start, polar_end;
	polar_start.job_id = 0;
	polar_end.job_id = n_num + 1;
	if(G_TYPE == G_TYPE_P)
	{
		polar_start.type = P_NODE|S_NODE;
		polar_end.type = P_NODE|E_NODE;
	}
	else
	{
		polar_start.type = C_NODE|S_NODE;
		polar_end.type = C_NODE|E_NODE;
	}
	polar_start.pair = polar_end.job_id;
	polar_end.pair = polar_start.job_id;
	polar_start.wcet = 0;
	polar_end.wcet = 0;

	v.push_back(polar_start);
	for(uint i = 0; i < n_num; i++)
	{
		VNode temp_node;
		temp_node.job_id = v.size();
		temp_node.type = J_NODE;
		temp_node.pair = MAX_INT;
		temp_node.wcet = 0;
		v.push_back(temp_node);
	}
	v.push_back(polar_end);
//creating arcs
	for(uint i = 1; i <= n_num; i++)
	{
		if(0 == v[i].precedences.size())
			add_arc(a, 0, i);
		if(0 == v[i].follow_ups.size())
			add_arc(a, i, n_num + 1);
	}
	refresh_relationship(v, a);
}

void DAG_Task::sequential_graph_gen(vector<VNode> &v, vector<ArcNode> &a, uint n_num)
{
	v.clear();
	a.clear();
	for(uint i = 0; i < n_num; i++)
	{
		VNode temp_node;
		temp_node.job_id = v.size();
		temp_node.type = J_NODE;
		temp_node.pair = MAX_INT;
		temp_node.wcet = 0;
		v.push_back(temp_node);
	}
	for(uint i = 0; i< n_num - 1; i++)
	{
		add_arc(a, i, i + 1);
	}
	refresh_relationship(v, a);
}

void DAG_Task::graph_insert(vector<VNode> &v, vector<ArcNode> &a, uint replace_node)
{
	if(replace_node >= vnodes.size())
	{
		cout<<"Out of bound."<<endl;
		return;
	}
	if(J_NODE != vnodes[replace_node].type)
	{
		cout<<"Only job node could be replaced."<<endl;
		return;
	}
	uint v_num = v.size();
	uint a_num = a.size();
	for(uint i = 0; i < v_num; i++)
	{
		v[i].job_id += replace_node;
		cout<<v[i].job_id<<endl;
		if(MAX_INT != v[i].pair)
			v[i].pair += replace_node;
	}
	int gap = v_num - 1;
	for(uint i = replace_node + 1; i < vnodes.size(); i++)
	{
		vnodes[i].job_id += gap;
	}
	for(uint i = 0; i < vnodes.size(); i++)
	{
		if((vnodes[i].pair > replace_node) && (MAX_INT != vnodes[i].pair))
			vnodes[i].pair += gap;
	}
	for(uint i = 0; i < arcnodes.size(); i++)
	{
		if(arcnodes[i].tail >= replace_node)
			arcnodes[i].tail += gap;
		if(arcnodes[i].head > replace_node)
			arcnodes[i].head += gap;
	}
	vnodes.insert(vnodes.begin() + replace_node + 1, v.begin(), v.end());
	vnodes.erase(vnodes.begin() + replace_node);
	vector<ArcNode>::iterator it2 = arcnodes.begin();
	for(uint i = 0; i < a_num; i++)
	{
		a[i].tail += replace_node;
		a[i].head += replace_node;
	}
	for(vector<ArcNode>::iterator it = a.begin(); it < a.end(); it++)
		arcnodes.push_back(*it);
	
	refresh_relationship();
}
	

uint DAG_Task::get_id() const {return task_id;}
void DAG_Task::set_id(uint id) { task_id = id;}
uint DAG_Task::get_vnode_num() const {return vnodes.size();}
uint DAG_Task::get_arcnode_num() const {return arcnodes.size();}
vector<VNode>& DAG_Task::get_vnodes() {return vnodes;}
VNode& DAG_Task::get_vnode_by_id(uint job_id) {return vnodes[job_id];}
ulong DAG_Task::get_deadline() const {return deadline;}
ulong DAG_Task::get_period() const {return period;}
ulong DAG_Task::get_vol() const {return vol;}
ulong DAG_Task::get_len() const {return len;}
fraction_t DAG_Task::get_utilization() const {return utilization;}
fraction_t DAG_Task::get_density() const {return density;}

void DAG_Task::add_job(ulong wcet, ulong deadline)
{
	VNode vnode;
	vnode.job_id = vnodes.size();
	vnode.wcet = wcet;
	if(0 == deadline)
		vnode.deadline = this->deadline;
	vnode.level = 0;
	vnodes.push_back(vnode);
	//update_vol();
	//update_len();
}
		
void DAG_Task::add_job(vector<VNode> &v, ulong wcet, ulong deadline)
{
	VNode vnode;
	vnode.job_id = v.size();
	vnode.wcet = wcet;
	if(0 == deadline)
		vnode.deadline = this->deadline;
	vnode.level = 0;
	v.push_back(vnode);
}

void DAG_Task::add_arc(uint tail, uint head)
{
	ArcNode arcnode;
	arcnode.tail = tail;
	arcnode.head = head;
	arcnodes.push_back(arcnode);
}

void DAG_Task::add_arc(vector<ArcNode> &a, uint tail, uint head)
{
	ArcNode arcnode;
	arcnode.tail = tail;
	arcnode.head = head;
	a.push_back(arcnode);
}


void DAG_Task::delete_arc(uint tail, uint head)
{
	
}

void DAG_Task::refresh_relationship()
{
	sort(arcnodes.begin(), arcnodes.end(), arcs_increase<ArcNode>);
	for(uint i = 0; i < vnodes.size(); i++)
	{
		vnodes[i].precedences.clear();
		vnodes[i].follow_ups.clear();
	}
	for(uint i = 0; i < arcnodes.size(); i++)
	{
		vnodes[arcnodes[i].tail].follow_ups.push_back(&arcnodes[i]);
		vnodes[arcnodes[i].head].precedences.push_back(&arcnodes[i]);
	}
}

void DAG_Task::refresh_relationship(vector<VNode> &v, vector<ArcNode> &a)
{
	sort(a.begin(), a.end(), arcs_increase<ArcNode>);
	for(uint i = 0; i < v.size(); i++)
	{
		v[i].precedences.clear();
		v[i].follow_ups.clear();
	}
	for(uint i = 0; i < a.size(); i++)
	{
		v[a[i].tail].follow_ups.push_back(&a[i]);
		v[a[i].head].precedences.push_back(&a[i]);
	}
}

void DAG_Task::update_vol()
{
	vol = 0;
	for(int i = 0; i < vnodes.size(); i++)
		vol += vnodes[i].wcet;
}

void DAG_Task::update_len()
{
	len = 0;
	for(uint i = 0; i < vnodes.size(); i++)
	{
		if(0 == vnodes[i].precedences.size())//finding the head
		{
			ulong temp = 0;
			for(uint j = 0; j < vnodes[i].follow_ups.size(); j++)
			{
				ulong temp2 = vnodes[i].wcet + DFS(vnodes[vnodes[i].follow_ups[j]->head]);
				if(temp < temp2)
					temp = temp2;
			}
			if(len < temp)
				len = temp;
		}
	}
	//cout<<"len:"<<len<<endl;
}

bool DAG_Task::is_acyclic()
{
	VNodePtr job = &vnodes[0];
	
}

ulong DAG_Task::DFS(VNode vnode)
{
//cout<<"node id:"<<vnode.job_id<<endl;
	ulong result = 0;
	if(0 == vnode.follow_ups.size())
		result = vnode.wcet;
	else
		for(uint i = 0; i < vnode.follow_ups.size(); i++)
		{	
//cout<<"head"<<vnode.follow_ups[i]->head<<endl;
			ulong temp = vnode.wcet + DFS(vnodes[vnode.follow_ups[i]->head]);
			if(result < temp)
				result = temp;
		}
//cout<<"max len from this node:"<<result<<endl;
	return result;
}

ulong DAG_Task::BFS(VNode vnode)
{
	
}

bool DAG_Task::is_arc_exist(uint tail, uint head)
{
	for(uint i = 0; i < arcnodes.size(); i++)
	{
		if(tail == arcnodes[i].tail)
			if(head == arcnodes[i].head)
				return true;
	}
	return false;
}

bool DAG_Task::is_arc_exist(const vector<ArcNode> &a, uint tail, uint head)
{
	for(uint i = 0; i < a.size(); i++)
	{
		if(tail == a[i].tail)
			if(head == a[i].head)
				return true;
	}
	return false;
}

void DAG_Task::display_vertices()
{
	cout<<"display main vertices:"<<endl;
	for(uint i = 0; i < vnodes.size(); i++)
	{
		cout<<vnodes[i].job_id<<":"<<vnodes[i].wcet<<":"<<vnodes[i].type;		
		if(MAX_INT == vnodes[i].pair)
			cout<<endl;
		else
			cout<<":"<<vnodes[i].pair<<endl;	
	}
}

void DAG_Task::display_vertices(vector<VNode> v)
{
	cout<<"display vertices:"<<endl;
	for(uint i = 0; i < v.size(); i++)
	{
		cout<<v[i].job_id<<":"<<v[i].type;
		if(MAX_INT == v[i].pair)
			cout<<endl;
		else
			cout<<":"<<v[i].pair<<endl;			
	}
}

void DAG_Task::display_arcs()
{
	cout<<"display main arcs:"<<endl;
	for(uint i = 0; i < arcnodes.size(); i++)
	{
		cout<<arcnodes[i].tail<<"--->"<<arcnodes[i].head<<endl;
	}
}

void DAG_Task::display_arcs(vector<ArcNode> a)
{
	cout<<"display arcs:"<<endl;
	for(uint i = 0; i < a.size(); i++)
	{
		cout<<a[i].tail<<"--->"<<a[i].head<<endl;
		//cout<<&a[i]<<endl;
	}
}

void DAG_Task::display_follow_ups(uint job_id)
{
	for(uint i = 0; i < vnodes[job_id].follow_ups.size(); i++)
	{
		cout<<"follow up of node "<<job_id<<":"<<vnodes[job_id].follow_ups[i]->head<<endl;
	}
}

void DAG_Task::display_precedences(uint job_id)
{
	for(uint i = 0; i < vnodes[job_id].precedences.size(); i++)
		cout<<"precedences of node "<<job_id<<":"<<vnodes[job_id].precedences[i]->tail<<endl;
}

uint DAG_Task::get_indegrees(uint job_id) const {return vnodes[job_id].precedences.size();}
uint DAG_Task::get_outdegrees(uint job_id) const {return vnodes[job_id].follow_ups.size();}

////////////////////////////DAG TaskSet////////////////////////////

DAG_TaskSet::DAG_TaskSet()
{
	utilization_sum = 0;
	utilization_max = 0;
	density_sum = 0;
	density_max = 0;
}

void DAG_TaskSet::add_task(ResourceSet& resourceset, Param param, long wcet, long period, long deadline)
{
	uint task_id = dag_tasks.size();
	dag_tasks.push_back(DAG_Task(	task_id,
									resourceset,
									param,
									wcet, 
									period,
									deadline));
	utilization_sum += get_task_by_id(task_id).get_utilization();
	density_sum += get_task_by_id(task_id).get_density();
	if(utilization_max < get_task_by_id(task_id).get_utilization())
		utilization_max = get_task_by_id(task_id).get_utilization();
	if(density_max < get_task_by_id(task_id).get_density())
		density_max = get_task_by_id(task_id).get_density();
	
cout<<"<------------------->"<<endl;
for(uint i = 0; i <= task_id; i++)
{
cout<<"task"<<i<<":"<<endl;
	get_task_by_id(i).refresh_relationship();
	DAG_Task dag_temp = get_task_by_id(i);
	for(uint j = 0; j < dag_temp.get_vnode_num(); j++)
	{
		dag_temp.display_precedences(j);
	}
}
cout<<"<------------------->"<<endl;
}

void DAG_TaskSet::add_task(DAG_Task dag_task)
{
	dag_tasks.push_back(dag_task);
	utilization_sum += dag_task.get_utilization();
	density_sum += dag_task.get_density();
	if(utilization_max < dag_task.get_utilization())
		utilization_max = dag_task.get_utilization();
	if(density_max < dag_task.get_density())
		density_max = dag_task.get_density();
}

DAG_TaskSet::~DAG_TaskSet()
{
	dag_tasks.clear();
}

DAG_Tasks& DAG_TaskSet::get_tasks()
{
	return dag_tasks;
}

DAG_Task& DAG_TaskSet::get_task_by_id(uint id)
{
	return dag_tasks[id];
}

uint DAG_TaskSet::get_taskset_size() const
{
	return dag_tasks.size();
}

fraction_t DAG_TaskSet::get_utilization_sum() const {return utilization_sum;}
fraction_t DAG_TaskSet::get_utilization_max() const {return utilization_max;}
fraction_t DAG_TaskSet::get_density_sum() const {return density_sum;}
fraction_t DAG_TaskSet::get_density_max() const {return density_max;}

void DAG_TaskSet::sort_by_period()
{
	sort(dag_tasks.begin(), dag_tasks.end(), period_increase<DAG_Task>);
	for(int i = 0; i < dag_tasks.size(); i++)
		dag_tasks[i].set_id(i);
}

//////////////////////////////Others//////////////////////////////

void tast_gen(TaskSet& taskset, ResourceSet& resourceset, Param param, double utilization)
{
	//Random_Gen r;
	while(taskset.get_utilization_sum() < utilization)//generate tasks
	{
		ulong period = Random_Gen::uniform_integral_gen(int(param.p_range.min),int(param.p_range.max));
		fraction_t u = Random_Gen::exponential_gen(param.lambda);
		//fraction_t u = Random_Gen::uniform_real_gen(0.05, 0.1);
		ulong wcet = period*u.get_d();
		if(0 == wcet)
			wcet++;
		else if(wcet > period)
			wcet = period;
		ulong deadline = 0;
		if(fabs(param.d_range.max) > _EPS)
		{
			deadline = ceil(period*Random_Gen::uniform_real_gen(param.d_range.min, param.d_range.max));
			//if(deadline > period)
				//deadline = period;
			if(deadline < wcet)
				deadline = wcet;
		}
//		cout<<"wcet:"<<wcet<<" deadling:"<<deadline<<" period:"<<period<<endl;
		fraction_t temp(wcet, period);
		if(taskset.get_utilization_sum() + temp > utilization)
		{
			temp = utilization - taskset.get_utilization_sum();			
			wcet = period*temp.get_d() + 1;
			if(deadline != 0 && deadline < wcet)
				deadline = wcet;
			//taskset->add_task(wcet, period);
			taskset.add_task(resourceset, param, wcet, period, deadline);
			break;
		}
		//taskset->add_task(wcet,period);	
		taskset.add_task(resourceset, param, wcet, period, deadline);
	}
	taskset.sort_by_period();
	//cout<<utilization<<":"<<taskset.get_utilization_sum().get_d()<<endl;
}

void dag_task_gen(DAG_TaskSet& dag_taskset, ResourceSet& resourceset, Param param, double utilization)
{
cout<<"DAG_Task generation, utilization:"<<utilization<<endl;
	while(dag_taskset.get_utilization_sum() < utilization)//generate tasks
	{
		ulong period = Random_Gen::uniform_integral_gen(int(param.p_range.min),int(param.p_range.max));
		fraction_t u = Random_Gen::exponential_gen(param.lambda);
		ulong wcet = period*u.get_d();
		if(0 == wcet)
			wcet++;
		else if(wcet > period)
			wcet = period;
		ulong deadline = 0;
		if(fabs(param.d_range.max) > _EPS)
		{
			deadline = ceil(period*Random_Gen::uniform_real_gen(param.d_range.min, param.d_range.max));
			//if(deadline > period)
				//deadline = period;
			if(deadline < wcet)
				deadline = wcet;
		}
		cout<<"wcet:"<<wcet<<" deadline:"<<deadline<<" period:"<<period<<endl;
		fraction_t temp(wcet, period);
		if(dag_taskset.get_utilization_sum() + temp > utilization)
		{
			temp = utilization - dag_taskset.get_utilization_sum();			
			wcet = period*temp.get_d() + 1;
			if(deadline != 0 && deadline < wcet)
				deadline = wcet;
			dag_taskset.add_task(resourceset, param, wcet, period, deadline);
			break;
		}
		dag_taskset.add_task(resourceset, param, wcet, period, deadline);
	}
	//dag_taskset.sort_by_period();
/*
	double sum = 0;
	for(uint i = 0; i < dag_taskset.get_taskset_size(); i++)
	{
		DAG_Task temp = dag_taskset.get_task_by_id(i);
		double t = temp.get_utilization().get_d();
		sum += t;
		cout<<"Task"<<i<<":"<<t<<endl;
		cout<<"Sum:"<<sum<<endl;
	}
*/
}

ulong gcd(ulong a, ulong b)
{
	ulong temp;
	while(b)
	{
		temp = a;
		a = b;
		b = temp % b;		
	}
	return a;
}

ulong lcm(ulong a, ulong b)
{
	return a*b/gcd(a,b);
}









