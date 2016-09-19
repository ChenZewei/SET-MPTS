#include "tasks.h"

////////////////////////////Request//////////////////////////////

Request::Request(uint resource_id,
	uint num_requests,
	ulong max_length,
	ulong total_length)
{
	this->resource_id = resource_id;
	this->num_requests = num_requests;
	this->max_length = max_length;
	this->total_length = total_length;
}

uint Request::get_resource_id() const { return resource_id; }
uint Request::get_num_requests() const { return num_requests; }
ulong Request::get_max_length() const { return max_length; }
ulong Request::get_total_length() const { return total_length; }

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
			add_request(i, num, max_len, param.tlf*max_len);
			resourceset.add_task(i, this);
		}
	}
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

void Task::add_request(uint res_id, uint num, ulong max_len, ulong total_len)
{
	requests.push_back(Request(res_id, num, max_len, total_len));
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
					list<Task*>::iterator it = queue.begin();
					ulong max_length = 0;
					for(uint k = 0; it != queue.end(); it++, k++)
					{
						Task* task_k = *it;
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
	foreach(tasks,tasks[i].get_deadline() != tasks[i].get_period());
	return true;
}
bool TaskSet::is_constrained_deadline()
{
	foreach(tasks,tasks[i].get_deadline() > tasks[i].get_period());
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
	sort(tasks.begin(), tasks.end(), period_increase);
	for(int i = 0; i < tasks.size(); i++)
		tasks[i].set_id(i);
}

void TaskSet::sort_by_utilization()
{
	sort(tasks.begin(), tasks.end(), utilization_decrease);
	for(int i = 0; i < tasks.size(); i++)
		tasks[i].set_id(i);
}

void TaskSet::display()
{
	for(int i = 0; i < tasks.size(); i++)
	{
		cout<<"Task id:"<<tasks[i].get_id()<<" Task period:"<<tasks[i].get_period()<<endl;
	}
}

////////////////////////////DAG Tasks//////////////////////////////

DAG_Task::DAG_Task(uint task_id, ulong period, ulong deadline):Task(task_id, 0, period, deadline)
{
	len = 0;
	vol = 0;
	if(0 == deadline)
		this->deadline = period;
	this->period = period;
	vexnum = 0;
	arcnum = 0;
}

ulong DAG_Task::get_vol() const {return vol;}
ulong DAG_Task::get_len() const {return len;}
ulong DAG_Task::get_deadline() const {return deadline;}
ulong DAG_Task::get_period() const {return period;}

void DAG_Task::add_job(uint wcet, ulong deadline)
{
	VNode vnode;
	vnode.id = vnodes.size();
	vnode.wcet = wcet;
	if(0 == deadline)
		vnode.deadline = this->deadline;
	vnode.level = 0;
	vnodes.push_back(vnode);
	//update_vol();
	//update_len();
}

void DAG_Task::add_arc(uint tail, uint head)
{
	ArcNode arcnode;
	arcnode.tail = tail;
	arcnode.head = head;
	arcnodes.push_back(arcnode);
	vnodes[tail].follow_ups.push_back(&arcnodes.back());
	vnodes[head].precedences.push_back(&arcnodes.back());
	
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
			ulong temp = vnodes[i].wcet;
			for(uint j = 0; j < vnodes[i].follow_ups.size(); j++)
			{
				temp += DFS(vnodes[vnodes[i].follow_ups[j]->head]);
			}
			if(len < temp)
				len = temp;
		}
	}
	//cout<<"len:"<<len<<endl;
}

bool DAG_Task::is_acyclic()
{

}

ulong DAG_Task::DFS(VNode vnode)
{
cout<<"node id:"<<vnode.id<<endl;
	ulong result = 0;
	if(0 == vnode.follow_ups.size())
		result = vnode.wcet;
	else
		for(uint i = 0; i < vnode.follow_ups.size(); i++)
		{	
cout<<"head"<<vnode.follow_ups[i]->head<<endl;
			ulong temp = vnode.wcet + DFS(vnodes[vnode.follow_ups[i]->head]);
			if(result < temp)
				result = temp;
		}
cout<<"max len from this node:"<<result<<endl;
	return result;
}

ulong DAG_Task::BFS(VNode vnode)
{

}

void DAG_Task::display_arcs()
{
	for(uint i; i < arcnodes.size(); i++)
	{
		cout<<arcnodes[i].tail<<"--->"<<arcnodes[i].head<<endl;
	}
}

void DAG_Task::display_follow_ups(uint job_id)
{
	for(uint i = 0; i < vnodes[job_id].follow_ups.size(); i++)
		cout<<"follow up of node "<<job_id<<":"<<vnodes[job_id].follow_ups[i]->head<<endl;
}

void DAG_Task::display_precedences(uint job_id)
{
	for(uint i = 0; i < vnodes[job_id].precedences.size(); i++)
		cout<<"precedences of node "<<job_id<<":"<<vnodes[job_id].precedences[i]->tail<<endl;
}

/////////////////////////////Others///////////////////////////////

void tast_gen(TaskSet& taskset, ResourceSet& resourceset, Param param, double utilization)
{
	//Random_Gen r;
	while(taskset.get_utilization_sum() < utilization)//generate tasks
	{
		ulong period = Random_Gen::uniform_integral_gen(int(param.p_range.min),int(param.p_range.max));
		fraction_t u = Random_Gen::exponential_gen(param.lambda);
		ulong wcet = period*u.get_d();
		ulong deadline = 0;
		if(fabs(param.d_range.max) < EPS)
		{
			deadline = ceil(period*Random_Gen::uniform_real_gen(param.d_range.min, param.d_range.max));
			if(deadline > period)
				deadline = period;
			if(deadline < wcet)
				deadline = wcet;
		}
//		cout<<"wcet:"<<wcet<<" deadling:"<<deadline<<" period:"<<period<<endl;
		if(0 == wcet)
			wcet++;
		else if(wcet > period)
			wcet = period;
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









