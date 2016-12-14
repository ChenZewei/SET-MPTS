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

//Constraint 1 [BrandenBurg 2013 RTAS] Xd(x,q,v) + Xi(x,q,v) + Xp(x,q,v) <= 1
void lp_dpcp_constraint_1(const Task& ti, const TaskSet& tasks, const ResourceSet& resources, LinearProgram& lp)
{
	VarMapper var = new VarMapper();
	
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
void lp_dpcp_constraint_2(const Task& ti, const TaskSet& tasks, const ResourceSet& resources, LinearProgram& lp)
{
	LinearExpression *exp = new LinearExpression();
	VarMapper var = new VarMapper();

	foreach_task_except(tasks, ti, tx)
	{
		uint x = tx->get_id();
		foreach_remote_request(task_ti, requests, request, request_iter)
		{
			uint q = request->get_resource_id();
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
void lp_dpcp_constraint_2(const Task& ti, const TaskSet& tasks, const ResourceSet& resources, LinearProgram& lp)
{

}


void 

#endif
