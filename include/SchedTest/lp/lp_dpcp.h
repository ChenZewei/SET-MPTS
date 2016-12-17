#ifndef LP_DPCP_H
#define LP_DPCP_H

//#include <lp.h>

class Task;
class TaskSet;
class Resource;
class ResourceSet;
class Processor;
class ProcessorSet;
class VarMapper;
class LinearExpression;
class LinearProgram;
class GLPKSolution;

void lp_dpcp_local_objective(const Task& ti, const TaskSet& tasks, const ResourceSet& resources, LinearProgram& lp, VarMapper& var)
{
	LinearExpression *obj = new LinearExpression();
	uint var_id;

	foreach_task_except(tasks, ti, tx)
	{
		uint x = tx->get_id();
		foreach_local_request(task_ti, tx->get_requests(), request_iter)
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
	
	lp->set_objective(obj);
	var.seal();
}

void lp_dpcp_remote_objective(const Task& ti, const TaskSet& tasks, const ResourceSet& resources, LinearProgram& lp, VarMapper& var)
{
	LinearExpression *obj = new LinearExpression();
	uint var_id;

	foreach_task_except(tasks, ti, tx)
	{
		uint x = tx->get_id();
		foreach_remote_request(task_ti, tx->get_requests(), request_iter)
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

	lp->set_objective(obj);
	var.seal();
}

//Constraint 1 [BrandenBurg 2013 RTAS] Xd(x,q,v) + Xi(x,q,v) + Xp(x,q,v) <= 1
void lp_dpcp_constraint_1(const Task& ti, const TaskSet& tasks, const ResourceSet& resources, LinearProgram& lp, VarMapper& var)
{
	foreach_task_except(tasks, ti, tx)
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
				
				var_id = vars.lookup(x, q, v, BLOCKING_INDIRECT);
				exp->add_var(var_id);

				var_id = vars.lookup(x, q, v, BLOCKING_PREEMPT);
				exp->add_var(var_id);

				lp.add_inequality(exp, 1);// Xd(x,q,v) + Xi(x,q,v) + Xp(x,q,v) <= 1
			}
		}
	}
	
}

//Constraint 2 [BrandenBurg 2013 RTAS] for any remote resource lq and task tx except ti Xp(x,q,v) = 0
void lp_dpcp_constraint_2(const Task& ti, const TaskSet& tasks, const ResourceSet& resources, LinearProgram& lp, VarMapper& var)
{
	LinearExpression *exp = new LinearExpression();

	foreach_task_except(tasks, ti, tx)
	{
		uint x = tx->get_id();
		foreach_remote_request(task_ti, tx->get_requests(), request_iter)
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
void lp_dpcp_constraint_3(const Task& ti, const TaskSet& tasks, const ResourceSet& resources, LinearProgram& lp, VarMapper& var)
{
	
}


void 

#endif
