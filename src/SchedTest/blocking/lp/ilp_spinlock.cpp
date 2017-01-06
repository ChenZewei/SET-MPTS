#include "ilp_spinlock.h"
#include <tasks.h>
#include <resources.h>
#include <processors.h>
#include <lp.h>
#include <solution.h>
#include <sstream>
#include <iostream>
#include <assert.h>

////////////////////ILPSpinLockMapper////////////////////
static uint64_t ILPSpinLockMapper::encode_request(uint64_t type, uint64_t part_1, uint64_t part_2, uint64_t part_3, uint64_t part_4)
{
	uint64_t one = 1;
	uint64_t key = 0;
	assert(type < (one << 4));
	assert(part_1 < (one << 10));
	assert(part_2 < (one << 10));
	assert(part_3 < (one << 10));
	assert(part_4 < (one << 10));

	key |= (type << 40);
	key |= (part_1 << 30);
	key |= (part_2 << 20);
	key |= (part_3 << 10);
	key |= part_4;
	return key;
}

static uint64_t ILPSpinLockMapper::get_type(uint64_t var)
{
	return (var >> 40) & (uint64_t) 0xf; //4 bits
}

static uint64_t ILPSpinLockMapper::get_part_1(uint64_t var)
{
	return (var >> 30) & (uint64_t) 0x3ff; //10 bits
}

static uint64_t ILPSpinLockMapper::get_part_2(uint64_t var)
{
	return (var >> 20) & (uint64_t) 0x3ff; //10 bits
}

static uint64_t ILPSpinLockMapper::get_part_3(uint64_t var)
{
	return (var >> 30) & (uint64_t) 0x3ff; //10 bits
}

static uint64_t ILPSpinLockMapper::get_part_4(uint64_t var)
{
	return var & (uint64_t) 0x3ff; //10 bits
}

ILPSpinLockMapper::ILPSpinLockMapper(uint start_var): VarMapperBase(start_var) {}

uint ILPSpinLockMapper::lookup(uint type, uint part_1, uint part_2, uint part_3, uint part_4)
{
	uint64_t key = encode_request(type, part_1, part_2, part_3, part_4);
	uint var = var_for_key(key);
	return var;
}


string ILPSpinLockMapper::key2str(uint64_t key, uint var) const
{
	ostringstream buf;

	switch (get_type(key))
	{
		case ILPSpinLockMapper::LOCALITY_ASSIGNMENT:
			buf << "A[";
			break;
		case ILPSpinLockMapper::PRIORITY_ASSIGNMENT:
			buf << "Pi[";
			break;
		case ILPSpinLockMapper::SAME_LOCALITY:
			buf << "V[";
			break;
		case ILPSpinLockMapper::HIGHER_PRIORITY:
			buf << "X[";
			break;
		case ILPSpinLockMapper::MAX_PREEMEPT:
			buf << "H[";
			break;
		case ILPSpinLockMapper::INTERFERENCE_TIME:
			buf << "I[";
			break;
		case ILPSpinLockMapper::SPIN_TIME:
			buf << "S[";
			break;
		case ILPSpinLockMapper::BLOCKING_TIME:
			buf << "B[";
			break;
		case ILPSpinLockMapper::AB_DEISION:
			buf << "Z[";
			break;
		case ILPSpinLockMapper::RESPONSE_TIME:
			buf << "R[";
			break;
		default:
			buf << "?[";
	}

	buf << get_part_1(key) << ", "
		<< get_part_2(key) << ", "
		<< get_part_3(key) << ", "
		<< get_part_4(key) << "]";

	return buf.str();
}

////////////////////SchedulabilityTest////////////////////
bool is_ILP_SpinLock_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	if(0 == tasks.get_tasks().size())
		return true;

	ILPSpinLockMapper vars;
	LinearProgram response_bound;

	ILP_SpinLock_set_objective(tasks, processors, resources, response_bound, vars);

	ILP_SpinLock_add_constraints(tasks, processors, resources, response_bound, vars);

	GLPKSolution *rb_solution = new GLPKSolution(response_bound, vars.get_num_vars(), 0.0, 1.0, 1);

	assert(rb_solution != NULL);

#if GLPK_MEM_USAGE_CHECK == 1
	int peak;
	glp_mem_usage(NULL, &peak, NULL, NULL);
	cout<<"Peak memory usage:"<<peak<<endl; 
#endif

	if(rb_solution->is_solved())
	{	
		delete rb_solution;
		return true;
	}

	delete rb_solution;
	return false;
}

void ILP_SpinLock_set_objective(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	//any objective function is okay.
	//assert(0 < tasks.get_tasks().size());

	LinearExpression *obj = new LinearExpression();
	uint var_id;

	var_id = vars.lookup(0, 0, 0, ILPSpinLockMapper::RESPONSE_TIME);
	obj->add_term(var_id, 1);

	lp.set_objective(obj);
}

void ILP_SpinLock_add_constraints(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
//cout<<"Set C1"<<endl;
	ILP_SpinLock_constraint_1(tasks, processors, resources, lp, vars);
//cout<<"Set C2"<<endl;
	ILP_SpinLock_constraint_2(tasks, processors, resources, lp, vars);
//cout<<"Set C3"<<endl;
	ILP_SpinLock_constraint_3(tasks, processors, resources, lp, vars);
//cout<<"Set C4"<<endl;
	ILP_SpinLock_constraint_4(tasks, processors, resources, lp, vars);
//cout<<"Set C5"<<endl;
	ILP_SpinLock_constraint_5(tasks, processors, resources, lp, vars);
//cout<<"Set C6"<<endl;
	ILP_SpinLock_constraint_6(tasks, processors, resources, lp, vars);
//cout<<"Set C7"<<endl;
	ILP_SpinLock_constraint_7(tasks, processors, resources, lp, vars);
//cout<<"Set C8"<<endl;
	ILP_SpinLock_constraint_8(tasks, processors, resources, lp, vars);
//cout<<"Set C9"<<endl;
	ILP_SpinLock_constraint_9(tasks, processors, resources, lp, vars);
//cout<<"Set C10"<<endl;
	ILP_SpinLock_constraint_10(tasks, processors, resources, lp, vars);
//cout<<"Set C11"<<endl;
	ILP_SpinLock_constraint_11(tasks, processors, resources, lp, vars);
//cout<<"Set C12"<<endl;
	ILP_SpinLock_constraint_12(tasks, processors, resources, lp, vars);
//cout<<"Set C13"<<endl;
	ILP_SpinLock_constraint_13(tasks, processors, resources, lp, vars);
//cout<<"Set C14"<<endl;
	ILP_SpinLock_constraint_14(tasks, processors, resources, lp, vars);
//cout<<"Set C15"<<endl;
	ILP_SpinLock_constraint_15(tasks, processors, resources, lp, vars);
//cout<<"Set C16"<<endl;
	ILP_SpinLock_constraint_16(tasks, processors, resources, lp, vars);
//cout<<"Set C17"<<endl;
	ILP_SpinLock_constraint_17(tasks, processors, resources, lp, vars);
//cout<<"Set C18"<<endl;
	ILP_SpinLock_constraint_18(tasks, processors, resources, lp, vars);
//cout<<"Set C19"<<endl;
	ILP_SpinLock_constraint_19(tasks, processors, resources, lp, vars);
//cout<<"Set all constraints."<<endl;
}

////////////////////Expressions////////////////////
void ILP_SpinLock_constraint_1(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processors.get_processor_num();
	LinearExpression *exp = new LinearExpression();

	foreach(tasks.get_tasks(), task)
	{
		uint  i = task->get_id();
		for(uint k = 1; k <= p_num; k++)
		{
			uint var_id;
			var_id = vars.lookup(ILPSpinLockMapper::LOCALITY_ASSIGNMENT, i, k);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);
		}
	}

	lp.add_inequality(exp, 1);
}

void ILP_SpinLock_constraint_2(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint t_num = tasks.get_tasks().size();
	LinearExpression *exp = new LinearExpression();

	foreach(tasks.get_tasks(), task)
	{
		uint  i = task->get_id();
		for(uint p = 1; p <= t_num; p++)
		{
			uint var_id;
			var_id = vars.lookup(ILPSpinLockMapper::PRIORITY_ASSIGNMENT, i, p);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);
		}
	}

	lp.add_inequality(exp, 1);
}

void ILP_SpinLock_constraint_3(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processors.get_processor_num();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		foreach_task_except(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_id();
			for(uint k = 1; k <= p_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPSpinLockMapper::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(ILPSpinLockMapper::LOCALITY_ASSIGNMENT, x, k);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(ILPSpinLockMapper::SAME_LOCALITY, i, x);
				exp->sub_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				lp.add_inequality(exp, 1);
			}
		}	
	}
}

void ILP_SpinLock_constraint_4(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint t_num = tasks.get_taskset_size();
	
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		foreach(tasks.get_tasks(), tx)
		{
			uint x = tx->get_id();
			for(uint p = 1; p < t_num - 1; p++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;				

				var_id = vars.lookup(ILPSpinLockMapper::HIGHER_PRIORITY, i, x);
				exp->add_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				var_id = vars.lookup(ILPSpinLockMapper::PRIORITY_ASSIGNMENT, i, p);
				exp->add_term(var_id, 1);

				for(uint j = p + 1; j <= t_num; j++)
				{
					var_id = vars.lookup(ILPSpinLockMapper::PRIORITY_ASSIGNMENT, x, j);
					exp->sub_term(var_id, 1);
				}

				lp.add_inequality(exp, 1);
			}
		}
	}
}

void ILP_SpinLock_constraint_5(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		foreach_lower_priority_task(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_id();
		
			LinearExpression *exp = new LinearExpression();
			uint var_id;
			
			var_id = vars.lookup(ILPSpinLockMapper::HIGHER_PRIORITY, i, x);
			exp->add_term(var_id, 1);

			var_id = vars.lookup(ILPSpinLockMapper::HIGHER_PRIORITY, x, i);
			exp->add_term(var_id, 1);

			lp.add_inequality(exp, 1);
		}
	}
}

void ILP_SpinLock_constraint_6(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(), task)
	{
		uint i = task->get_id();
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		ulong upper_bound = task->get_deadline();

		var_id = vars.lookup(ILPSpinLockMapper::RESPONSE_TIME, i);
		exp->add_term(var_id, 1);
		lp.declare_variable_integer(var_id);

		lp.add_inequality(exp, upper_bound);		
	}
}

void ILP_SpinLock_constraint_7(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(), task)
	{
		uint i = task->get_id();
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPSpinLockMapper::RESPONSE_TIME, i);
		exp->add_term(var_id, 1);

		var_id = vars.lookup(ILPSpinLockMapper::BLOCKING_TIME, i);
		exp->sub_term(var_id, 1);

		var_id = vars.lookup(ILPSpinLockMapper::SPIN_TIME, i);
		exp->sub_term(var_id, 1);

		var_id = vars.lookup(ILPSpinLockMapper::INTERFERENCE_TIME, i);
		exp->sub_term(var_id, 1);

		lp.add_equality(exp, task->get_wcet());
	}
}

void ILP_SpinLock_constraint_8(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();

		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPSpinLockMapper::INTERFERENCE_TIME, i);
		exp->add_term(var_id, 1);
		
		foreach_task_except(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_id();
			
			var_id = vars.lookup(ILPSpinLockMapper::MAX_PREEMEPT, i, x);
			exp->sub_term(var_id, tx->get_wcet());
			lp.declare_variable_integer(var_id);
		}

		lp.add_equality(exp, 0);
	}
}

void ILP_SpinLock_constraint_9(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		
		foreach_task_except(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_id();
			LinearExpression *exp_1 = new LinearExpression();
			LinearExpression *exp_2 = new LinearExpression();
			uint var_id;

			uint upper_bound = ceiling(ti->get_deadline(), tx->get_period());

			var_id = vars.lookup(ILPSpinLockMapper::MAX_PREEMEPT, i, x);
			exp_1->sub_term(var_id, tx->get_wcet());
			exp_2->add_term(var_id, tx->get_wcet());

			lp.add_inequality(exp_1, 0);
			lp.add_inequality(exp_2, upper_bound);
		}
	}
}

void ILP_SpinLock_constraint_10(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();

		foreach_task_except(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_id();
			LinearExpression *exp = new LinearExpression();
			uint var_id;
			
			uint c1 = ceiling(ti->get_deadline(), tx->get_period());
			double c2 = (ti->get_response_time())/(tx->get_period());

			var_id = vars.lookup(ILPSpinLockMapper::SAME_LOCALITY, i, x);
			exp->add_term(var_id, c1);

			var_id = vars.lookup(ILPSpinLockMapper::HIGHER_PRIORITY, i, x);
			exp->sub_term(var_id, c1);

			var_id = vars.lookup(ILPSpinLockMapper::MAX_PREEMEPT, i, x);
			exp->sub_term(var_id, 1);

			lp.add_inequality(exp, c2 - c1);
		}
	}
}

void ILP_SpinLock_constraint_11(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		LinearExpression *exp = new LinearExpression();
		uint var_id;
		var_id = vars.lookup(ILPSpinLockMapper::SPIN_TIME, i);
		exp->add_term(var_id, 1);
		lp.declare_variable_integer(var_id);

		for(uint k = 1; k <= p_num; k++)
		{

			var_id = vars.lookup(ILPSpinLockMapper::SPIN_TIME, i, k);
			exp->sub_term(var_id, 1);
			lp.declare_variable_integer(var_id);
		}
		lp.add_equality(exp, 0);
	}
}

void ILP_SpinLock_constraint_12(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		for(uint k = 1; k <= p_num; k++)
		{
			LinearExpression *exp = new LinearExpression();
			uint var_id;
			
			var_id = vars.lookup(ILPSpinLockMapper::SPIN_TIME, i, k);
			exp->sub_term(var_id, 1);

			for(uint q = 1; q <= r_num; q++)
			{
				var_id = vars.lookup(ILPSpinLockMapper::SPIN_TIME, i, k, q);
				exp->add_term(var_id, 1);
				lp.declare_variable_integer(var_id);
			}

			lp.add_equality(exp, 0);
		}
	}
}

void ILP_SpinLock_constraint_13(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint t_num = tasks.get_taskset_size();
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	
	ulong max_L = 0;
	uint max_N = 0;

	foreach(tasks.get_tasks(), tx)
	{
		foreach(tx->get_requests(), request)
		{
			if(request->get_num_requests() > max_N)
				max_N = request->get_num_requests();
			if(request->get_max_length() > max_L)
				max_L = request->get_max_length();
		}
	}
	ulong M = max_L * t_num * max_N;

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		foreach_task_except(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_id();
			
			for(uint q = 1; q <= r_num; q++)
			{
				ulong L_x_q;
				uint N_x_q;
				uint N_i_q;
				if(tx->is_request_exist(q - 1))
				{
					Request& request_x = tx->get_request_by_id(q - 1);
					L_x_q = request_x.get_max_length();
					N_x_q = request_x.get_num_requests();
				}
				else
				{
					L_x_q = 0;
					N_x_q = 0;
				}

				if(ti->is_request_exist(q - 1))
				{	
					Request& request_i = ti->get_request_by_id(q - 1);		
					N_i_q = request_i.get_num_requests();
				}
				else
				{
					N_i_q = 0;
				}

				for(uint k = 1; k <= p_num; k++)
				{
					LinearExpression *exp = new LinearExpression();
					uint var_id;

					var_id = vars.lookup(ILPSpinLockMapper::LOCALITY_ASSIGNMENT, x, k);
					exp->add_term(var_id, M);

					foreach_higher_priority_task(tasks.get_tasks(), (*ti), th)
					{
						uint h = th->get_id();
						uint N_h_q;
						if(th->is_request_exist(q - 1))
						{	
							Request& request_h = th->get_request_by_id(q - 1);		
							N_h_q = request_h.get_num_requests();
						}
						else
						{
							N_h_q = 0;
						}
						
						var_id = vars.lookup(ILPSpinLockMapper::MAX_PREEMEPT, i, h);
						exp->add_term(var_id, L_x_q*N_h_q);
					}

					var_id = vars.lookup(ILPSpinLockMapper::SPIN_TIME, i, k, q);
					exp->sub_term(var_id, 1);

					lp.add_inequality(exp, M - L_x_q*N_i_q);
				}
			}
		}
	}
}

void ILP_SpinLock_constraint_14(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint r_num = resources.get_resourceset_size();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		uint var_id;
		var_id = vars.lookup(ILPSpinLockMapper::BLOCKING_TIME, i);
		lp.declare_variable_integer(var_id);

		for(uint q = 1; q <= r_num; q++)
		{
			LinearExpression *exp = new LinearExpression();

			var_id = vars.lookup(ILPSpinLockMapper::BLOCKING_TIME, i);
			exp->sub_term(var_id, 1);

			var_id = vars.lookup(ILPSpinLockMapper::BLOCKING_TIME, i, q);
			exp->add_term(var_id, 1);
			lp.declare_variable_integer(var_id);

			lp.add_inequality(exp, 0);
		}
	}
}

void ILP_SpinLock_constraint_15(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();

		for(uint q = 1; q <= r_num; q++)
		{
			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPSpinLockMapper::BLOCKING_TIME, i, q);
			exp->sub_term(var_id, 1);

			for(uint k = 1; k <= r_num; k++)
			{
				var_id = vars.lookup(ILPSpinLockMapper::BLOCKING_TIME, i, q, k);
				exp->add_term(var_id, 1);
			}

			lp.add_equality(exp, 0);
		}
	}
}

void ILP_SpinLock_constraint_16(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint r_num = resources.get_resourceset_size();
	
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		for(uint q = 1; q <= r_num; q++)
		{
			if(ti->is_request_exist(q - 1))
			{
				
				uint var_id;
				
				var_id = vars.lookup(ILPSpinLockMapper::AB_DEISION, i, q);
				lp.declare_variable_binary(var_id);

				foreach_lower_priority_task(tasks.get_tasks(), (*ti), tx)
				{
					uint x = tx->get_id();
					if(tx->is_request_exist(q - 1))
					{
						foreach_higher_priority_task(tasks.get_tasks(), (*ti), th)
						{
							uint h = th->get_id();

							if(th->is_request_exist(q - 1))
							{
								LinearExpression *exp = new LinearExpression();
								
								var_id = vars.lookup(ILPSpinLockMapper::AB_DEISION, i, q);
								exp->sub_term(var_id, 1);
								lp.declare_variable_binary(var_id);

								var_id = vars.lookup(ILPSpinLockMapper::SAME_LOCALITY, x, i);
								exp->add_term(var_id, 1);

								var_id = vars.lookup(ILPSpinLockMapper::SAME_LOCALITY, i, h);
								exp->add_term(var_id, 1);

								var_id = vars.lookup(ILPSpinLockMapper::HIGHER_PRIORITY, i, x);
								exp->add_term(var_id, 1);

								var_id = vars.lookup(ILPSpinLockMapper::HIGHER_PRIORITY, i, h);
								exp->sub_term(var_id, 1);

								lp.add_inequality(exp, 2);
							}
						}
					}
				}
			}
		}
	}
}

void ILP_SpinLock_constraint_17(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint r_num = resources.get_resourceset_size();
	
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		for(uint q = 1; q <= r_num; q++)
		{
			if(ti->is_request_exist(q - 1))
			{
				
				uint var_id;
				
				var_id = vars.lookup(ILPSpinLockMapper::AB_DEISION, i, q);
				lp.declare_variable_binary(var_id);

				foreach_lower_priority_task(tasks.get_tasks(), (*ti), tx)
				{
					uint x = tx->get_id();
					if(tx->is_request_exist(q - 1))
					{
						foreach_higher_priority_task(tasks.get_tasks(), (*ti), th)
						{
							uint h = th->get_id();

							if(th->is_request_exist(q - 1))
							{
								LinearExpression *exp = new LinearExpression();
								
								var_id = vars.lookup(ILPSpinLockMapper::AB_DEISION, i, q);
								exp->sub_term(var_id, 1);

								var_id = vars.lookup(ILPSpinLockMapper::SAME_LOCALITY, x, i);
								exp->add_term(var_id, 1);

								var_id = vars.lookup(ILPSpinLockMapper::SAME_LOCALITY, h, i);
								exp->sub_term(var_id, 1);

								var_id = vars.lookup(ILPSpinLockMapper::HIGHER_PRIORITY, i, x);
								exp->add_term(var_id, 1);

								lp.add_inequality(exp, 1);
							}
						}
					}
				}
			}
		}
	}
}

void ILP_SpinLock_constraint_18(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		for(uint q = 1; q <= r_num; q++)
		{
			if(ti->is_request_exist(q - 1))
			{
				for(uint k = 1; k <= p_num; k++)
				{
					foreach(tasks.get_tasks(), tx)
					{
						LinearExpression *exp = new LinearExpression();
						uint var_id;

						uint x = tx->get_id();
						ulong L_x_q = 0;
						if(tx->is_request_exist(q - 1))
						{
							Request& request = tx->get_request_by_id(q - 1);
							L_x_q = request.get_max_length();
						}

						var_id = vars.lookup(ILPSpinLockMapper::LOCALITY_ASSIGNMENT, x, k);
						exp->sub_term(var_id, L_x_q);

						var_id = vars.lookup(ILPSpinLockMapper::LOCALITY_ASSIGNMENT, i, k);
						exp->add_term(var_id, L_x_q);

						var_id = vars.lookup(ILPSpinLockMapper::AB_DEISION, i, q);
						exp->add_term(var_id, L_x_q);

						var_id = vars.lookup(ILPSpinLockMapper::HIGHER_PRIORITY, x, i);
						exp->sub_term(var_id, L_x_q);

						var_id = vars.lookup(ILPSpinLockMapper::BLOCKING_TIME, i, q, k);
						exp->sub_term(var_id, 1);

						lp.add_inequality(exp, L_x_q);
					}
				}
			}
		}
	}
}

void ILP_SpinLock_constraint_19(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		for(uint q = 1; q <= r_num; q++)
		{
			if(ti->is_request_exist(q - 1))
			{
				for(uint k = 1; k <= p_num; k++)
				{
					foreach(tasks.get_tasks(), tx)
					{
						LinearExpression *exp = new LinearExpression();
						uint var_id;

						uint x = tx->get_id();
						ulong L_x_q = 0;
						if(tx->is_request_exist(q - 1))
						{
							Request& request = tx->get_request_by_id(q - 1);
							L_x_q = request.get_max_length();
						}

						var_id = vars.lookup(ILPSpinLockMapper::LOCALITY_ASSIGNMENT, x, k);
						exp->sub_term(var_id, L_x_q);

						var_id = vars.lookup(ILPSpinLockMapper::LOCALITY_ASSIGNMENT, i, k);
						exp->sub_term(var_id, L_x_q);

						var_id = vars.lookup(ILPSpinLockMapper::AB_DEISION, i, q);
						exp->add_term(var_id, L_x_q);

						var_id = vars.lookup(ILPSpinLockMapper::BLOCKING_TIME, i, q, k);
						exp->sub_term(var_id, 1);

						lp.add_inequality(exp, L_x_q);
					}
				}
			}
		}
	}
}

































