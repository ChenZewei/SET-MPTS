#include "ilp_rta_pfp_dpcp_3.h"
#include <lp.h>
#include <solution.h>
#include <sstream>
#include <iostream>
#include <assert.h>
#include "iteration-helper.h"
#include "math-helper.h"

////////////////////ILPDPCPMapper_3////////////////////
static uint64_t ILPDPCPMapper_3::encode_request(uint64_t type, uint64_t part_1, uint64_t part_2, uint64_t part_3, uint64_t part_4)
{
	uint64_t one = 1;
	uint64_t key = 0;
	assert(type < (one << 5));
	assert(part_1 < (one << 10));
	assert(part_2 < (one << 10));
	assert(part_3 < (one << 10));
	assert(part_4 < (one << 10));


	key |= (type << 40);
	key |= (part_1 << 30);
	key |= (part_2 << 20);
	key |= (part_3 << 10);
	key |= part_4;

/*
cout<<"type:"<<type<<endl;
cout<<"part_1:"<<part_1<<endl;
cout<<"part_2:"<<part_2<<endl;
cout<<"part_3:"<<part_3<<endl;
cout<<"part_4:"<<part_4<<endl;
cout<<"key:"<<key<<endl;
*/
	return key;
}

static uint64_t ILPDPCPMapper_3::get_type(uint64_t var)
{
	return (var >> 40) & (uint64_t) 0x1f; //5 bits
}

static uint64_t ILPDPCPMapper_3::get_part_1(uint64_t var)
{
	return (var >> 30) & (uint64_t) 0x3ff; //10 bits
}

static uint64_t ILPDPCPMapper_3::get_part_2(uint64_t var)
{
	return (var >> 20) & (uint64_t) 0x3ff; //10 bits
}

static uint64_t ILPDPCPMapper_3::get_part_3(uint64_t var)
{
	return (var >> 10) & (uint64_t) 0x3ff; //10 bits
}

static uint64_t ILPDPCPMapper_3::get_part_4(uint64_t var)
{
	return var & (uint64_t) 0x3ff; //10 bits
}

ILPDPCPMapper_3::ILPDPCPMapper_3(uint start_var): VarMapperBase(start_var) {}

uint ILPDPCPMapper_3::lookup(uint type, uint part_1, uint part_2, uint part_3, uint part_4)
{
	uint64_t key = encode_request(type, part_1, part_2, part_3, part_4);
	
//cout<<"string:"<<key2str(key)<<endl;
	uint var = var_for_key(key);
	return var;
}

string ILPDPCPMapper_3::var2str(unsigned int var) const
{
	uint64_t key;

	if (search_key_for_var(var, key))
	{
		return key2str(key);
	}
	else
		return "<?>";
}

string ILPDPCPMapper_3::key2str(uint64_t key) const
{
	ostringstream buf;

	switch (get_type(key))
	{
		case ILPDPCPMapper_3::SAME_LOCALITY:
			buf << "S[";
			break;
/*		case ILPDPCPMapper_3::LOCALITY_ASSIGNMENT:
			buf << "A[";
			break;
		case ILPDPCPMapper_3::RESOURCE_ASSIGNMENT:
			buf << "Q[";
			break;
		case ILPDPCPMapper_3::SAME_TASK_LOCALITY:
			buf << "U[";
			break;
		case ILPDPCPMapper_3::SAME_RESOURCE_LOCALITY:
			buf << "V[";
			break;
		case ILPDPCPMapper_3::SAME_TR_LOCALITY:
			buf << "W[";
			break;
//		case ILPDPCPMapper_3::APPLICATION_CORE:
//			buf << "AC[";
//			break;
*/
		case ILPDPCPMapper_3::PREEMPT_NUM:
			buf << "P[";
			break;
		case ILPDPCPMapper_3::TBT_PREEMPT_NUM:
			buf << "H[";
			break;
		case ILPDPCPMapper_3::RBT_PREEMPT_NUM:
			buf << "H[";
			break;
		case ILPDPCPMapper_3::RBR_PREEMPT_NUM:
			buf << "H[";
			break;
		case ILPDPCPMapper_3::RESPONSE_TIME:
			buf << "R[";
			break;
		case ILPDPCPMapper_3::BLOCKING_TIME:
			buf << "B[";
			break;
		case ILPDPCPMapper_3::REQUEST_BLOCKING_TIME:
			buf << "b[";
			break;
		case ILPDPCPMapper_3::INTERFERENCE_TIME_R:
			buf << "I_R[";
			break;
		case ILPDPCPMapper_3::INTERFERENCE_TIME_R_RESOURCE:
			buf << "I_R[";
			break;
		case ILPDPCPMapper_3::INTERFERENCE_TIME_C:
			buf << "I_C[";
			break;
		case ILPDPCPMapper_3::INTERFERENCE_TIME_C_TASK:
			buf << "I_C[";
			break;
		case ILPDPCPMapper_3::INTERFERENCE_TIME_C_RESOURCE:
			buf << "I_C[";
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

////////////////////ILP_RTA_PFP_DPCP_3////////////////////
ILP_RTA_PFP_DPCP_3::ILP_RTA_PFP_DPCP_3(): PartitionedSched(true, RTA, FIX_PRIORITY, DPCP, "", "DPCP") {}

ILP_RTA_PFP_DPCP_3::ILP_RTA_PFP_DPCP_3(TaskSet tasks, ProcessorSet processors, ResourceSet resources): PartitionedSched(true, RTA, FIX_PRIORITY, DPCP, "", "DPCP")
{
	this->tasks = tasks;
	this->processors = processors;
	this->resources = resources;

	this->resources.update(&(this->tasks));
	this->processors.update(&(this->tasks), &(this->resources));
	
	this->tasks.RM_Order();
	this->processors.init();
}

ILP_RTA_PFP_DPCP_3::~ILP_RTA_PFP_DPCP_3(){}

bool ILP_RTA_PFP_DPCP_3::is_schedulable()
{
	if(0 == tasks.get_tasks().size())
		return true;

	ILPDPCPMapper_3 vars;
	LinearProgram response_bound;

	set_objective(response_bound, vars);

	add_constraints(response_bound, vars);

//cout<<"var num:"<<vars.get_num_vars()<<endl;

	GLPKSolution *rb_solution = new GLPKSolution(response_bound, vars.get_num_vars(), 0.0, 1.0, 1, 1);

	assert(rb_solution != NULL);

#if GLPK_MEM_USAGE_CHECK == 1
	int peak;
	glp_mem_usage(NULL, &peak, NULL, NULL);
	cout<<"Peak memory usage:"<<peak<<endl; 
#endif

	if(rb_solution->is_solved())
	{	

#if ILP_SOLUTION_VAR_CHECK == 1


	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	uint t_num = tasks.get_taskset_size();

cout<<"|===========SAME_LOCALITY===========|"<<endl;
	for(uint i = 0; i < t_num + r_num; i++)
	{
		LinearExpression *exp = new LinearExpression();
		double result;
		for(uint j = 0; j < t_num + r_num; j++)
		{
			construct_exp(vars, exp, ILPDPCPMapper_3::SAME_LOCALITY, i, j);
			result = rb_solution->evaluate(*exp);
cout<<result<<"  ";
		}
cout<<endl;
	}

cout<<"|===========NOT_ALONE===========|"<<endl;
	for(uint i = 0; i < t_num + r_num - 1; i++)
	{
		LinearExpression *exp = new LinearExpression();
		double result;

		construct_exp(vars, exp, ILPDPCPMapper_3::NOT_ALONE, i);
		result = rb_solution->evaluate(*exp);

cout<<"Task "<<i<<":"<<result<<endl;
	}

cout<<"|===========TASK_PREEMPTION===========|"<<endl;
for(uint i = 0; i < t_num; i++)
{
	
	LinearExpression *exp = new LinearExpression();
	double result;
	for(uint j = 0; j < t_num; j++)
	{
		construct_exp(vars, exp, ILPDPCPMapper_3::TBT_PREEMPT_NUM, i, j);
		result = rb_solution->evaluate(*exp);
cout<<result<<"  ";
	}
cout<<endl;
}


		for(uint i = 0; i < tasks.get_tasks().size(); i++)
		{
			Task& task = tasks.get_task_by_index(i);
cout<<"/===========Task "<<i<<"===========/"<<endl;
			LinearExpression *exp = new LinearExpression();
			double result;

			construct_exp(vars, exp, ILPDPCPMapper_3::RESPONSE_TIME, i);
			result = rb_solution->evaluate(*exp);
cout<<"R"<<i<<":"<<result<<endl;

/*
			construct_exp(vars, exp, ILPDPCPMapper_3::INTERFERENCE_TIME_R, i);
			result = rb_solution->evaluate(*exp);
cout<<"IR_"<<i<<":"<<result<<endl;

			foreach(task.get_requests(), request)
			{
				uint u = request->get_resource_id();
//cout<<"B_"<<i<<"_"<<u<<":"<<vars.lookup(ILPDPCPMapper_3::REQUEST_BLOCKING_TIME, i, u)<<endl;
				construct_exp(vars, exp, ILPDPCPMapper_3::INTERFERENCE_TIME_R_RESOURCE, i, u);
				result = rb_solution->evaluate(*exp);
cout<<"IR_"<<i<<"_"<<u<<":"<<result<<endl;
			}
*/

			construct_exp(vars, exp, ILPDPCPMapper_3::INTERFERENCE_TIME_C, i);
			result = rb_solution->evaluate(*exp);
cout<<"IC_"<<i<<":"<<result<<endl;
/*
			foreach_higher_priority_task(tasks.get_tasks(), task, th)
			{
				uint h = th->get_index();
				construct_exp(vars, exp, ILPDPCPMapper_3::INTERFERENCE_TIME_C_TASK, i, h);
				result = rb_solution->evaluate(*exp);
cout<<"IC_"<<i<<"_"<<h<<":"<<result<<endl;

			}

			
			foreach_lower_priority_task(tasks.get_tasks(), task, tl)
			{
				uint l = tl->get_index();

				foreach(tl->get_requests(), request)
				{
					uint v = request->get_resource_id();
					
					construct_exp(vars, exp, ILPDPCPMapper_3::INTERFERENCE_TIME_C_RESOURCE, i, l, v);
					result = rb_solution->evaluate(*exp);
cout<<"IC_"<<i<<"_"<<l<<"_"<<v<<":"<<result<<endl;
				}
			}
*/
cout<<"C_"<<i<<":"<<tasks.get_tasks()[i].get_wcet()<<endl;
cout<<"D_"<<i<<":"<<tasks.get_tasks()[i].get_deadline()<<endl;
			delete exp;
		}
#endif

		//rb_solution->show_error();
		delete rb_solution;
		return true;
	}
	else
	{
		//rb_solution->show_error();
	}

	delete rb_solution;
	return false;
}

void ILP_RTA_PFP_DPCP_3::construct_exp(ILPDPCPMapper_3& vars, LinearExpression *exp, uint type, uint part_1, uint part_2, uint part_3, uint part_4)
{
	exp->get_terms().clear();
	uint var_id;

	var_id = vars.lookup(type, part_1, part_2, part_3, part_4);
	exp->add_term(var_id, 1);

#if ILP_SOLUTION_VAR_CHECK == 1
//cout<<vars.var2str(var_id)<<":"<<endl;
#endif
}

void ILP_RTA_PFP_DPCP_3::set_objective(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	//any objective function is okay.
	//assert(0 < tasks.get_tasks().size());

	LinearExpression *obj = new LinearExpression();
	uint var_id;

	uint i = tasks.get_taskset_size() - 1;
	var_id = vars.lookup(ILPDPCPMapper_3::RESPONSE_TIME, i, 0, 0);
	obj->add_term(var_id, 1);

	lp.set_objective(obj);
}

void ILP_RTA_PFP_DPCP_3::add_constraints(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	constraint_1(lp, vars);
	constraint_2(lp, vars);
	constraint_3(lp, vars);
//	constraint_3_1(lp, vars);
//	constraint_3_2(lp, vars);
	constraint_4(lp, vars);
//	constraint_4_1(lp, vars);
//	constraint_4_2(lp, vars);
	constraint_5(lp, vars);
//	constraint_5_1(lp, vars);
//	constraint_5_2(lp, vars);
	constraint_6(lp, vars);
	constraint_7(lp, vars);
/*
	constraint_8(lp, vars);
	constraint_9(lp, vars);
*/
	constraint_10(lp, vars);
	constraint_11(lp, vars);
	constraint_12(lp, vars);
/*
	constraint_13(lp, vars);
	constraint_14(lp, vars);
	constraint_15(lp, vars);
	constraint_16(lp, vars);
*/
	constraint_17(lp, vars);
	constraint_18(lp, vars);
/*
	constraint_19(lp, vars);
	constraint_20(lp, vars);
	constraint_21(lp, vars);
	constraint_22(lp, vars);
	constraint_22_1(lp, vars);
*/
	constraint_23(lp, vars);
}

////////////////////Expressions////////////////////
//C1
void ILP_RTA_PFP_DPCP_3::constraint_1(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	uint t_num = tasks.get_taskset_size();

	for(uint i = 0; i < r_num + t_num; i++)
	{
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper_3::SAME_LOCALITY, i, i);
		exp->add_term(var_id, 1);
		lp.declare_variable_binary(var_id);

		lp.add_equality(exp, 1);
	}
}

//C2
void ILP_RTA_PFP_DPCP_3::constraint_2(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	uint t_num = tasks.get_taskset_size();

	for(uint i = 0; i < r_num + t_num - 1; i++)
	{
		for(uint j = i + 1; j < r_num + t_num; j++)
		{
			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper_3::SAME_LOCALITY, i, j);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);

			var_id = vars.lookup(ILPDPCPMapper_3::SAME_LOCALITY, j, i);
			exp->add_term(var_id, -1);
			lp.declare_variable_binary(var_id);

			lp.add_equality(exp, 0);
		}
	}
}

//C3
void ILP_RTA_PFP_DPCP_3::constraint_3(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	uint t_num = tasks.get_taskset_size();

	for(uint i = 0; i < r_num + t_num - 1; i++)
	{
		for(uint j = i + 1; j < r_num + t_num; j++)
		{
			for(uint k = 0; k < r_num + t_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPDPCPMapper_3::SAME_LOCALITY, i, j);
				exp->add_term(var_id, -1);

				var_id = vars.lookup(ILPDPCPMapper_3::SAME_LOCALITY, k, i);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(ILPDPCPMapper_3::SAME_LOCALITY, k, j);
				exp->add_term(var_id, 1);

				lp.add_inequality(exp, 1);
			}
		}
	}
}

//C4
void ILP_RTA_PFP_DPCP_3::constraint_4(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	int p_num = processors.get_processor_num();
	int r_num = resources.get_resourceset_size();
	int t_num = tasks.get_taskset_size();

	for(uint i = 0; i < r_num + t_num - 1; i++)
	{
		LinearExpression *exp = new LinearExpression();
		uint var_id;
		
		var_id = vars.lookup(ILPDPCPMapper_3::NOT_ALONE, i);
		exp->add_term(var_id, 1);
		lp.declare_variable_binary(var_id);

		for(uint j = i + 1; j < r_num + t_num; j++)
		{
			var_id = vars.lookup(ILPDPCPMapper_3::SAME_LOCALITY, i, j);
			exp->add_term(var_id, -1);
		}

		lp.add_inequality(exp, 0);
	}

	
}

//C5
void ILP_RTA_PFP_DPCP_3::constraint_5(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	int p_num = processors.get_processor_num();
	int r_num = resources.get_resourceset_size();
	int t_num = tasks.get_taskset_size();

	LinearExpression *exp = new LinearExpression();
	uint var_id;
	
	for(uint i = 0; i < r_num + t_num - 1; i++)
	{
		var_id = vars.lookup(ILPDPCPMapper_3::NOT_ALONE, i);
		exp->add_term(var_id, -1);
	}

	lp.add_inequality(exp, p_num - (r_num + t_num));
}

/*
//C3
void ILP_RTA_PFP_DPCP_3::constraint_3(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		foreach(tasks.get_tasks(), tx)
		{
			uint x = tx->get_index();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper_3::SAME_TASK_LOCALITY, i, x);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);

			for(uint k = 1; k <= p_num; k++)
			{
				var_id = vars.lookup(ILPDPCPMapper_3::SAME_TASK_ON_PROCESSOR, i, x, k);
				exp->add_term(var_id, -1);
				lp.declare_variable_binary(var_id);
			}

			lp.add_equality(exp, 0);
		}	
	}
}

//C4
void ILP_RTA_PFP_DPCP_3::constraint_4(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();

	foreach(resources.get_resources(), ru)
	{
		uint u = ru->get_resource_id();
		foreach(resources.get_resources(), rv)
		{
			uint v = rv->get_resource_id();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper_3::SAME_RESOURCE_LOCALITY, u, v);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);

			for(uint k = 1; k <= p_num; k++)
			{
				var_id = vars.lookup(ILPDPCPMapper_3::SAME_RESOURCE_ON_PROCESSOR, u, v, k);
				exp->add_term(var_id, -1);
				lp.declare_variable_binary(var_id);
			}

			lp.add_equality(exp, 0);
		}	
	}
}

//C5
void ILP_RTA_PFP_DPCP_3::constraint_5(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		foreach(resources.get_resources(), ru)
		{
			uint u = ru->get_resource_id();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper_3::SAME_TR_LOCALITY, i, u);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);

			for(uint k = 1; k <= p_num; k++)
			{
				var_id = vars.lookup(ILPDPCPMapper_3::SAME_TR_ON_PROCESSOR, i, u, k);
				exp->add_term(var_id, -1);
				lp.declare_variable_binary(var_id);
			}

			lp.add_equality(exp, 0);
		}	
	}
}


//C3-1
void ILP_RTA_PFP_DPCP_3::constraint_3_1(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();

		foreach(tasks.get_tasks(), tx)
		{
			uint x = tx->get_index();
			for(uint k = 1; k <= p_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPDPCPMapper_3::SAME_TASK_ON_PROCESSOR, i, x, k);
				exp->add_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				var_id = vars.lookup(ILPDPCPMapper_3::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, -0.5);

				var_id = vars.lookup(ILPDPCPMapper_3::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, -0.5);

				lp.add_inequality(exp, 0);
			}
		}
	}
}

//C3-2
void ILP_RTA_PFP_DPCP_3::constraint_3_2(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();

		foreach(tasks.get_tasks(), tx)
		{
			uint x = tx->get_index();
			for(uint k = 1; k <= p_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPDPCPMapper_3::SAME_TASK_ON_PROCESSOR, i, x, k);
				exp->add_term(var_id, -1);

				var_id = vars.lookup(ILPDPCPMapper_3::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, 0.5);

				var_id = vars.lookup(ILPDPCPMapper_3::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, 0.5);

				lp.add_inequality(exp, 0.5);
			}
		}
	}
}

//C4-1
void ILP_RTA_PFP_DPCP_3::constraint_4_1(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(resources.get_resources(), ru)
	{
		uint u = ru->get_resource_id();
		foreach(resources.get_resources(), rv)
		{
			uint v = rv->get_resource_id();
			for(uint k = 1; k <= p_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPDPCPMapper_3::SAME_RESOURCE_ON_PROCESSOR, u, v, k);
				exp->add_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				var_id = vars.lookup(ILPDPCPMapper_3::RESOURCE_ASSIGNMENT, u, k);
				exp->add_term(var_id, -0.5);

				var_id = vars.lookup(ILPDPCPMapper_3::RESOURCE_ASSIGNMENT, v, k);
				exp->add_term(var_id, -0.5);

				lp.add_inequality(exp, 0);
			}
		}
	}
}

//C4-2
void ILP_RTA_PFP_DPCP_3::constraint_4_2(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(resources.get_resources(), ru)
	{
		uint u = ru->get_resource_id();
		foreach(resources.get_resources(), rv)
		{
			uint v = rv->get_resource_id();
			for(uint k = 1; k <= p_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPDPCPMapper_3::SAME_RESOURCE_ON_PROCESSOR, u, v, k);
				exp->add_term(var_id, -1);

				var_id = vars.lookup(ILPDPCPMapper_3::RESOURCE_ASSIGNMENT, u, k);
				exp->add_term(var_id, 0.5);

				var_id = vars.lookup(ILPDPCPMapper_3::RESOURCE_ASSIGNMENT, v, k);
				exp->add_term(var_id, 0.5);

				lp.add_inequality(exp, 0.5);
			}
		}
	}
}

//C5-1
void ILP_RTA_PFP_DPCP_3::constraint_5_1(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		foreach(resources.get_resources(), ru)
		{
			uint u = ru->get_resource_id();
			for(uint k = 1; k <= p_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPDPCPMapper_3::SAME_TR_ON_PROCESSOR, i, u, k);
				exp->add_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				var_id = vars.lookup(ILPDPCPMapper_3::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, -0.5);

				var_id = vars.lookup(ILPDPCPMapper_3::RESOURCE_ASSIGNMENT, u, k);
				exp->add_term(var_id, -0.5);

				lp.add_inequality(exp, 0);
			}
		}
	}
}

//C5-2
void ILP_RTA_PFP_DPCP_3::constraint_5_2(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		foreach(resources.get_resources(), ru)
		{
			uint u = ru->get_resource_id();
			for(uint k = 1; k <= p_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPDPCPMapper_3::SAME_TR_ON_PROCESSOR, i, u, k);
				exp->add_term(var_id, -1);

				var_id = vars.lookup(ILPDPCPMapper_3::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, 0.5);

				var_id = vars.lookup(ILPDPCPMapper_3::RESOURCE_ASSIGNMENT, u, k);
				exp->add_term(var_id, 0.5);

				lp.add_inequality(exp, 0.5);
			}
		}
	}
}
*/
//C6
void ILP_RTA_PFP_DPCP_3::constraint_6(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong deadline = ti->get_deadline();
		LinearExpression *exp = new LinearExpression();
		uint var_id;
		
		var_id = vars.lookup(ILPDPCPMapper_3::RESPONSE_TIME, i);
		exp->add_term(var_id, 1);
		
		lp.add_inequality(exp, deadline);
	}
}

//C7
void ILP_RTA_PFP_DPCP_3::constraint_7(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong wcet = ti->get_wcet();
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper_3::RESPONSE_TIME, i);
		exp->add_term(var_id, 1);
		//lp.declare_variable_integer(var_id);
		lp.declare_variable_bounds(var_id, true, 0, false, -1);

//		var_id = vars.lookup(ILPDPCPMapper_3::BLOCKING_TIME, i);
//		exp->add_term(var_id, -1);
		//lp.declare_variable_integer(var_id);
//		lp.declare_variable_bounds(var_id, true, 0, false, -1);

//		var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_R, i);
//		exp->add_term(var_id, -1);
		//lp.declare_variable_integer(var_id);
//		lp.declare_variable_bounds(var_id, true, 0, false, -1);

		var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_C, i);
		exp->add_term(var_id, -1);
		//lp.declare_variable_integer(var_id);
		lp.declare_variable_bounds(var_id, true, 0, false, -1);

		lp.add_equality(exp, wcet);
	}
}
/*
//C8
void ILP_RTA_PFP_DPCP_3::constraint_8(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		LinearExpression *exp = new LinearExpression();
		uint var_id;
		
		var_id = vars.lookup(ILPDPCPMapper_3::BLOCKING_TIME, i);
		exp->add_term(var_id, 1);

		foreach(ti->get_requests(), request)
		{
			uint u = request->get_resource_id();

			int N_i_u = request->get_num_requests();
		
			var_id = vars.lookup(ILPDPCPMapper_3::REQUEST_BLOCKING_TIME, i, u);
			exp->add_term(var_id, -N_i_u);
			//lp.declare_variable_integer(var_id);
			lp.declare_variable_bounds(var_id, true, 0, false, -1);
		}

		lp.add_equality(exp, 0);
	}
}
//C9
void ILP_RTA_PFP_DPCP_3::constraint_9(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		
		foreach(ti->get_requests(), ru)
		{
			uint u = ru->get_resource_id();
			Resource& res_u = resources.get_resources()[u];

			foreach_lower_priority_task(tasks.get_tasks(), (*ti), tx)
			{
				uint x = tx->get_index();
				foreach(tx->get_requests(), rv)
				{
					uint v = rv->get_resource_id();
					Resource& res_v = resources.get_resources()[v];

					if(res_u.get_ceiling() > res_v.get_ceiling())
					{
						LinearExpression *exp = new LinearExpression();
						uint var_id;

						long L_x_v = ru->get_max_length();

						var_id = vars.lookup(ILPDPCPMapper_3::REQUEST_BLOCKING_TIME, i, u);
						exp->add_term(var_id, -1);
		
						var_id = vars.lookup(ILPDPCPMapper_3::SAME_TR_LOCALITY, i, u);
						exp->add_term(var_id, -L_x_v);
		
						var_id = vars.lookup(ILPDPCPMapper_3::SAME_RESOURCE_LOCALITY, u, v);
						exp->add_term(var_id, L_x_v);

						lp.add_inequality(exp, 0);
					}
				}
			}			
		}		
	}
}
*/
//C10
void ILP_RTA_PFP_DPCP_3::constraint_10(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach(tasks.get_tasks(), tx)
		{
			uint x = tx->get_index();
			long px = tx->get_period();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			uint upper_bound = ceiling(di, px);

			var_id = vars.lookup(ILPDPCPMapper_3::PREEMPT_NUM, i, x);
			exp->add_term(var_id, 1);
			lp.declare_variable_integer(var_id);
			lp.declare_variable_bounds(var_id, true, 0, true, upper_bound);

			//var_id = vars.lookup(ILPDPCPMapper_3::RESPONSE_TIME, i);
			//exp->add_term(var_id, -1.0/px);


			lp.add_inequality(exp, upper_bound);
			//lp.add_inequality(exp, 1);
		}
	}
}
//C11
void ILP_RTA_PFP_DPCP_3::constraint_11(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach(tasks.get_tasks(), tx)
		{
			uint x = tx->get_index();
			ulong px = tx->get_period();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper_3::PREEMPT_NUM, i, x);
			exp->add_term(var_id, -1);

			var_id = vars.lookup(ILPDPCPMapper_3::RESPONSE_TIME, i);
			exp->add_term(var_id, 1.0/px);

			lp.add_inequality(exp, 0);
		}
	}
}

//C12
void ILP_RTA_PFP_DPCP_3::constraint_12(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach(tasks.get_tasks(), tx)
		{
			uint x = tx->get_index();
			ulong px = tx->get_period();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			uint bound = ceiling(di, px);

			var_id = vars.lookup(ILPDPCPMapper_3::PREEMPT_NUM, i, x);
			exp->add_term(var_id, 1);

			var_id = vars.lookup(ILPDPCPMapper_3::TBT_PREEMPT_NUM, i, x);
			exp->add_term(var_id, -1);
			lp.declare_variable_integer(var_id);
			lp.declare_variable_bounds(var_id, true, 0, true, bound);

			var_id = vars.lookup(ILPDPCPMapper_3::SAME_LOCALITY, i, x);
			exp->add_term(var_id, bound);

			lp.add_inequality(exp, bound);
		}
	}
}

//C13
void ILP_RTA_PFP_DPCP_3::constraint_13(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint t_num = tasks.get_taskset_size();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach(tasks.get_tasks(), tx)
		{
			uint x = tx->get_index();
			ulong px = tx->get_period();

			foreach(tx->get_requests(), request_v)
			{
				uint v = request_v->get_resource_id();
				int N_x_v = request_v->get_num_requests();

				LinearExpression *exp = new LinearExpression();
				uint var_id;

				uint bound = ceiling(di, px);

				var_id = vars.lookup(ILPDPCPMapper_3::TBT_PREEMPT_NUM, i, x);
				exp->add_term(var_id, N_x_v);

				var_id = vars.lookup(ILPDPCPMapper_3::RBT_PREEMPT_NUM, i, x, t_num + v);
				exp->add_term(var_id, -1);
				lp.declare_variable_integer(var_id);
				lp.declare_variable_bounds(var_id, true, 0, true, bound);

				var_id = vars.lookup(ILPDPCPMapper_3::SAME_LOCALITY, i, t_num + v);
				exp->add_term(var_id, N_x_v*bound);

				lp.add_inequality(exp, N_x_v*bound);
			}
		}
	}
}

//C14
void ILP_RTA_PFP_DPCP_3::constraint_14(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint t_num = tasks.get_taskset_size();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach(tasks.get_tasks(), tx)
		{
			uint x = tx->get_index();
			ulong px = tx->get_period();

			foreach(ti->get_requests(), request_u)
			{
				uint u = request_u->get_resource_id();

				foreach(tx->get_requests(), request_v)
				{
					uint v = request_v->get_resource_id();
					int N_x_v = request_v->get_num_requests();

					LinearExpression *exp = new LinearExpression();
					uint var_id;

					uint bound = ceiling(di, px);

					var_id = vars.lookup(ILPDPCPMapper_3::TBT_PREEMPT_NUM, i, x);
					exp->add_term(var_id, N_x_v);

					var_id = vars.lookup(ILPDPCPMapper_3::RBR_PREEMPT_NUM, i, x, t_num + u, t_num + v);
					exp->add_term(var_id, -1);
					lp.declare_variable_integer(var_id);
					lp.declare_variable_bounds(var_id, true, 0, true, bound);

					var_id = vars.lookup(ILPDPCPMapper_3::SAME_LOCALITY, t_num + u, t_num + v);
					exp->add_term(var_id, N_x_v*bound);

					lp.add_inequality(exp, N_x_v*bound);
				}
			}
		}
	}
}
/*
//C15
void ILP_RTA_PFP_DPCP_3::constraint_15(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint t_num = tasks.get_taskset_size();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();

		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_R, i);
		exp->add_term(var_id, 1);
		//lp.declare_variable_integer(var_id);
		lp.declare_variable_bounds(var_id, true, 0, false, -1);

		foreach(ti->get_requests(), request_u)
		{
			uint u = request_u->get_resource_id();

			var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_R_RESOURCE, i, t_num + u);
			exp->add_term(var_id, -1);
			//lp.declare_variable_integer(var_id);
			lp.declare_variable_bounds(var_id, true, 0, false, -1);
		}

		lp.add_equality(exp, 0);
	}
}

//C16
void ILP_RTA_PFP_DPCP_3::constraint_16(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint t_num = tasks.get_taskset_size();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach(ti->get_requests(), request_u)
		{
			uint u = request_u->get_resource_id();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_R_RESOURCE, i, u);
			exp->add_term(var_id, -1);
			
			foreach_higher_priority_task(tasks.get_tasks(), (*ti), tx)
			{
				uint x = tx->get_index();
				ulong px = tx->get_period();

				foreach(tx->get_requests(), request_v)
				{
					uint v = request_v->get_resource_id();
					int N_x_v = request_v->get_num_requests();
					long L_x_v = request_v->get_max_length();

					var_id = vars.lookup(ILPDPCPMapper_3::RBR_PREEMPT_NUM, i, x, u, v);
					exp->add_term(var_id, L_x_v);

					int term = ceiling(di, px);

					var_id = vars.lookup(ILPDPCPMapper_3::SAME_TR_LOCALITY, i, u);
					exp->add_term(var_id, -1*term*N_x_v*L_x_v);
				}
			}

			lp.add_inequality(exp, 0);
		}
	}
}
*/
//C17
void ILP_RTA_PFP_DPCP_3::constraint_17(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();

		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_C, i);
		exp->add_term(var_id, 1);
		//lp.declare_variable_integer(var_id);
		lp.declare_variable_bounds(var_id, true, 0, false, -1);
		
		foreach_higher_priority_task(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_index();

			var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_C_TASK, i, x);
			exp->add_term(var_id, -1);
			//lp.declare_variable_integer(var_id);
			lp.declare_variable_bounds(var_id, true, 0, false, -1);
		}
/*
		foreach_task_except(tasks.get_tasks(), (*ti), ty)
		{
			uint y = ty->get_index();
			foreach(ty->get_requests(), request_u)
			{
				uint u = request_u->get_resource_id();
				
				var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_C_RESOURCE, i, y,u);
				exp->add_term(var_id, -1);
				lp.declare_variable_bounds(var_id, true, 0, false, -1);
			}
		}
*/
		lp.add_equality(exp, 0);
	}
}

//C18
void ILP_RTA_PFP_DPCP_3::constraint_18(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();
		
		foreach_higher_priority_task(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_index();
			ulong px = tx->get_period();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_C_TASK, i, x);
			exp->add_term(var_id, -1);

			long NC_WCET_x = tx->get_wcet_non_critical_sections();

			var_id = vars.lookup(ILPDPCPMapper_3::TBT_PREEMPT_NUM, i, x);
			exp->add_term(var_id, NC_WCET_x);

			lp.add_inequality(exp, 0);
		}
	}
}

/*
//C19
void ILP_RTA_PFP_DPCP_3::constraint_19(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();
		
		foreach_lower_priority_task(tasks.get_tasks(), (*ti), ty)
		{
			uint y = ty->get_index();

			foreach(ty->get_requests(), request_u)
			{
				uint u = request_u->get_resource_id();
				long L_y_u = request_u->get_max_length();
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_C_RESOURCE, i, y, u);
				exp->add_term(var_id, -1);

				var_id = vars.lookup(ILPDPCPMapper_3::RBT_PREEMPT_NUM, i, y, u);
				exp->add_term(var_id, L_y_u);

				lp.add_inequality(exp, 0);
			}
		}
	}
}
//C20
void ILP_RTA_PFP_DPCP_3::constraint_20(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();

		for(uint k = 1; k <= p_num; k++)
		{
			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper_3::TASK_ONLY_PROCESSOR, i, k);
			exp->add_term(var_id, 1);

			var_id = vars.lookup(ILPDPCPMapper_3::LOCALITY_ASSIGNMENT, i, k);
			exp->add_term(var_id, -0.5);

			var_id = vars.lookup(ILPDPCPMapper_3::APPLICATION_CORE, k);
			exp->add_term(var_id, -0.5);

			lp.add_inequality(exp, 0);
		}
	}
}

//C21
void ILP_RTA_PFP_DPCP_3::constraint_21(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();

		for(uint k = 1; k <= p_num; k++)
		{
			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper_3::TASK_ONLY_PROCESSOR, i, k);
			exp->add_term(var_id, -1);

			var_id = vars.lookup(ILPDPCPMapper_3::LOCALITY_ASSIGNMENT, i, k);
			exp->add_term(var_id, 0.5);

			var_id = vars.lookup(ILPDPCPMapper_3::APPLICATION_CORE, k);
			exp->add_term(var_id, 0.5);

			lp.add_inequality(exp, 0.5);
		}
	}
}

//C22
void ILP_RTA_PFP_DPCP_3::constraint_22(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();
		
		foreach_higher_priority_task(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_index();
			ulong px = tx->get_period();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_C_TASK, i, x);
			exp->add_term(var_id, -1);

			long NC_WCET_x = tx->get_wcet_non_critical_sections();

			var_id = vars.lookup(ILPDPCPMapper_3::TBT_PREEMPT_NUM, i, x);
			exp->add_term(var_id, NC_WCET_x);

			uint bound = ceiling(di, px);

			long WCET_x = tx->get_wcet();

			var_id = vars.lookup(ILPDPCPMapper_3::TASK_ONLY, i);
			exp->add_term(var_id, -1*bound*WCET_x);

			lp.add_inequality(exp, bound*WCET_x);
		}
	}
}

//C22-1
void ILP_RTA_PFP_DPCP_3::constraint_22_1(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();
		
		foreach_higher_priority_task(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_index();
			ulong px = tx->get_period();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper_3::INTERFERENCE_TIME_C_TASK, i, x);
			exp->add_term(var_id, -1);

			long NC_WCET_x = tx->get_wcet_non_critical_sections();

			var_id = vars.lookup(ILPDPCPMapper_3::TBT_PREEMPT_NUM, i, x);
			exp->add_term(var_id, NC_WCET_x);

			uint bound = ceiling(di, px);
		
			foreach(tx->get_requests(), request_v)
			{
				uint v = request_v->get_resource_id();
				int N_x_v = request_v->get_num_requests();
				long L_x_v = request_v->get_max_length();

				var_id = vars.lookup(ILPDPCPMapper_3::RBT_PREEMPT_NUM, i, x, v);
				exp->add_term(var_id, N_x_v*L_x_v);
			}

			long WCET_x = tx->get_wcet();

			var_id = vars.lookup(ILPDPCPMapper_3::TASK_ONLY, i);
			exp->add_term(var_id, -1*bound*WCET_x);

			lp.add_inequality(exp, 0);
		}
	}
}
*/
void ILP_RTA_PFP_DPCP_3::constraint_23(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	uint t_num = tasks.get_taskset_size();

	for(uint i = 0; i < t_num; i++)
	{
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		for(uint j = 0; j < t_num; j++)
		{
			Task& task = tasks.get_task_by_index(j);
			fraction_t utilization = task.get_utilization();
			
			var_id = vars.lookup(ILPDPCPMapper_3::SAME_LOCALITY, i, j);
			exp->add_term(var_id, utilization.get_d());
		}

		lp.add_inequality(exp, 1);
	}

}

void constraint_24(LinearProgram& lp, ILPDPCPMapper_3& vars)
{
	
}

bool ILP_RTA_PFP_DPCP_3::alloc_schedulable() {}































