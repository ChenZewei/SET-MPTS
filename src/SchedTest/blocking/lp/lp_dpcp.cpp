#include <types.h>
#include <tasks.h>
#include <resources.h>
#include <processors.h>
#include <lp.h>
#include <varmapper.h>

void lp_dpcp_objective(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars, LinearExpression *local_obj, LinearExpression *remote_obj)
{
	LinearExpression *obj = new LinearExpression();
	uint var_id;

	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach(tx->get_requests(), request)
		{
			uint q = request->get_resource_id();
			bool is_local = (request->get_locality() == ti.get_partition());
			ulong length = request->get_max_length();
			foreach_request_instance(ti, *tx, v)
			{
				var_id = vars.lookup(x, q, v, BLOCKING_DIRECT);
				obj->add_term(var_id, length);
				if (is_local && (local_obj != NULL))
					local_obj->add_term(var_id, length);
				else if (!is_local && (remote_obj != NULL))
					remote_obj->add_term(var_id, length);

				var_id = vars.lookup(x, q, v, BLOCKING_INDIRECT);
				obj->add_term(var_id, length);
				if (is_local && (local_obj != NULL))
					local_obj->add_term(var_id, length);
				else if (!is_local && (remote_obj != NULL))
					remote_obj->add_term(var_id, length);

				var_id = vars.lookup(x, q, v, BLOCKING_PREEMPT);
				obj->add_term(var_id, length);
				if (is_local && (local_obj != NULL))
					local_obj->add_term(var_id, length);
				else if (!is_local && (remote_obj != NULL))
					remote_obj->add_term(var_id, length);
			}
		}
	}
	delete obj;
	vars.seal();
}

void lp_dpcp_local_objective(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars)
{
	LinearExpression *obj = new LinearExpression();
	uint var_id;

	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach_local_request(ti, tx->get_requests(), request_iter)
		{
			uint q = request_iter->get_resource_id();
			foreach_request_instance(ti, *tx, v)
			{
				var_id = vars.lookup(x, q, v, BLOCKING_DIRECT);
				obj->add_term(var_id, request_iter->get_max_length());

				var_id = vars.lookup(x, q, v, BLOCKING_INDIRECT);
				obj->add_term(var_id, request_iter->get_max_length());

				var_id = vars.lookup(x, q, v, BLOCKING_PREEMPT);
				obj->add_term(var_id, request_iter->get_max_length());
			}
		}
	}
	
	lp.set_objective(obj);
	delete obj;
	vars.seal();
}

void lp_dpcp_remote_objective(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars)
{
	LinearExpression *obj = new LinearExpression();
	uint var_id;

	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach_remote_request(ti, tx->get_requests(), request_iter)
		{
			uint q = request_iter->get_resource_id();
			foreach_request_instance(ti, *tx, v)
			{
				var_id = vars.lookup(x, q, v, BLOCKING_DIRECT);
				obj->add_var(var_id);
				obj->add_term(var_id, request_iter->get_max_length());

				var_id = vars.lookup(x, q, v, BLOCKING_INDIRECT);
				obj->add_var(var_id);
				obj->add_term(var_id, request_iter->get_max_length());

				var_id = vars.lookup(x, q, v, BLOCKING_PREEMPT);
				obj->add_var(var_id);
				obj->add_term(var_id, request_iter->get_max_length());
			}
		}
	}

	lp.set_objective(obj);
	delete obj;
	vars.seal();
}

//Constraint 1 [BrandenBurg 2013 RTAS] Xd(x,q,v) + Xi(x,q,v) + Xp(x,q,v) <= 1
void lp_dpcp_constraint_1(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars)
{
//cout<<"Constraint 1"<<endl;
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

				var_id = vars.lookup(x, q, v, BLOCKING_DIRECT);
				exp->add_var(var_id);
//cout<<"var_id1:"<<var_id<<endl;			
				var_id = vars.lookup(x, q, v, BLOCKING_INDIRECT);
				exp->add_var(var_id);
//cout<<"var_id2:"<<var_id<<endl;
				var_id = vars.lookup(x, q, v, BLOCKING_PREEMPT);
				exp->add_var(var_id);
//cout<<"var_id3:"<<var_id<<endl;
				lp.add_inequality(exp, 1);// Xd(x,q,v) + Xi(x,q,v) + Xp(x,q,v) <= 1
//cout<<"----------inequality----------"<<endl;
/*
				foreach(exp->get_terms(), term)
				{
					cout<<"term:"<<term->first<<endl;
					cout<<"var:"<<term->second<<endl;
				}
*/
//cout<<"==========inequality=========="<<endl;
			}
		}
	}

	
	
}

//Constraint 2 [BrandenBurg 2013 RTAS] for any remote resource lq and task tx except ti Xp(x,q,v) = 0
void lp_dpcp_constraint_2(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars)
{
	LinearExpression *exp = new LinearExpression();

	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach_remote_request(ti, tx->get_requests(), request_iter)
		{
			uint q = request_iter->get_resource_id();
			foreach_request_instance(ti, *tx, v)
			{
				uint var_id;
				var_id = vars.lookup(x, q, v, BLOCKING_PREEMPT);
				exp->add_var(var_id);
			}
		}
	}
	lp.add_equality(exp, 0);
}

//Constraint 3 [BrandenBurg 2013 RTAS]
void lp_dpcp_constraint_3(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars)
{
	
}
