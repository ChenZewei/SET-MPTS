#include "ilp_rta_pfp_dpcp.h"
#include <lp.h>
#include <solution.h>
#include <sstream>
#include <iostream>
#include <assert.h>
#include "iteration-helper.h"
#include "math-helper.h"

////////////////////ILPDPCPMapper////////////////////
static uint64_t ILPDPCPMapper::encode_request(uint64_t type, uint64_t part_1, uint64_t part_2, uint64_t part_3, uint64_t part_4)
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

static uint64_t ILPDPCPMapper::get_type(uint64_t var)
{
	return (var >> 40) & (uint64_t) 0x1f; //5 bits
}

static uint64_t ILPDPCPMapper::get_part_1(uint64_t var)
{
	return (var >> 30) & (uint64_t) 0x3ff; //10 bits
}

static uint64_t ILPDPCPMapper::get_part_2(uint64_t var)
{
	return (var >> 20) & (uint64_t) 0x3ff; //10 bits
}

static uint64_t ILPDPCPMapper::get_part_3(uint64_t var)
{
	return (var >> 10) & (uint64_t) 0x3ff; //10 bits
}

static uint64_t ILPDPCPMapper::get_part_4(uint64_t var)
{
	return var & (uint64_t) 0x3ff; //10 bits
}

ILPDPCPMapper::ILPDPCPMapper(uint start_var): VarMapperBase(start_var) {}

uint ILPDPCPMapper::lookup(uint type, uint part_1, uint part_2, uint part_3, uint part_4)
{
	uint64_t key = encode_request(type, part_1, part_2, part_3, part_4);
	
//cout<<"string:"<<key2str(key)<<endl;
	uint var = var_for_key(key);
	return var;
}

string ILPDPCPMapper::var2str(unsigned int var) const
{
	uint64_t key;

	if (search_key_for_var(var, key))
	{
		return key2str(key);
	}
	else
		return "<?>";
}

string ILPDPCPMapper::key2str(uint64_t key) const
{
	ostringstream buf;

	switch (get_type(key))
	{
		case ILPDPCPMapper::LOCALITY_ASSIGNMENT:
			buf << "A[";
			break;
		case ILPDPCPMapper::RESOURCE_ASSIGNMENT:
			buf << "Q[";
			break;
		case ILPDPCPMapper::SAME_TASK_LOCALITY:
			buf << "U[";
			break;
		case ILPDPCPMapper::SAME_RESOURCE_LOCALITY:
			buf << "V[";
			break;
		case ILPDPCPMapper::SAME_TR_LOCALITY:
			buf << "W[";
			break;
		case ILPDPCPMapper::APPLICATION_CORE:
			buf << "AC[";
			break;
		case ILPDPCPMapper::TBT_PREEMPT_NUM:
			buf << "H[";
			break;
		case ILPDPCPMapper::RBT_PREEMPT_NUM:
			buf << "H[";
			break;
		case ILPDPCPMapper::RBR_PREEMPT_NUM:
			buf << "H[";
			break;
		case ILPDPCPMapper::RESPONSE_TIME:
			buf << "R[";
			break;
		case ILPDPCPMapper::BLOCKING_TIME:
			buf << "B[";
			break;
		case ILPDPCPMapper::REQUEST_BLOCKING_TIME:
			buf << "b[";
			break;
		case ILPDPCPMapper::INTERFERENCE_TIME_R:
			buf << "I_R[";
			break;
		case ILPDPCPMapper::INTERFERENCE_TIME_R_RESOURCE:
			buf << "I_R[";
			break;
		case ILPDPCPMapper::INTERFERENCE_TIME_C:
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

////////////////////ILP_RTA_PFP_DPCP////////////////////
ILP_RTA_PFP_DPCP::ILP_RTA_PFP_DPCP(): PartitionedSched(true, RTA, FIX_PRIORITY, DPCP, "", "DPCP") {}

ILP_RTA_PFP_DPCP::ILP_RTA_PFP_DPCP(TaskSet tasks, ProcessorSet processors, ResourceSet resources): PartitionedSched(true, RTA, FIX_PRIORITY, DPCP, "", "DPCP")
{
	this->tasks = tasks;
	this->processors = processors;
	this->resources = resources;

	this->resources.update(&(this->tasks));
	this->processors.update(&(this->tasks), &(this->resources));
	
	this->tasks.RM_Order();
	this->processors.init();
}

ILP_RTA_PFP_DPCP::~ILP_RTA_PFP_DPCP(){}

bool ILP_RTA_PFP_DPCP::is_schedulable()
{
	if(0 == tasks.get_tasks().size())
		return true;

	ILPDPCPMapper vars;
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

		for(uint r = 0; r < resources.get_resourceset_size(); r++)
		{
			Resource& resoruce = resources.get_resources()[r];
cout<<"/===========Resource "<<r<<"===========/"<<endl;
			LinearExpression *exp = new LinearExpression();
			double result;
		
			cout<<"|===========Locailty Assignment===========|"<<endl;
			for(uint k = 1; k <= processors.get_processor_num(); k++)
			{
				construct_exp(vars, exp, ILPDPCPMapper::RESOURCE_ASSIGNMENT, r, k);
				result = rb_solution->evaluate(*exp);
cout<<"Q_"<<r<<"_"<<k<<":"<<result<<endl;
			}
		}


cout<<"|===========SAME_RESOURCE_LOCALITY===========|"<<endl;
		for(uint u = 0; u < resources.get_resourceset_size(); u++)
		{
			
			LinearExpression *exp = new LinearExpression();
			double result;
			for(uint v = 0; v < resources.get_resourceset_size(); v++)
			{
				construct_exp(vars, exp, ILPDPCPMapper::SAME_RESOURCE_LOCALITY, u, v);
				result = rb_solution->evaluate(*exp);
cout<<"V_"<<u<<"_"<<v<<":"<<result<<endl;
			}
		}

cout<<"|===========SAME_TR_LOCALITY===========|"<<endl;
		for(uint i = 0; i < tasks.get_tasks().size(); i++)
		{
			
			LinearExpression *exp = new LinearExpression();
			double result;
			for(uint v = 0; v < resources.get_resourceset_size(); v++)
			{
				construct_exp(vars, exp, ILPDPCPMapper::SAME_TR_LOCALITY, i, v);
				result = rb_solution->evaluate(*exp);
cout<<"W_"<<i<<"_"<<v<<":"<<result<<endl;
			}
		}

		for(uint i = 0; i < tasks.get_tasks().size(); i++)
		{
			Task& task = tasks.get_task_by_index(i);
cout<<"/===========Task "<<i<<"===========/"<<endl;
			LinearExpression *exp = new LinearExpression();
			double result;

			construct_exp(vars, exp, ILPDPCPMapper::RESPONSE_TIME, i);
			result = rb_solution->evaluate(*exp);
cout<<"R"<<i<<":"<<result<<endl;

			construct_exp(vars, exp, ILPDPCPMapper::BLOCKING_TIME, i);
			result = rb_solution->evaluate(*exp);
cout<<"B_"<<i<<":"<<result<<endl;

		
			foreach(task.get_requests(), request)
			{
				uint u = request->get_resource_id();
//cout<<"B_"<<i<<"_"<<u<<":"<<vars.lookup(ILPDPCPMapper::REQUEST_BLOCKING_TIME, i, u)<<endl;
				construct_exp(vars, exp, ILPDPCPMapper::REQUEST_BLOCKING_TIME, i, u);
				result = rb_solution->evaluate(*exp);
cout<<"B_"<<i<<"_"<<u<<":"<<result<<endl;
			}

			construct_exp(vars, exp, ILPDPCPMapper::INTERFERENCE_TIME_R, i);
			result = rb_solution->evaluate(*exp);
cout<<"IR_"<<i<<":"<<result<<endl;

			foreach(task.get_requests(), request)
			{
				uint u = request->get_resource_id();
//cout<<"B_"<<i<<"_"<<u<<":"<<vars.lookup(ILPDPCPMapper::REQUEST_BLOCKING_TIME, i, u)<<endl;
				construct_exp(vars, exp, ILPDPCPMapper::INTERFERENCE_TIME_R_RESOURCE, i, u);
				result = rb_solution->evaluate(*exp);
cout<<"IR_"<<i<<"_"<<u<<":"<<result<<endl;
			}

			construct_exp(vars, exp, ILPDPCPMapper::INTERFERENCE_TIME_C, i);
			result = rb_solution->evaluate(*exp);
cout<<"IC_"<<i<<":"<<result<<endl;


cout<<"|===========Locailty Assignment===========|"<<endl;
			for(uint k = 1; k <= processors.get_processor_num(); k++)
			{
				construct_exp(vars, exp, ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
				result = rb_solution->evaluate(*exp);
cout<<"A_"<<i<<"_"<<k<<":"<<result<<endl;
			}
/*
*/
cout<<"C_"<<i<<":"<<tasks.get_tasks()[i].get_wcet()<<endl;
cout<<"D_"<<i<<":"<<tasks.get_tasks()[i].get_deadline()<<endl;
			delete exp;
		}
#endif

		rb_solution->show_error();
		delete rb_solution;
		return true;
	}
	else
	{
		rb_solution->show_error();
	}

	delete rb_solution;
	return false;
}

void ILP_RTA_PFP_DPCP::construct_exp(ILPDPCPMapper& vars, LinearExpression *exp, uint type, uint part_1, uint part_2, uint part_3, uint part_4)
{
	exp->get_terms().clear();
	uint var_id;

	var_id = vars.lookup(type, part_1, part_2, part_3, part_4);
	exp->add_term(var_id, 1);

#if ILP_SOLUTION_VAR_CHECK == 1
//cout<<vars.var2str(var_id)<<":"<<endl;
#endif
}

void ILP_RTA_PFP_DPCP::set_objective(LinearProgram& lp, ILPDPCPMapper& vars)
{
	//any objective function is okay.
	//assert(0 < tasks.get_tasks().size());

	LinearExpression *obj = new LinearExpression();
	uint var_id;

	foreach(tasks.get_tasks(), task)
	{
		uint i = task->get_index();
		var_id = vars.lookup(ILPDPCPMapper::RESPONSE_TIME, i, 0, 0);
		obj->add_term(var_id, 1);
	}

	lp.set_objective(obj);
}

void ILP_RTA_PFP_DPCP::add_constraints(LinearProgram& lp, ILPDPCPMapper& vars)
{
	constraint_1(lp, vars);
	constraint_2(lp, vars);
	constraint_3(lp, vars);
	constraint_3_1(lp, vars);
	constraint_3_2(lp, vars);
	constraint_4(lp, vars);
	constraint_4_1(lp, vars);
	constraint_4_2(lp, vars);
	constraint_5(lp, vars);
	constraint_5_1(lp, vars);
	constraint_5_2(lp, vars);
	constraint_6(lp, vars);
	constraint_7(lp, vars);
	constraint_8(lp, vars);
	constraint_9(lp, vars);
	constraint_10(lp, vars);
	constraint_11(lp, vars);
	constraint_12(lp, vars);
	constraint_13(lp, vars);
	constraint_14(lp, vars);
	constraint_14_1(lp, vars);
	constraint_15(lp, vars);
	constraint_16(lp, vars);
	constraint_17(lp, vars);
	constraint_18(lp, vars);
	constraint_19(lp, vars);
	constraint_20(lp, vars);
	constraint_21(lp, vars);
	constraint_22(lp, vars);
	constraint_22_1(lp, vars);
	constraint_23(lp, vars);
/*
*/
}

////////////////////Expressions////////////////////
//C1
void ILP_RTA_PFP_DPCP::constraint_1(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();

	foreach(tasks.get_tasks(), ti)
	{
		LinearExpression *exp = new LinearExpression();
		uint var_id;
		uint  i = ti->get_index();
		for(uint k = 1; k <= p_num; k++)
		{
			var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
//cout<<"A_"<<i<<"_"<<k<<":"<<var_id<<endl;
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);
		}
		
		lp.add_equality(exp, 1);
	}
}
//C2
void ILP_RTA_PFP_DPCP::constraint_2(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();

	foreach(resources.get_resources(), resource)
	{
		LinearExpression *exp = new LinearExpression();
		uint  i = resource->get_resource_id();
		for(uint k = 1; k <= p_num; k++)
		{
			uint var_id;
			var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, i, k);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);
		}
		
		lp.add_equality(exp, 1);
	}
}

//C3
void ILP_RTA_PFP_DPCP::constraint_3(LinearProgram& lp, ILPDPCPMapper& vars)
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

			var_id = vars.lookup(ILPDPCPMapper::SAME_TASK_LOCALITY, i, x);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);

			for(uint k = 1; k <= p_num; k++)
			{
				var_id = vars.lookup(ILPDPCPMapper::SAME_TASK_ON_PROCESSOR, i, x, k);
				exp->add_term(var_id, -1);
				lp.declare_variable_binary(var_id);
			}

			lp.add_equality(exp, 0);
		}	
	}
}

//C4
void ILP_RTA_PFP_DPCP::constraint_4(LinearProgram& lp, ILPDPCPMapper& vars)
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

			var_id = vars.lookup(ILPDPCPMapper::SAME_RESOURCE_LOCALITY, u, v);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);

			for(uint k = 1; k <= p_num; k++)
			{
				var_id = vars.lookup(ILPDPCPMapper::SAME_RESOURCE_ON_PROCESSOR, u, v, k);
				exp->add_term(var_id, -1);
				lp.declare_variable_binary(var_id);
			}

			lp.add_equality(exp, 0);
		}	
	}
}

//C5
void ILP_RTA_PFP_DPCP::constraint_5(LinearProgram& lp, ILPDPCPMapper& vars)
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

			var_id = vars.lookup(ILPDPCPMapper::SAME_TR_LOCALITY, i, u);
			exp->add_term(var_id, 1);
			lp.declare_variable_binary(var_id);

			for(uint k = 1; k <= p_num; k++)
			{
				var_id = vars.lookup(ILPDPCPMapper::SAME_TR_ON_PROCESSOR, i, u, k);
				exp->add_term(var_id, -1);
				lp.declare_variable_binary(var_id);
			}

			lp.add_equality(exp, 0);
		}	
	}
}


//C3-1
void ILP_RTA_PFP_DPCP::constraint_3_1(LinearProgram& lp, ILPDPCPMapper& vars)
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

				var_id = vars.lookup(ILPDPCPMapper::SAME_TASK_ON_PROCESSOR, i, x, k);
				exp->add_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, -0.5);

				var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, -0.5);

				lp.add_inequality(exp, 0);
			}
		}
	}
}

//C3-2
void ILP_RTA_PFP_DPCP::constraint_3_2(LinearProgram& lp, ILPDPCPMapper& vars)
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

				var_id = vars.lookup(ILPDPCPMapper::SAME_TASK_ON_PROCESSOR, i, x, k);
				exp->add_term(var_id, -1);

				var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, 0.5);

				var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, 0.5);

				lp.add_inequality(exp, 0.5);
			}
		}
	}
}

//C4-1
void ILP_RTA_PFP_DPCP::constraint_4_1(LinearProgram& lp, ILPDPCPMapper& vars)
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

				var_id = vars.lookup(ILPDPCPMapper::SAME_RESOURCE_ON_PROCESSOR, u, v, k);
				exp->add_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, u, k);
				exp->add_term(var_id, -0.5);

				var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, v, k);
				exp->add_term(var_id, -0.5);

				lp.add_inequality(exp, 0);
			}
		}
	}
}

//C4-2
void ILP_RTA_PFP_DPCP::constraint_4_2(LinearProgram& lp, ILPDPCPMapper& vars)
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

				var_id = vars.lookup(ILPDPCPMapper::SAME_RESOURCE_ON_PROCESSOR, u, v, k);
				exp->add_term(var_id, -1);

				var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, u, k);
				exp->add_term(var_id, 0.5);

				var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, v, k);
				exp->add_term(var_id, 0.5);

				lp.add_inequality(exp, 0.5);
			}
		}
	}
}

//C5-1
void ILP_RTA_PFP_DPCP::constraint_5_1(LinearProgram& lp, ILPDPCPMapper& vars)
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

				var_id = vars.lookup(ILPDPCPMapper::SAME_TR_ON_PROCESSOR, i, u, k);
				exp->add_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, -0.5);

				var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, u, k);
				exp->add_term(var_id, -0.5);

				lp.add_inequality(exp, 0);
			}
		}
	}
}

//C5-2
void ILP_RTA_PFP_DPCP::constraint_5_2(LinearProgram& lp, ILPDPCPMapper& vars)
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

				var_id = vars.lookup(ILPDPCPMapper::SAME_TR_ON_PROCESSOR, i, u, k);
				exp->add_term(var_id, -1);

				var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, 0.5);

				var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, u, k);
				exp->add_term(var_id, 0.5);

				lp.add_inequality(exp, 0.5);
			}
		}
	}
}

//C6
void ILP_RTA_PFP_DPCP::constraint_6(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();
	for(uint k = 1; k <= p_num; k++)
	{
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper::APPLICATION_CORE, k);
		exp->add_term(var_id, -1);
		lp.declare_variable_binary(var_id);


		foreach(resources.get_resources(), resource)
		{
			uint r = resource->get_resource_id();
			var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, r, k);
			exp->add_term(var_id, -1);
		}


		lp.add_inequality(exp, -1);
	}
}
//C7
void ILP_RTA_PFP_DPCP::constraint_7(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	for(uint k = 1; k <= p_num; k++)
	{
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper::APPLICATION_CORE, k);
		exp->add_term(var_id, 1);


		foreach(resources.get_resources(), resource)
		{
			uint r = resource->get_resource_id();
			var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, r, k);
			exp->add_term(var_id, 1.0/r_num);
		}


		lp.add_inequality(exp, 1);
	}
}
//C8
void ILP_RTA_PFP_DPCP::constraint_8(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong wcet = ti->get_wcet();
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper::RESPONSE_TIME, i);
		exp->add_term(var_id, 1);
		//lp.declare_variable_integer(var_id);
		lp.declare_variable_bounds(var_id, true, 0, false, -1);

		var_id = vars.lookup(ILPDPCPMapper::BLOCKING_TIME, i);
		exp->add_term(var_id, -1);
		//lp.declare_variable_integer(var_id);
		lp.declare_variable_bounds(var_id, true, 0, false, -1);

		var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_R, i);
		exp->add_term(var_id, -1);
		//lp.declare_variable_integer(var_id);
		lp.declare_variable_bounds(var_id, true, 0, false, -1);

		var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_C, i);
		exp->add_term(var_id, -1);
		//lp.declare_variable_integer(var_id);
		lp.declare_variable_bounds(var_id, true, 0, false, -1);

		lp.add_equality(exp, wcet);
	}
}
//C9
void ILP_RTA_PFP_DPCP::constraint_9(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong deadline = ti->get_deadline();
		LinearExpression *exp = new LinearExpression();
		uint var_id;
		
		var_id = vars.lookup(ILPDPCPMapper::RESPONSE_TIME, i);
		exp->add_term(var_id, 1);
		
		lp.add_inequality(exp, deadline);
	}
}
//C10
void ILP_RTA_PFP_DPCP::constraint_10(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		LinearExpression *exp = new LinearExpression();
		uint var_id;
		
		var_id = vars.lookup(ILPDPCPMapper::BLOCKING_TIME, i);
		exp->add_term(var_id, 1);

		foreach(ti->get_requests(), request)
		{
			uint u = request->get_resource_id();

			int N_i_u = request->get_num_requests();
		
			var_id = vars.lookup(ILPDPCPMapper::REQUEST_BLOCKING_TIME, i, u);
			exp->add_term(var_id, -N_i_u);
			//lp.declare_variable_integer(var_id);
			lp.declare_variable_bounds(var_id, true, 0, false, -1);
//cout<<"B_"<<i<<"_"<<u<<":"<<var_id<<endl;
		}

		lp.add_equality(exp, 0);
	}
}
//C11
void ILP_RTA_PFP_DPCP::constraint_11(LinearProgram& lp, ILPDPCPMapper& vars)
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

//cout<<"Task "<<i<<" -> Res "<<u<<" block by "<<"Task "<<x<<" -> Res "<<v<<endl;
//cout<<"ceiling res "<<u<<":"<<res_u.get_ceiling()<<" ceiling res "<<v<<":"<<res_v.get_ceiling()<<endl;

					if(res_u.get_ceiling() > res_v.get_ceiling())
					{
						LinearExpression *exp = new LinearExpression();
						uint var_id;

						long L_x_v = ru->get_max_length();

//cout<<"L_x_v:"<<L_x_v<<endl;
		
						var_id = vars.lookup(ILPDPCPMapper::REQUEST_BLOCKING_TIME, i, u);
						exp->add_term(var_id, -1);
		
						var_id = vars.lookup(ILPDPCPMapper::SAME_TR_LOCALITY, i, u);
						exp->add_term(var_id, -L_x_v);
		
						var_id = vars.lookup(ILPDPCPMapper::SAME_RESOURCE_LOCALITY, u, v);
						exp->add_term(var_id, L_x_v);

						lp.add_equality(exp, 0);
					}
				}
			}			
		}		
	}
}
//C12
void ILP_RTA_PFP_DPCP::constraint_12(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();

		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_R, i);
		exp->add_term(var_id, 1);
		//lp.declare_variable_integer(var_id);
		lp.declare_variable_bounds(var_id, true, 0, false, -1);

		foreach(ti->get_requests(), request_u)
		{
			uint u = request_u->get_resource_id();

			var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_R_RESOURCE, i, u);
			exp->add_term(var_id, -1);
			//lp.declare_variable_integer(var_id);
			lp.declare_variable_bounds(var_id, true, 0, false, -1);
		}

		lp.add_equality(exp, 0);
	}
}
//C13
void ILP_RTA_PFP_DPCP::constraint_13(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach_task_except(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_index();
			long px = tx->get_period();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			uint upper_bound = ceiling(di, px);

			var_id = vars.lookup(ILPDPCPMapper::PREEMPT_NUM, i, x);
			exp->add_term(var_id, 1);
			lp.declare_variable_integer(var_id);
			lp.declare_variable_bounds(var_id, true, 0, true, upper_bound);

			var_id = vars.lookup(ILPDPCPMapper::RESPONSE_TIME, i);
			exp->add_term(var_id, -1.0/px);


			//lp.add_inequality(exp, upper_bound);
			lp.add_inequality(exp, 1);
		}
	}
}
//C14
void ILP_RTA_PFP_DPCP::constraint_14(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach_task_except(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_index();
			ulong px = tx->get_period();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper::PREEMPT_NUM, i, x);
			exp->add_term(var_id, -1);

			var_id = vars.lookup(ILPDPCPMapper::RESPONSE_TIME, i);
			exp->add_term(var_id, 1.0/px);

			lp.add_inequality(exp, 0);
		}

	}
}
//C14-1
void ILP_RTA_PFP_DPCP::constraint_14_1(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach_task_except(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_index();
			ulong px = tx->get_period();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			uint bound = ceiling(di, px);

			var_id = vars.lookup(ILPDPCPMapper::PREEMPT_NUM, i, x);
			exp->add_term(var_id, 1);

			var_id = vars.lookup(ILPDPCPMapper::TBT_PREEMPT_NUM, i, x);
			exp->add_term(var_id, -1);
			//lp.declare_variable_integer(var_id);
			lp.declare_variable_bounds(var_id, true, 0, true, bound);

			var_id = vars.lookup(ILPDPCPMapper::SAME_TASK_LOCALITY, i, x);
			exp->add_term(var_id, bound);

			lp.add_inequality(exp, bound);
		}
	}
}
//C15
void ILP_RTA_PFP_DPCP::constraint_15(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach_task_except(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_index();
			ulong px = tx->get_period();

			foreach(tx->get_requests(), request_v)
			{
				uint v = request_v->get_resource_id();

				LinearExpression *exp = new LinearExpression();
				uint var_id;

				uint bound = ceiling(di, px);

				var_id = vars.lookup(ILPDPCPMapper::TBT_PREEMPT_NUM, i, x);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(ILPDPCPMapper::RBT_PREEMPT_NUM, i, x, v);
				exp->add_term(var_id, -1);
				//lp.declare_variable_integer(var_id);
				lp.declare_variable_bounds(var_id, true, 0, true, bound);

				var_id = vars.lookup(ILPDPCPMapper::SAME_TR_LOCALITY, i, v);
				exp->add_term(var_id, bound);

				lp.add_inequality(exp, bound);
			}
		}
	}
}
//C16
void ILP_RTA_PFP_DPCP::constraint_16(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach_task_except(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_index();
			ulong px = tx->get_period();

			foreach(ti->get_requests(), request_u)
			{
				uint u = request_u->get_resource_id();

				foreach(tx->get_requests(), request_v)
				{
					uint v = request_v->get_resource_id();

					LinearExpression *exp = new LinearExpression();
					uint var_id;

					uint bound = ceiling(di, px);

					var_id = vars.lookup(ILPDPCPMapper::TBT_PREEMPT_NUM, i, x);
					exp->add_term(var_id, 1);

					var_id = vars.lookup(ILPDPCPMapper::RBR_PREEMPT_NUM, i, x, u, v);
					exp->add_term(var_id, -1);
					//lp.declare_variable_integer(var_id);
					lp.declare_variable_bounds(var_id, true, 0, true, bound);

					var_id = vars.lookup(ILPDPCPMapper::SAME_RESOURCE_LOCALITY, u, v);
					exp->add_term(var_id, bound);

					lp.add_inequality(exp, bound);
				}
			}
		}
	}
}
//C17
void ILP_RTA_PFP_DPCP::constraint_17(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		ulong di = ti->get_deadline();

		foreach(ti->get_requests(), request_u)
		{
			uint u = request_u->get_resource_id();

			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_R_RESOURCE, i, u);
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

					var_id = vars.lookup(ILPDPCPMapper::RBR_PREEMPT_NUM, i, x, u, v);
					exp->add_term(var_id, N_x_v*L_x_v);

					int term = ceiling(di, px);

					var_id = vars.lookup(ILPDPCPMapper::SAME_TR_LOCALITY, i, u);
					exp->add_term(var_id, -1*term*N_x_v*L_x_v);
				}
			}

			
			lp.add_inequality(exp, 0);
		}

	}
}
//C18
void ILP_RTA_PFP_DPCP::constraint_18(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();

		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_C, i);
		exp->add_term(var_id, 1);
		//lp.declare_variable_integer(var_id);
		lp.declare_variable_bounds(var_id, true, 0, false, -1);
		
		foreach_higher_priority_task(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_index();

			var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_C_TASK, i, x);
			exp->add_term(var_id, -1);
			//lp.declare_variable_integer(var_id);
			lp.declare_variable_bounds(var_id, true, 0, false, -1);
		}
		lp.add_equality(exp, 0);
	}
}
//C19
void ILP_RTA_PFP_DPCP::constraint_19(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();
		
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper::TASK_ONLY, i);
		exp->add_term(var_id, 1);
		lp.declare_variable_binary(var_id);

		for(uint k = 1; k <= p_num; k++)
		{
			var_id = vars.lookup(ILPDPCPMapper::TASK_ONLY_PROCESSOR, i, k);
			exp->add_term(var_id, -1);
			lp.declare_variable_binary(var_id);
		}
		lp.add_equality(exp, 0);
	}
}
//C20
void ILP_RTA_PFP_DPCP::constraint_20(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();

		for(uint k = 1; k <= p_num; k++)
		{
			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper::TASK_ONLY_PROCESSOR, i, k);
			exp->add_term(var_id, 1);

			var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
			exp->add_term(var_id, -0.5);

			var_id = vars.lookup(ILPDPCPMapper::APPLICATION_CORE, k);
			exp->add_term(var_id, -0.5);

			lp.add_inequality(exp, 0);
		}
	}
}
//C21
void ILP_RTA_PFP_DPCP::constraint_21(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_index();

		for(uint k = 1; k <= p_num; k++)
		{
			LinearExpression *exp = new LinearExpression();
			uint var_id;

			var_id = vars.lookup(ILPDPCPMapper::TASK_ONLY_PROCESSOR, i, k);
			exp->add_term(var_id, -1);

			var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
			exp->add_term(var_id, 0.5);

			var_id = vars.lookup(ILPDPCPMapper::APPLICATION_CORE, k);
			exp->add_term(var_id, 0.5);

			lp.add_inequality(exp, 0.5);
		}
	}
}
//C22
void ILP_RTA_PFP_DPCP::constraint_22(LinearProgram& lp, ILPDPCPMapper& vars)
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

			var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_C_TASK, i, x);
			exp->add_term(var_id, -1);

			long NC_WCET_x = tx->get_wcet_non_critical_sections();

			var_id = vars.lookup(ILPDPCPMapper::TBT_PREEMPT_NUM, i, x);
			exp->add_term(var_id, NC_WCET_x);

			uint bound = ceiling(di, px);

			long WCET_x = tx->get_wcet();

			var_id = vars.lookup(ILPDPCPMapper::TASK_ONLY, i);
			exp->add_term(var_id, -1*bound*WCET_x);

			lp.add_inequality(exp, bound*WCET_x);
		}
	}
}

//C22-1
void ILP_RTA_PFP_DPCP::constraint_22_1(LinearProgram& lp, ILPDPCPMapper& vars)
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

			var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_C_TASK, i, x);
			exp->add_term(var_id, -1);

			long NC_WCET_x = tx->get_wcet_non_critical_sections();

			var_id = vars.lookup(ILPDPCPMapper::TBT_PREEMPT_NUM, i, x);
			exp->add_term(var_id, NC_WCET_x);

			uint bound = ceiling(di, px);
		
			foreach(tx->get_requests(), request_v)
			{
				uint v = request_v->get_resource_id();
				int N_x_v = request_v->get_num_requests();
				long L_x_v = request_v->get_max_length();

				var_id = vars.lookup(ILPDPCPMapper::RBT_PREEMPT_NUM, i, x, v);
				exp->add_term(var_id, N_x_v*L_x_v);
			}

			long WCET_x = tx->get_wcet();

			var_id = vars.lookup(ILPDPCPMapper::TASK_ONLY, i);
			exp->add_term(var_id, -1*bound*WCET_x);

			lp.add_inequality(exp, 0);
		}
	}
}

void ILP_RTA_PFP_DPCP::constraint_23(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();
	for(uint k = 1; k <= p_num; k++)
	{
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		foreach(tasks.get_tasks(), ti)
		{
			uint i = ti->get_id();
			fraction_t utilization = ti->get_utilization();
			
			var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
			exp->add_term(var_id, utilization.get_d());
		}

		lp.add_inequality(exp, 1);
	}
}

bool ILP_RTA_PFP_DPCP::alloc_schedulable() {}































