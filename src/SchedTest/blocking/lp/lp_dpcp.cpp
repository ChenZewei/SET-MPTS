#include "lp_dpcp.h"
#include <types.h>
#include <tasks.h>
#include <resources.h>
#include <processors.h>
#include <lp.h>
#include <varmapper.h>

void lp_dpcp_objective(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars, LinearExpression *local_obj, LinearExpression *remote_obj)
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
			foreach_request_instance(ti, *tx, v)
			{
				uint var_id;

				var_id = vars.lookup(x, q, v, BLOCKING_DIRECT);
				//obj->add_term(var_id, length);
				if (is_local && (local_obj != NULL))
					local_obj->add_term(var_id, length);
				else if (!is_local && (remote_obj != NULL))
					remote_obj->add_term(var_id, length);

				var_id = vars.lookup(x, q, v, BLOCKING_INDIRECT);
				//obj->add_term(var_id, length);
				if (is_local && (local_obj != NULL))
					local_obj->add_term(var_id, length);
				else if (!is_local && (remote_obj != NULL))
					remote_obj->add_term(var_id, length);

				var_id = vars.lookup(x, q, v, BLOCKING_PREEMPT);
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

void lp_dpcp_local_objective(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars)
{
	LinearExpression *obj = new LinearExpression();
	
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach_local_request(ti, tx->get_requests(), request_iter)
		{
			uint q = request_iter->get_resource_id();
			foreach_request_instance(ti, *tx, v)
			{
				uint var_id;

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
	
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach_remote_request(ti, tx->get_requests(), request_iter)
		{
			uint q = request_iter->get_resource_id();
			foreach_request_instance(ti, *tx, v)
			{
				uint var_id;

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

void lp_dpcp_add_constraints(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, VarMapper& vars)
{
	lp_dpcp_constraint_1(ti, tasks, resources, lp, vars);
	lp_dpcp_constraint_2(ti, tasks, resources, lp, vars);
	lp_dpcp_constraint_3(ti, tasks, resources, lp, vars);
	lp_dpcp_constraint_4(ti, tasks, resources, lp, vars);
	lp_dpcp_constraint_5(ti, tasks, processors, resources, lp, vars);
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
			foreach_request_instance(ti, *tx, v)
			{
				uint var_id;
				var_id = vars.lookup(x, q, v, BLOCKING_PREEMPT);
				exp->add_var(var_id);
			}
		}
		lp.add_inequality(exp, max_arrival);
	}
}

//Constraint 6 [BrandenBurg 2013 RTAS]
void lp_dpcp_constraint_4(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars)
{
	LinearExpression *exp = new LinearExpression();
	uint priority = ti.get_id();
	
	foreach_task_except(tasks.get_tasks(), ti, tx)
	{
		uint x = tx->get_id();
		foreach(tx->get_requests(), request)
		{
			uint q = request->get_resource_id();
			Resource& resource = resources.get_resources()[q];
			if(resource.get_ceiling() > priority)
			{
				foreach_request_instance(ti, *tx, v)
				{
					uint var_id;

					var_id = vars.lookup(x, q, v, BLOCKING_DIRECT);
					exp->add_var(var_id);

					var_id = vars.lookup(x, q, v, BLOCKING_INDIRECT);
					exp->add_var(var_id);
				}
			}
		}
	}
	lp.add_equality(exp, 0);
}

//Constraint 7 [BrandenBurg 2013 RTAS]
void lp_dpcp_constraint_5(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, VarMapper& vars)
{	
	uint priority = ti.get_id();

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
					foreach_request_instance(ti, *tx, v)
					{
						uint var_id;

						var_id = vars.lookup(x, q, v, BLOCKING_DIRECT);
						exp->add_var(var_id);

						var_id = vars.lookup(x, q, v, BLOCKING_INDIRECT);
						exp->add_var(var_id);
					}
				}
			}
		}

		lp.add_inequality(exp, cluster_request);
	}
}

//Constraint 8 [BrandenBurg 2013 RTAS]
void lp_dpcp_constraint_6(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, VarMapper& vars)
{
	
}


























