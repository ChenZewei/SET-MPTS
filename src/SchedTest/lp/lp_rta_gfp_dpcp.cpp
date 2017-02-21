#include "lp_rta_gfp_dpcp.h"
#include "solution.h"
#include <lp.h>
#include <sstream>

////////////////////DPCPMapper////////////////////
uint64_t DPCPMapper::encode_request(uint64_t task_id, uint64_t res_id, uint64_t req_id, uint64_t type)
{
	uint64_t one = 1;
	uint64_t key = 0;
	assert(task_id < (one << 10));
	assert(res_id < (one << 10));
	assert(req_id < (one << 10));
	assert(type < (one << 2));

	key |= (type << 30);
	key |= (task_id << 20);
	key |= (res_id << 10);
	key |= req_id;
	return key;
}

uint64_t DPCPMapper::get_type(uint64_t var)
{
	return (var >> 30) & (uint64_t) 0x3; //2 bits
}

uint64_t DPCPMapper::get_task(uint64_t var)
{
	return (var >> 20) & (uint64_t) 0x3ff; //10 bits
}

uint64_t DPCPMapper::get_res_id(uint64_t var)
{
	return (var >> 10) & (uint64_t) 0x3ff; //10 bits
}

uint64_t DPCPMapper::get_req_id(uint64_t var)
{
	return var & (uint64_t) 0x3ff; //10 bits
}

DPCPMapper::DPCPMapper(uint start_var): VarMapperBase(start_var) {}

uint DPCPMapper::lookup(uint task_id, uint res_id, uint req_id, var_type type)
{
	uint64_t key = encode_request(task_id, res_id, req_id, type);
	uint var = var_for_key(key);
//cout<<"Key:"<<key<<endl;
//cout<<"Var:"<<var<<endl;
	return var;
}

string DPCPMapper::key2str(uint64_t key, uint var) const
{
	ostringstream buf;

	switch (get_type(key))
	{
		case DPCPMapper::BLOCKING_DIRECT:
			buf << "Xd[";
			break;
		case DPCPMapper::BLOCKING_INDIRECT:
			buf << "Xi[";
			break;
		case DPCPMapper::BLOCKING_PREEMPT:
			buf << "Xp[";
			break;
		case DPCPMapper::BLOCKING_OTHER:
			buf << "Xo[";
			break;
		default:
			buf << "X?[";
	}

	buf << get_task(key) << ", "
		<< get_res_id(key) << ", "
		<< get_req_id(key) << "]";

	return buf.str();
}

////////////////////LP_RTA_GFP_DPCP////////////////////
LP_RTA_GFP_DPCP::LP_RTA_GFP_DPCP(): LPSched(RTA, GLOBAL, FIX_PRIORITY, DPCP, "", "DPCP") {}
LP_RTA_GFP_DPCP::LP_RTA_GFP_DPCP(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources): LPSched(RTA, GLOBAL, FIX_PRIORITY, DPCP, "", "DPCP")
{
	this->tasks = tasks;
	this->processors = processors;
	this->resources = resources;
	
	this->tasks.RM_Order();
	this->processors.init();
}

LP_RTA_GFP_DPCP::~LP_RTA_GFP_DPCP() {}

bool LP_RTA_GFP_DPCP::is_schedulable()
{
	
	for(uint t_id = 0; t_id < tasks.get_taskset_size(); t_id++)
	{
		Task& task = tasks.get_task_by_id(t_id);
		ulong response_bound = task.get_response_time();
		if(task.get_partition() == MAX_LONG)
			continue;

		ulong temp = response_time(task);

		assert(temp >= response_bound);
		response_bound = temp;

		if(response_bound <= task.get_deadline())
			task.set_response_time(response_bound);
		else
			return false;
	}
	return true;
}

ulong LP_RTA_GFP_DPCP::local_blocking(Task& task_i)
{
	ulong local_blocking = 0;
	Resources& r = resources.get_resources();	
	Resource_Requests& rr = task_i.get_requests();
	uint p_id = task_i.get_partition();//processor id
	ulong r_i = task_i.get_response_time();//response time of task i(t_id)
	DPCPMapper var;
	LinearProgram local_bound;
	LinearExpression *local_obj = new LinearExpression();
	lp_dpcp_objective(task_i, local_bound, var, local_obj, NULL);
	local_bound.set_objective(local_obj);
//construct constraints
	lp_dpcp_add_constraints(task_i, local_bound, var);

	GLPKSolution *lb_solution = new GLPKSolution(local_bound, var.get_num_vars());

	assert(lb_solution != NULL);

	if(lb_solution->is_solved())
	{
		local_blocking = lrint(lb_solution->evaluate(*(local_bound.get_objective())));
	}

	task_i.set_local_blocking(local_blocking);
	
#if GLPK_MEM_USAGE_CHECK == 1
	int peak;
	glp_mem_usage(NULL, &peak, NULL, NULL);
	cout<<"Peak memory usage:"<<peak<<endl; 
#endif

	delete lb_solution;
	return local_blocking;
}

ulong LP_RTA_GFP_DPCP::remote_blocking(Task& task_i)
{
	ulong remote_blocking = 0;
	Resources& r = resources.get_resources();	
	Resource_Requests& rr = task_i.get_requests();
	uint p_id = task_i.get_partition();//processor id
	ulong r_i = task_i.get_response_time();//response time of task i(t_id)
	DPCPMapper var;
	LinearProgram remote_bound;
	LinearExpression *remote_obj = new LinearExpression();
	lp_dpcp_objective(task_i, remote_bound, var, NULL, remote_obj);
	remote_bound.set_objective(remote_obj);
//construct constraints
	lp_dpcp_add_constraints(task_i, remote_bound, var);

	GLPKSolution *rb_solution = new GLPKSolution(remote_bound, var.get_num_vars());

	if(rb_solution->is_solved())
	{
		remote_blocking = lrint(rb_solution->evaluate(*(remote_bound.get_objective())));
	}

	task_i.set_remote_blocking(remote_blocking);
	
#if GLPK_MEM_USAGE_CHECK == 1
	int peak;
	glp_mem_usage(NULL, &peak, NULL, NULL);
	cout<<"Peak memory usage:"<<peak<<endl; 
#endif

	delete rb_solution;
	return remote_blocking;
}

ulong LP_RTA_GFP_DPCP::total_blocking(Task& task_i)
{	
	ulong total_blocking;
//cout<<"111"<<endl;
	ulong blocking_l = local_blocking(task_i);
//cout<<"222"<<endl;
	ulong blocking_r = remote_blocking(task_i);
//cout<<"333"<<endl;
	total_blocking = blocking_l + blocking_r;
	task_i.set_total_blocking(total_blocking);
	return total_blocking;
}

ulong LP_RTA_GFP_DPCP::interference(Task& task, ulong interval)
{
	return task.get_wcet() * ceiling((interval + task.get_response_time()), task.get_period());
}


ulong LP_RTA_GFP_DPCP::response_time(Task& task_i)
{
	ulong test_end = task_i.get_deadline();
	ulong test_start = task_i.get_total_blocking() + task_i.get_wcet();
	ulong response = test_start;
	ulong demand = 0;
	while (response <= test_end)
	{
		total_blocking(task_i);
		demand = task_i.get_total_blocking() + task_i.get_wcet();

		ulong total_interf = 0;
		for (uint th = 0; th < task_i.get_id(); th ++)
		{
			Task& task_h = tasks.get_task_by_id(th);
			if (task_i.get_partition() == task_h.get_partition())
			{
				total_interf += interference(task_h, response);
			}
		}

		demand += total_interf;

		if (response == demand)
			return response + task_i.get_jitter();
		else 
			response = demand;
	}
	return test_end + 100;
}

ulong LP_RTA_GFP_DPCP::get_max_wait_time(Task& ti, Request& rq)
{
	uint priority = ti.get_priority();
	uint p_id = rq.get_locality();
	ulong L_i_q = rq.get_max_length();
	ulong max_wait_time_l = 0;
	ulong max_wait_time_h = 0;
	ulong max_wait_time = 0;
	

	foreach_lower_priority_task(tasks.get_tasks(), ti, tx)
	{
		foreach(tx->get_requests(), request_v)
		{
			Resource& resource = resources.get_resources()[request_v->get_resource_id()];
			if((resource.get_ceiling() > priority) && (resource.get_locality() == p_id))
			{
				if(max_wait_time_l < request_v->get_max_length())
					max_wait_time_l = request_v->get_max_length();
			}
		}
	}

	max_wait_time = max_wait_time_h + max_wait_time_l + L_i_q;

	while(true)
	{
		ulong temp = 0;
		foreach_higher_priority_task(tasks.get_tasks(), ti, tx)
		{
			ulong request_time = 0;
			foreach(tx->get_requests(), request_y)
			{
				Resource& resource = resources.get_resources()[request_y->get_resource_id()];
				if((resource.get_ceiling() > priority) && (resource.get_locality() == p_id))
				{
					request_time += request_y->get_num_requests() * request_y->get_max_length();
				}
			}
			temp += ceiling(tx->get_response_time() + max_wait_time, tx->get_period()) * request_time;
		}

		assert(temp >= max_wait_time_h);
		if(temp > max_wait_time_h)
		{
			max_wait_time_h = temp;
			max_wait_time = max_wait_time_h + max_wait_time_l + L_i_q;
//cout<<"max_wait_time:"<<max_wait_time<<endl;
		}
		else
		{
			max_wait_time = max_wait_time_h + max_wait_time_l + L_i_q;
//cout<<"max_wait_time:"<<max_wait_time<<endl;
			break;	
		}
	}
	return max_wait_time;
}

void LP_RTA_GFP_DPCP::lp_dpcp_objective(Task& ti, LinearProgram& lp, DPCPMapper& vars, LinearExpression *local_obj, LinearExpression *remote_obj)
{
	//LinearExpression *obj = new LinearExpression();
	
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach(tx->get_requests(), request)
		{
			uint q = request->get_resource_id();
			bool is_local = (request->get_locality() == ti.get_partition());
			ulong length = request->get_max_length();
			foreach_request_instance(ti, *tx, q, v)
			{
				uint var_id;

				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_DIRECT);
				//obj->add_term(var_id, length);
				if (is_local && (local_obj != NULL))
					local_obj->add_term(var_id, length);
				else if (!is_local && (remote_obj != NULL))
					remote_obj->add_term(var_id, length);

				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_INDIRECT);
				//obj->add_term(var_id, length);
				if (is_local && (local_obj != NULL))
					local_obj->add_term(var_id, length);
				else if (!is_local && (remote_obj != NULL))
					remote_obj->add_term(var_id, length);

				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_PREEMPT);
				//obj->add_term(var_id, length);
				if (is_local && (local_obj != NULL))
					local_obj->add_term(var_id, length);
				else if (!is_local && (remote_obj != NULL))
					remote_obj->add_term(var_id, length);
			}
		}
	}
	//delete obj;
	vars.seal();
}
/*
void LP_RTA_GFP_DPCP::lp_dpcp_local_objective(Task& ti, LinearProgram& lp, DPCPMapper& vars)
{
	LinearExpression *obj = new LinearExpression();
	
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach_local_request(ti, tx->get_requests(), request_iter)
		{
			uint q = request_iter->get_resource_id();
			foreach_request_instance(ti, *tx, q, v)
			{
				uint var_id;

				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_DIRECT);
				obj->add_term(var_id, request_iter->get_max_length());

				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_INDIRECT);
				obj->add_term(var_id, request_iter->get_max_length());

				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_PREEMPT);
				obj->add_term(var_id, request_iter->get_max_length());
			}
		}
	}
	
	lp.set_objective(obj);
	delete obj;
	vars.seal();
}

void LP_RTA_GFP_DPCP::lp_dpcp_remote_objective(Task& ti, LinearProgram& lp, DPCPMapper& vars)
{
	LinearExpression *obj = new LinearExpression();
	
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach_remote_request(ti, tx->get_requests(), request_iter)
		{
			uint q = request_iter->get_resource_id();
			foreach_request_instance(ti, *tx, q, v)
			{
				uint var_id;

				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_DIRECT);
				obj->add_var(var_id);
				obj->add_term(var_id, request_iter->get_max_length());

				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_INDIRECT);
				obj->add_var(var_id);
				obj->add_term(var_id, request_iter->get_max_length());

				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_PREEMPT);
				obj->add_var(var_id);
				obj->add_term(var_id, request_iter->get_max_length());
			}
		}
	}

	lp.set_objective(obj);
	delete obj;
	vars.seal();
}
*/
void LP_RTA_GFP_DPCP::lp_dpcp_add_constraints(Task& ti, LinearProgram& lp, DPCPMapper& vars)
{
	lp_dpcp_constraint_1(ti, lp, vars);
	lp_dpcp_constraint_2(ti, lp, vars);
	lp_dpcp_constraint_3(ti, lp, vars);
	lp_dpcp_constraint_4(ti, lp, vars);
	lp_dpcp_constraint_5(ti, lp, vars);
	lp_dpcp_constraint_6(ti, lp, vars);
}

void LP_RTA_GFP_DPCP::lp_dpcp_constraint_1(Task& ti, LinearProgram& lp, DPCPMapper& vars)
{
//cout<<"Constraint 1"<<endl;
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach(tx->get_requests(), request)
		{
			uint q = request->get_resource_id();
			foreach_request_instance(ti, *tx, q, v)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_DIRECT);
				exp->add_var(var_id);
		
				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_INDIRECT);
				exp->add_var(var_id);

				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_PREEMPT);
				exp->add_var(var_id);

				lp.add_inequality(exp, 1);// Xd(x,q,v) + Xi(x,q,v) + Xp(x,q,v) <= 1

			}
		}
	}
}

void LP_RTA_GFP_DPCP::lp_dpcp_constraint_2(Task& ti, LinearProgram& lp, DPCPMapper& vars)
{
	LinearExpression *exp = new LinearExpression();

	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach_remote_request(ti, tx->get_requests(), request_iter)
		{
			uint q = request_iter->get_resource_id();
			foreach_request_instance(ti, *tx, q, v)
			{
				uint var_id;
				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_PREEMPT);
				exp->add_var(var_id);
			}
		}
	}
	lp.add_equality(exp, 0);
}

void LP_RTA_GFP_DPCP::lp_dpcp_constraint_3(Task& ti, LinearProgram& lp, DPCPMapper& vars)
{
	uint t_id = ti.get_id();	
	uint max_arrival = 1;

	foreach_remote_request(ti, ti.get_requests(), request)
	{
		max_arrival += request->get_num_requests();
	}
	
	foreach_lower_priority_local_task(tasks.get_tasks(), ti, tx)
	{
		LinearExpression *exp = new LinearExpression();
		uint x = tx->get_id();
		foreach_local_request(ti, tx->get_requests(), request)
		{
			uint q = request->get_resource_id();
			foreach_request_instance(ti, *tx, q, v)
			{
				uint var_id;
				var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_PREEMPT);
				exp->add_var(var_id);
			}
		}
		lp.add_inequality(exp, max_arrival);
	}
}

void LP_RTA_GFP_DPCP::lp_dpcp_constraint_4(Task& ti, LinearProgram& lp, DPCPMapper& vars)
{
	LinearExpression *exp = new LinearExpression();
	uint priority = ti.get_priority();
	
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach(tx->get_requests(), request)
		{
			uint q = request->get_resource_id();
			Resource& resource = resources.get_resources()[q];
			if(resource.get_ceiling() > priority)
			{
				foreach_request_instance(ti, *tx, q, v)
				{
					uint var_id;

					var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_DIRECT);
					exp->add_var(var_id);

					var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_INDIRECT);
					exp->add_var(var_id);
				}
			}
		}
	}
	lp.add_equality(exp, 0);
}

void LP_RTA_GFP_DPCP::lp_dpcp_constraint_5(Task& ti, LinearProgram& lp, DPCPMapper& vars)
{	
	uint priority = ti.get_priority();

	foreach(processors.get_processors(), processor)
	{
		LinearExpression *exp = new LinearExpression();
		
		uint p_id = processor->get_processor_id();
		uint cluster_request = 0;
		foreach(ti.get_requests(), request)
		{
			if(request->get_locality() == p_id)
			{
				cluster_request += request->get_num_requests();
			}
		}

		foreach_lower_priority_task(tasks.get_tasks(), ti, tx)
		{
			uint x = tx->get_id();
			foreach(tx->get_requests(), request)
			{
				uint q = request->get_resource_id();
				Resource& resource = resources.get_resources()[q];
				if((resource.get_ceiling() > priority) && (resource.get_locality() == p_id))
				{
					foreach_request_instance(ti, *tx, q, v)
					{
						uint var_id;

						var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_DIRECT);
						exp->add_var(var_id);

						var_id = vars.lookup(x, q, v, DPCPMapper::BLOCKING_INDIRECT);
						exp->add_var(var_id);
					}
				}
			}
		}

		lp.add_inequality(exp, cluster_request);
	}
}

void LP_RTA_GFP_DPCP::lp_dpcp_constraint_6(Task& ti, LinearProgram& lp, DPCPMapper& vars)
{
	ulong max_wait_time_l = 0;
	ulong max_wait_time_h = 0;
	ulong max_wait_time = 0;

	foreach_higher_priority_task(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach(tx->get_requests(), request_y)
		{
			LinearExpression *exp = new LinearExpression();
			uint y = request_y->get_resource_id();
			ulong max_request_num = 0;
			
			foreach(ti.get_requests(), request_q)
			{
				if(request_q->get_locality() == request_y->get_locality())
				{
					uint N_i_q = request_q->get_num_requests();
					ulong mwt = get_max_wait_time(ti, *request_q);
					ulong D = ceiling(tx->get_response_time() + mwt, tx->get_period()) * request_y->get_num_requests();
					max_request_num += D * N_i_q;
				}
			}

			foreach_request_instance(ti, *tx, y, v)
			{
				uint var_id;

				var_id = vars.lookup(x, y, v, DPCPMapper::BLOCKING_DIRECT);
				exp->add_var(var_id);

				var_id = vars.lookup(x, y, v, DPCPMapper::BLOCKING_INDIRECT);
				exp->add_var(var_id);
			}
			lp.add_inequality(exp, max_request_num);
		}
	}
}


























