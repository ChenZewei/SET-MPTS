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
static uint64_t ILPSpinLockMapper::encode_request(uint64_t type, uint64_t task_i, uint64_t task_x, uint64_t processor, uint64_t priority);

static uint64_t ILPSpinLockMapper::get_type(uint64_t var);

static uint64_t ILPSpinLockMapper::get_task_i(uint64_t var);

static uint64_t ILPSpinLockMapper::get_task_x(uint64_t var);

static uint64_t ILPSpinLockMapper::get_processor(uint64_t var);

static uint64_t ILPSpinLockMapper::get_priority(uint64_t var);

ILPSpinLockMapper::ILPSpinLockMapper(uint start_var = 0);

uint ILPSpinLockMapper::lookup(uint type, uint task_i, uint task_x, uint processor, uint priority);

string ILPSpinLockMapper::key2str(uint64_t key, uint var) const;

////////////////////SchedulabilityTest////////////////////

////////////////////Expressions////////////////////
void ILP_SpinLock_constraint_1(TaskSet& tasks, ProcessorSet& processors, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processors.get_processor_num();
	LinearExpression *exp = new LinearExpression();

	foreach(tasks.get_tasks(), task)
	{
		uint  i = task->get_id();
		for(uint k = 1; k <= p_num; k++)
		{
			uint var_id;
			var_id = vars.lookup(LOCALITY_ASSIGNMENT, i, k);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);
		}
	}

	lp.add_inequality(exp, 1);
}

void ILP_SpinLock_constraint_2(TaskSet& tasks, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint t_num = tasks.get_tasks().size();
	LinearExpression *exp = new LinearExpression();

	foreach(tasksget_tasks(), task)
	{
		uint  i = task->get_id();
		for(uint p = 1; p <= t_num; p++)
		{
			uint var_id;
			var_id = vars.lookup(PRIORITY_ASSIGNMENT, i, p);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);
		}
	}

	lp.add_inequality(exp, 1);
}

void ILP_SpinLock_constraint_3(TaskSet& tasks, ProcessorSet& processors, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processors.get_processor_num();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		foreach_task_except(task.get_tasks(), *ti, tx)
		{
			uint x = tx->get_id();
			for(uint k = 1, k <= p_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(LOCALITY_ASSIGNMENT, x, k);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(SAME_LOCALITY, i, x);
				exp->sub_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				lp.add_inequality(exp, 1);
			}
		}	
	}
}

void ILP_SpinLock_constraint_4(TaskSet& tasks, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint t_num = tasks.get_taskset_size();
	
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		foreach(tasks.get_tasks.get_tasks(), tx)
		{
			uint x = tx->get_id();
			for(uint p = 1; p < t_num - 1; p++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;				

				var_id = vars.lookup(HIGHER_PRIORITY, i, x);
				exp->add_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				var_id = vars.lookup(PRIORITY_ASSIGNMENT, i, p);
				exp->add_term(var_id, 1);

				for(uint j = p + 1; j <= t_num; j++)
				{
					var_id = vars.lookup(PRIORITY_ASSIGNMENT, x, j);
					exp->sub_term(var_id, 1);
				}

				lp.add_inequality(exp, 1);
			}
		}
	}
}

void ILP_SpinLock_constraint_5(TaskSet& tasks, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		foreach_lower_priority_task(tasks.get_tasks(), *ti, tx)
		{
			uint x = tx->get_id();
		
			LinearExpression *exp = new LinearExpression();
			uint var_id;
			
			var_id = vars.lookup(HIGHER_PRIORITY, i, x);
			exp->add_term(var_id, 1);

			var_id = vars.lookup(HIGHER_PRIORITY, x, i);
			exp->add_term(var_id, 1);

			lp.add_inequrality(exp, 1);
		}
	}
}

void ILP_SpinLock_constraint_6(TaskSet& tasks, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(), task)
	{
		uint i = task->get_id();
		uint var_id;

		ulong upper_bound = task->get_deadline();

		var_id = vars.lookup(RESPONSE_TIME, i);
		declare_variable_bounds(var_id, false, 0, true, upper_bound);
	}
}

void ILP_SpinLock_constraint_7(TaskSet& tasks, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(), task)
	{
		uint i = task->get_id();
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(RESPONSE_TIME, i);
		exp->add_term(var_id, 1);

		var_id = vars.lookup(BLOCKING_TIME, i);
		exp->sub_term(var_id, 1);

		var_id = vars.lookup(SPIN_TIME, i);
		exp->sub_term(var_id, 1);

		var_id = vars.lookup(INTERFERENCE, i);
		exp->sub_term(var_id, 1);

		lp.add_equality(exp, task->get_wcet());
	}
}

void ILP_SpinLock_constraint_8(TaskSet& tasks, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(). ti)
	{
		uint i = ti->get_id();

		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(INTERFERENCE, i);
		exp->add_term(var_id, 1);
		
		foreach_task_except(tasks.get_tasks(), *ti, tx)
		{
			uint x = tx->get_id();
			
			var_id = vars.lookup(MAX_PREEMEPT, i, x);
			exp->sub_term(var_id, tx->get_wcet());
			lp.declare_variable_integer(var_id);
		}

		lp.add_equality(exp, 0);
	}
}

void ILP_SpinLock_constraint_9(TaskSet& tasks, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		
		foreach_task_except(tasks.get_tasks(), *ti, tx)
		{
			uint x = tx->get_id();
			uint var_id;

			uint upper_bound = ceiling(ti->get_deadline(), tx->get_period());

			var_id = vars.lookup(MAX_PREEMEPT, i, x);
			declare_variable_bounds(var_id, true, 0, true, upper_bound);
		}
	}
}

void ILP_SpinLock_constraint_10(TaskSet& tasks, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();

		foreach_task_except(tasks.get_tasks(), *ti, tx)
		{
			uint x = tx->get_id();
			LinearExpression *exp = new LinearExpression();
			uint var_id;
			
			uint c1 = ceiling(ti->get_deadline(), tx->get_period());
			double c2 = (ti->get_response_time())/(tx->get_period());

			var_id = vars.lookup(SAME_LOCALITY, i, x);
			exp->add_term(var_id, c1);

			var_id = vars.lookup(HIGHER_PRIORITY, i, x);
			exp->sub_term(var_id, c1);

			var_id = vars.lookup(MAX_PREEMEPT, i, x);
			exp->sub_term(var_id, 1);

			lp.add_inequality(exp, c2 - c1);
		}
	}
}

void ILP_SpinLock_constraint_11(TaskSet& tasks, ProcessorSet& processors, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		LinearExpression *exp = new LinearExpression();
		uint var_id;
		var_id = vars.lookup(SPIN_TIME, i);
		exp->add_term(var_id, 1);
		lp.declare_variable_integer(var_id);

		uint i = ti->get_id();
		for(uint k = 1; k <= p_num; k++)
		{

			var_id = vars.lookup(SPIN_TIME, i, k);
			exp->sub_term(var_id, 1);
			lp.declare_variable_integer(var_id);
		}
		lp.add_equality(exp, 0);
	}
}

void ILP_SpinLock_constraint_12(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processor.get_processor_num();
	uint r_num = resources.get_resourceset_size();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		for(uint k = 1; k <= p_num; k++)
		{
			LinearExpression *exp = new LinearExpression();
			uint var_id;
			
			var_id = vars.lookup(SPIN_TIME, i, k);
			exp->add_term(var_id, 1);

			for(uint q = 1; q <= r_num; q++)
			{
				var_id = vars.lookup(SPIN_TIME, i, k, q);
				exp->add_term(var_id, 1);
				lp.declare_variable_integer(var_id);
			}
			lp.add_equality(exp, 0);
		}
	}
}

void ILP_SpinLock_constraint_13(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint t_num = tasks.get_taskset_size();
	uint p_num = processor.get_processor_num();
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
		foreach_task_except(tasks.get_tasks(), *ti, tx)
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

					var_id = vars.lookup(LOCALITY_ASSIGNMENT, x, k);
					exp->add_term(var_id, M);

					foreach_higher_priority_task(tasks.get_tasks(), *ti, th)
					{
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
						
						var_id = vars.lookup(MAX_PREEMEPT, i, h);
						exp->add_term(var_id, L_x_q*N_h_q);
					}

					var_id = vars.lookup(SPIN_TIME, i, k, q);
					exp->sub_term(var_id, 1);

					lp.add_inequality(exp, M - L_x_q*N_i_q);
				}
			}
		}
	}
}

void ILP_SpinLock_constraint_14(TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint r_num = resources.get_resourceset_size();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		uint var_id;
		var_id = vars.lookup(BLOCKING_TIME, i);
		lp.declare_variable_integer(var_id);

		for(uint q = 1; q <= r_num; q++)
		{
			LinearExpression *exp = new LinearExpression();

			var_id = vars.lookup(BLOCKING_TIME, i);
			exp->sub_term(var_id, 1);

			var_id = vars.lookup(BLOCKING_TIME, i, q);
			exp->add_term(var_id, 1);
			lp.declare_variable_integer(var_id);

			lp.add_inequality(exp, 0);
		}
	}
}

void ILP_SpinLock_constraint_15(TaskSet& tasks, ProcessorSet& processors, LinearProgram& lp, ILPSpinLockMapper& vars)
{
	uint p_num = processor.get_processor_num();
	uint r_num = resources.get_resourceset_size();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();

		for(uint q = 1; q <= r_num; q++)
		{
			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(BLOCKING_TIME, i, q);
			exp->sub_term(var_id, 1);

			for(uint k = 1; k <= r_num; k++)
			{
				var_id = vars.lookup(BLOCKING_TIME, i, q, k);
				exp->add_term(var_id, 1);
			}

			lp.add_equality(exp, 0);
		}
	}
}








