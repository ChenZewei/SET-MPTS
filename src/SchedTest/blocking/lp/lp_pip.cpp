#include "lp_pip.h"
#include <tasks.h>
#include <resources.h>
#include <processors.h>
#include <lp.h>
#include <solution.h>
#include <sstream>
#include <iostream>
#include <assert.h>

////////////////////PIPMapper////////////////////
uint64_t PIPMapper::encode_request(uint64_t task_id, uint64_t res_id, uint64_t req_id, uint64_t type)
{
	uint64_t one = 1;
	uint64_t key = 0;
	assert(task_id < (one << 10));
	assert(res_id < (one << 10));
	assert(req_id < (one << 10));
	assert(type < (one << 3));

	key |= (type << 30);
	key |= (task_id << 20);
	key |= (res_id << 10);
	key |= req_id;
	return key;
}

uint64_t PIPMapper::get_type(uint64_t var)
{
	return (var >> 30) & (uint64_t) 0x7; //3 bits
}

uint64_t PIPMapper::get_task(uint64_t var)
{
	return (var >> 20) & (uint64_t) 0x3ff; //10 bits
}

uint64_t PIPMapper::get_res_id(uint64_t var)
{
	return (var >> 10) & (uint64_t) 0x3ff; //10 bits
}

uint64_t PIPMapper::get_req_id(uint64_t var)
{
	return var & (uint64_t) 0x3ff; //10 bits
}

PIPMapper::PIPMapper(uint start_var): VarMapperBase(start_var) {}

uint PIPMapper::lookup(uint task_id, uint res_id, uint req_id, var_type type)
{
	uint64_t key = encode_request(task_id, res_id, req_id, type);
	uint var = var_for_key(key);
	return var;
}

string PIPMapper::key2str(uint64_t key, uint var) const
{
	ostringstream buf;

	switch (get_type(key))
	{
		case PIPMapper::BLOCKING_DIRECT:
			buf << "Xd[";
			break;
		case PIPMapper::BLOCKING_INDIRECT:
			buf << "Xi[";
			break;
		case PIPMapper::BLOCKING_PREEMPT:
			buf << "Xp[";
			break;
		case PIPMapper::BLOCKING_OTHER:
			buf << "Xo[";
			break;
		case PIPMapper::INTERF_REGULAR:
			buf << "Ir[";
			break;
		case PIPMapper::INTERF_CO_BOOSTING:
			buf << "Ic[";
			break;
		case PIPMapper::INTERF_STALLING:
			buf << "Is[";
			break;
		case PIPMapper::INTERF_OTHER:
			buf << "Io[";
			break;
		default:
			buf << "?[";
	}

	buf << get_task(key) << ", "
		<< get_res_id(key) << ", "
		<< get_req_id(key) << "]";

	return buf.str();
}

////////////////////SchedulabilityTest////////////////////
bool is_global_pip_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	bool update;
	do
	{
		update = false;
		foreach(tasks.get_tasks(), ti)
		{
			
			ulong response_time = ti->get_response_time();
		
			ulong temp = get_response_time(*ti, tasks, processors, resources);
cout<<"last response time:"<<response_time<<" current response time:"<<temp<<endl;
			assert(temp >= response_time);
			if(temp > response_time)
			{
				response_time = temp;
				update = true;
			}

			if(response_time < ti->get_deadline())
			{
				ti->set_response_time(response_time);
			}
			else
				return false;
		}
	}
	while(update);
	return true;
}

ulong get_response_time(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	ulong reponse_time = 0;

	PIPMapper vars;
	LinearProgram response_bound;
	LinearExpression *obj = new LinearExpression();

	lp_pip_objective(ti, tasks, processors, response_bound, vars, obj);

	lp_pip_add_constraints(ti, tasks, processors, resources, response_bound, vars);

	GLPKSolution *rb_solution = new GLPKSolution(response_bound, vars.get_num_vars());

	assert(rb_solution != NULL);

	if(rb_solution->is_solved())
	{
cout<<"solved."<<endl;
		reponse_time = lrint(rb_solution->evaluate(*(response_bound.get_objective())));
	}
	else
cout<<"unsolved."<<endl;

#if GLPK_MEM_USAGE_CHECK == 1
	int peak;
	glp_mem_usage(NULL, &peak, NULL, NULL);
	cout<<"Peak memory usage:"<<peak<<endl; 
#endif

	delete rb_solution;
	return reponse_time;
}

////////////////////Expressions////////////////////
void lp_pip_directed_blocking(Task& ti, Task& tx, TaskSet& tasks, PIPMapper& vars, LinearExpression *exp, double coef)
{
	uint x = tx.get_id();
	foreach(tx.get_requests(), request)
	{
		uint q = request->get_resource_id();
		ulong length = request->get_max_length();
		foreach_request_instance(ti, tx, v)
		{
			uint var_id;

			var_id = vars.lookup(x, q, v, PIPMapper::BLOCKING_DIRECT);
			exp->add_term(var_id, coef*length);
		}
	}
}

void lp_pip_indirected_blocking(Task& ti, Task& tx, TaskSet& tasks, PIPMapper& vars, LinearExpression *exp, double coef)
{
	uint x = tx.get_id();
	foreach(tx.get_requests(), request)
	{
		uint q = request->get_resource_id();
		ulong length = request->get_max_length();
		foreach_request_instance(ti, tx, v)
		{
			uint var_id;

			var_id = vars.lookup(x, q, v, PIPMapper::BLOCKING_INDIRECT);
			exp->add_term(var_id, coef*length);
		}
	}
}

void lp_pip_preemption_blocking(Task& ti, Task& tx, TaskSet& tasks, PIPMapper& vars, LinearExpression *exp, double coef)
{
	uint x = tx.get_id();
	foreach(tx.get_requests(), request)
	{
		uint q = request->get_resource_id();
		ulong length = request->get_max_length();
		foreach_request_instance(ti, tx, v)
		{
			uint var_id;

			var_id = vars.lookup(x, q, v, PIPMapper::BLOCKING_PREEMPT);
			exp->add_term(var_id, coef*length);
		}
	}
}


void lp_pip_OD(Task& ti, TaskSet& tasks, ProcessorSet& processors, PIPMapper& vars, LinearExpression *exp, double coef)
{
	uint p_num = processors.get_processor_num();
	uint var_id;

	foreach_higher_priority_task(tasks.get_tasks(), ti, th)
	{
		uint h = th->get_id();
		
		var_id = vars.lookup(h, 0, 0, PIPMapper::INTERF_REGULAR);
		exp->add_term(var_id, coef*(1/p_num));
	}

	foreach_lower_priority_task(tasks.get_tasks(), ti, tl)
	{
		uint l = tl->get_id();
		
		var_id = vars.lookup(l, 0, 0, PIPMapper::INTERF_CO_BOOSTING);
		exp->add_term(var_id, coef*(1/p_num));
		
		var_id = vars.lookup(l, 0, 0, PIPMapper::INTERF_STALLING);
		exp->add_term(var_id, coef*(1/p_num));

		lp_pip_indirected_blocking(ti, *tl, tasks, vars, exp, coef*(1/p_num));

		lp_pip_preemption_blocking(ti, *tl, tasks, vars, exp, coef*(1/p_num));
	}
}
/*
void lp_pip_objective(Task& ti, TaskSet& tasks, ProcessorSet& processors, LinearProgram& lp, PIPMapper& vars, LinearExpression *obj)
{
	uint p_num = processors.get_processor_num();
	uint var_id;

	foreach_higher_priority_task(tasks.get_tasks(), ti, th)
	{
		uint h = th->get_id();
		
		var_id = vars.lookup(h, 0, 0, PIPMapper::INTERF_REGULAR);
		obj->add_term(var_id, 1/p_num);
	}

	foreach_lower_priority_task(tasks.get_tasks(), ti, tl)
	{
		uint l = tl->get_id();
		
		var_id = vars.lookup(l, 0, 0, PIPMapper::INTERF_CO_BOOSTING);
		obj->add_term(var_id, 1/p_num);
		
		var_id = vars.lookup(l, 0, 0, PIPMapper::INTERF_STALLING);
		obj->add_term(var_id, 1/p_num);

		lp_pip_indirected_blocking(ti, *tl, tasks, vars, obj, 1/p_num);

		lp_pip_preemption_blocking(ti, *tl, tasks, vars, obj, 1/p_num);
	}

	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		lp_pip_directed_blocking(ti, *tx, tasks, vars, obj, 1/p_num);
	}
	
	vars.seal();
}
*/
void lp_pip_objective(Task& ti, TaskSet& tasks, ProcessorSet& processors, LinearProgram& lp, PIPMapper& vars, LinearExpression *obj)
{
	uint p_num = processors.get_processor_num();
	uint var_id;

	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		
		var_id = vars.lookup(x, 0, 0, PIPMapper::INTERF_REGULAR);
		obj->add_term(var_id, 1/p_num);

		var_id = vars.lookup(x, 0, 0, PIPMapper::INTERF_CO_BOOSTING);
		obj->add_term(var_id, 1/p_num);
		
		var_id = vars.lookup(x, 0, 0, PIPMapper::INTERF_STALLING);
		obj->add_term(var_id, 1/p_num);
		
		lp_pip_directed_blocking(ti, *tx, tasks, vars, obj, 1/p_num);

		lp_pip_indirected_blocking(ti, *tx, tasks, vars, obj, 1/p_num);

		lp_pip_preemption_blocking(ti, *tx, tasks, vars, obj, 1/p_num);
	}
	
	vars.seal();
}

ulong workload_bound(Task& tx, ulong Ri)
{
	ulong e = tx.get_wcet();
	ulong d = tx.get_deadline();
	ulong p = tx.get_period();
	ulong r = tx.get_response_time();
	assert(d >= r);

	ulong N = (Ri - e + r)/p;

	return N*e + min(e, Ri + r - e - N*p);
}

void lp_pip_add_constraints(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars)
{
	lp_pip_constraint_1(ti, tasks, resources, lp, vars);
	lp_pip_constraint_2(ti, tasks, processors, resources, lp, vars);
	lp_pip_constraint_3(ti, tasks, resources, lp, vars);
	lp_pip_constraint_4(ti, tasks, resources, lp, vars);
	lp_pip_constraint_5(ti, tasks, resources, lp, vars);
}

void lp_pip_constraint_1(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars)
{
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		LinearExpression *exp = new LinearExpression();

		uint var_id;
		uint x = tx->get_id();
				
		var_id = vars.lookup(x, 0, 0, PIPMapper::INTERF_REGULAR);
		exp->add_var(var_id);

		var_id = vars.lookup(x, 0, 0, PIPMapper::INTERF_CO_BOOSTING);
		exp->add_var(var_id);

		var_id = vars.lookup(x, 0, 0, PIPMapper::INTERF_STALLING);
		exp->add_var(var_id);
		
		lp_pip_directed_blocking(ti, *tx, tasks, vars, exp);

		lp_pip_indirected_blocking(ti, *tx, tasks, vars, exp);

		lp_pip_preemption_blocking(ti, *tx, tasks, vars, exp);

		lp.add_inequality(exp, workload_bound(*tx, ti.get_response_time()));
	}
}

void lp_pip_constraint_2(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars)
{
	
	
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		LinearExpression *exp = new LinearExpression();

		uint var_id;
		uint x = tx->get_id();

		var_id = vars.lookup(x, 0, 0, PIPMapper::INTERF_REGULAR);
		exp->add_var(var_id);

		var_id = vars.lookup(x, 0, 0, PIPMapper::INTERF_CO_BOOSTING);
		exp->add_var(var_id);

		var_id = vars.lookup(x, 0, 0, PIPMapper::INTERF_STALLING);
		exp->add_var(var_id);

		lp_pip_indirected_blocking(ti, *tx, tasks, vars, exp);

		lp_pip_preemption_blocking(ti, *tx, tasks, vars, exp);

		lp_pip_OD(ti, tasks, processors, vars, exp, -1);

		lp.add_inequality(exp, 0);
	}
}

void lp_pip_constraint_3(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars)
{
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach(tx->get_requests(), request)
		{
			uint q = request->get_resource_id();
			foreach_request_instance(ti, *tx, v)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;
				
				var_id = vars.lookup(x, q, v, PIPMapper::BLOCKING_DIRECT);
				exp->add_var(var_id);

				var_id = vars.lookup(x, q, v, PIPMapper::BLOCKING_INDIRECT);
				exp->add_var(var_id);

				var_id = vars.lookup(x, q, v, PIPMapper::BLOCKING_PREEMPT);
				exp->add_var(var_id);

				lp.add_inequality(exp, 1);
			}
		}
	}
}

void lp_pip_constraint_4(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars)
{
	if(0 == ti.get_requests().size())
	{
		foreach_lower_priority_task(tasks.get_tasks(), ti, tx)
		{	
			LinearExpression *exp = new LinearExpression();
			uint x = tx->get_id();
			uint var_id;
		
			var_id = vars.lookup(x, 0, 0, PIPMapper::INTERF_STALLING);
			exp->add_var(var_id);

			lp.add_equality(exp, 0);
		}
	}
}

void lp_pip_constraint_5(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars)
{
	LinearExpression *exp = new LinearExpression();
	
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach(tx->get_requests(), request)
		{
			uint q = request->get_resource_id();
			if(!ti.is_request_exist(q))
			{
				foreach_request_instance(ti, *tx, v)
				{
					uint var_id;
				
					var_id = vars.lookup(x, q, v, PIPMapper::BLOCKING_DIRECT);
					exp->add_var(var_id);
				}
			}
		}
	}
	
	lp.add_equality(exp, 0);
}

































