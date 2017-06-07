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
	return (var >> 40) & (uint64_t) 0xf; //4 bits
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
		case ILPDPCPMapper::RESOURCE_ASSIGNMENT
			buf << "Q[";
			break;
		case ILPDPCPMapper::SAME_TASK_LOCALITY
			buf << "U[";
			break;
		case ILPDPCPMapper::SAME_RESOURCE_LOCALITY
			buf << "V[";
			break;
		case ILPDPCPMapper::SAME_TR_LOCALITY
			buf << "W[";
			break;
		case ILPDPCPMapper::APPLICATION_CORE
			buf << "AC[";
			break;
		case ILPDPCPMapper::UNIFORM_CORE
			buf << "UC[";
			break;
		case ILPDPCPMapper::RESPONSE_TIME
			buf << "R[";
			break;
		case ILPDPCPMapper::BLOCKING_TIME
			buf << "B[";
			break;
		case ILPDPCPMapper::REQUEST_BLOCKING_TIME
			buf << "b[";
			break;
		case ILPDPCPMapper::INTERFERENCE_TIME_R
			buf << "I_R[";
			break;
		case ILPDPCPMapper::INTERFERENCE_TIME_R_PROCESSOR
			buf << "I_R[";
			break;
		case ILPDPCPMapper::WORKLOAD_R
			buf << "W_R[";
			break;
		case ILPDPCPMapper::INTERFERENCE_TIME_C
			buf << "I_C[";
			break;
		case ILPDPCPMapper::WORKLOAD_C
			buf << "W_C[";
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

	ILP_SpinLock_set_objective(response_bound, vars);

	ILP_SpinLock_add_constraints(response_bound, vars);

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
		for(uint i = 0; i < tasks.get_tasks().size(); i++)
		{
cout<<"/===========Task "<<i<<"===========/"<<endl;
			LinearExpression *exp = new LinearExpression();
			double result;
			ILP_SpinLock_construct_exp(vars, exp, ILPDPCPMapper::RESPONSE_TIME, i);
			result = rb_solution->evaluate(*exp);
cout<<"response time of task "<<i<<":"<<result<<endl;

			ILP_SpinLock_construct_exp(vars, exp, ILPDPCPMapper::INTERFERENCE_TIME, i);
			result = rb_solution->evaluate(*exp);
cout<<"interference time of task "<<i<<":"<<result<<endl;

			ILP_SpinLock_construct_exp(vars, exp, ILPDPCPMapper::BLOCKING_TIME, i);
			result = rb_solution->evaluate(*exp);
cout<<"blocking time of task "<<i<<":"<<result<<endl;

			ILP_SpinLock_construct_exp(vars, exp, ILPDPCPMapper::SPIN_TIME, i);
			result = rb_solution->evaluate(*exp);
cout<<"spin time of task "<<i<<":"<<result<<endl;

cout<<"|===========Locailty Assignment===========|"<<endl;
			for(uint k = 1; k <= processors.get_processor_num(); k++)
			{
				ILP_SpinLock_construct_exp(vars, exp, ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
				result = rb_solution->evaluate(*exp);
cout<<"allocation of processor "<<k<<":"<<result<<endl;

			}
cout<<"|===========Priority Assignment===========|"<<endl;
			for(uint p = 1; p <= tasks.get_tasks().size(); p++)
			{
				ILP_SpinLock_construct_exp(vars, exp, ILPDPCPMapper::PRIORITY_ASSIGNMENT, i, p);
				result = rb_solution->evaluate(*exp);
cout<<"Priority assignment "<<p<<":"<<result<<endl;

			}

cout<<"wcet of task "<<i<<":"<<tasks.get_tasks()[i].get_wcet()<<endl;
cout<<"deadline of task "<<i<<":"<<tasks.get_tasks()[i].get_deadline()<<endl;
			delete exp;
		}
#endif
		
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
cout<<vars.var2str(var_id)<<":"<<endl;
#endif
}

void ILP_RTA_PFP_DPCP::set_objective(LinearProgram& lp, ILPDPCPMapper& vars)
{
	//any objective function is okay.
	//assert(0 < tasks.get_tasks().size());

	LinearExpression *obj = new LinearExpression();
	uint var_id;

	var_id = vars.lookup(ILPDPCPMapper::RESPONSE_TIME, 0, 0, 0);
	obj->add_term(var_id, 1);

	lp.set_objective(obj);
}

void ILP_RTA_PFP_DPCP::add_constraints(LinearProgram& lp, ILPDPCPMapper& vars)
{
	
}

////////////////////Expressions////////////////////
//C1
void ILP_RTA_PFP_DPCP::constraint_1(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();

	foreach(tasks.get_tasks(), ti)
	{
		LinearExpression *exp = new LinearExpression();
		uint  i = ti->get_id();
		for(uint k = 1; k <= p_num; k++)
		{
			uint var_id;
			var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
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

	foreach(tasks.get_tasks(), ti)
	{
		LinearExpression *exp = new LinearExpression();
		uint  i = ti->get_id();
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
		uint i = ti->get_id();
		foreach_task_except(tasks.get_tasks(), (*ti), tx)
		{
			uint x = tx->get_id();
			for(uint k = 1; k <= p_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, x, k);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(ILPDPCPMapper::SAME_LOCALITY, i, x);
				exp->sub_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				lp.add_inequality(exp, 1);
			}
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
		foreach_resource_except(resources.get_resources(), (*ru), rv)
		{
			uint v = rv->get_resource_id();
			for(uint k = 1; k <= p_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, u, k);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, v, k);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(ILPDPCPMapper::SAME_RESOURCE_LOCALITY, u, v);
				exp->sub_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				lp.add_inequality(exp, 1);
			}
		}	
	}
}
//C5
void ILP_RTA_PFP_DPCP::constraint_5(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();

	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		foreach(resources.get_resources(), ru)
		{
			uint u = ru->get_resource_id();
			for(uint k = 1; k <= p_num; k++)
			{
				LinearExpression *exp = new LinearExpression();
				uint var_id;

				var_id = vars.lookup(ILPDPCPMapper::LOCALITY_ASSIGNMENT, i, k);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(ILPDPCPMapper::RESOURCE_ASSIGNMENT, u, k);
				exp->add_term(var_id, 1);

				var_id = vars.lookup(ILPDPCPMapper::SAME_TR_LOCALITY, i, u);
				exp->sub_term(var_id, 1);
				lp.declare_variable_binary(var_id);

				lp.add_inequality(exp, 1);
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
	uint r_num = resources.get_resource_num();
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
			exp->add_term(var_id, -1.0/r_num);
		}


		lp.add_inequality(exp, 1);
	}
}
//C8
void ILP_RTA_PFP_DPCP::constraint_8(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();
	for(uint k = 1; k <= p_num; k++)
	{
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper::APPLICATION_CORE, k);
		exp->add_term(var_id, 1);

		var_id = vars.lookup(ILPDPCPMapper::UNIFORM_CORE, k);
		exp->add_term(var_id, 1);
		lp.declare_variable_binary(var_id);

		lp.add_equality(exp, 1);
	}
}
//C9
void ILP_RTA_PFP_DPCP::constraint_9(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		ulong wcet = ti->get_wcet();
		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper::RESPONSE_TIME, i);
		exp->add_term(var_id, 1);
		lp.declare_variable_integer(var_id);

		var_id = vars.lookup(ILPDPCPMapper::BLOCKING_TIME, i);
		exp->add_term(var_id, -1);
		lp.declare_variable_integer(var_id);

		var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_R, i);
		exp->add_term(var_id, -1);
		lp.declare_variable_integer(var_id);

		var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_C, i);
		exp->add_term(var_id, -1);
		lp.declare_variable_integer(var_id);

		lp.add_equality(exp, wcet);
	}
}
//C10
void ILP_RTA_PFP_DPCP::constraint_10(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		ulong deadline = ti->get_deadline();
		LinearExpression *exp = new LinearExpression();
		uint var_id;
		
		var_id = vars.lookup(ILPDPCPMapper::RESPONSE_TIME, i);
		exp->add_term(var_id, 1);
		
		lp.add_inequality(exp, deadline);
	}
}
//C11
void ILP_RTA_PFP_DPCP::constraint_11(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		LinearExpression *exp = new LinearExpression();
		uint var_id;
		
		var_id = vars.lookup(ILPDPCPMapper::BLOCKING_TIME, i);
		exp->add_term(var_id, 1);

		foreach(ti->get_requests(), request)
		{
			uint r = request->get_resource_id();

			uint N_i_r = request->get_num_requests();
		
			var_id = vars.lookup(ILPDPCPMapper::REQUEST_BLOCKING_TIME, i, r);
			exp->add_term(var_id, -1*N_i_r);
			lp.declare_variable_integer(var_id);
		}

		lp.add_equality(exp, 0);
	}
}
//C12
void ILP_RTA_PFP_DPCP::constraint_12(LinearProgram& lp, ILPDPCPMapper& vars)
{
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();
		
		foreach(ti->get_requests(), rr)
		{
			uint r = rr->get_resource_id();
			Resource& res_r = resources.get_resources()[r];

			foreach_lower_priority_task(tasks.get_tasks(), (*ti), tx)
			{
				uint x = tx->get_id();
				foreach(tx->get_requests(), ru)
				{
					uint u = ru->get_resource_id();
					Resource& res_u = resources.get_resources()[u];
					if(res_u.get_ceiling() > res_r.get_ceiling())
					{
						LinearExpression *exp = new LinearExpression();
						uint var_id;

						ulong L_x_u = ru->get_max_length();
		
						var_id = vars.lookup(ILPDPCPMapper::REQUEST_BLOCKING_TIME, i, r);
						exp->add_term(var_id, -1);
		
						var_id = vars.lookup(ILPDPCPMapper::SAME_TR_LOCALITY, i, r);
						exp->add_term(var_id, L_x_u);
		
						var_id = vars.lookup(ILPDPCPMapper::SAME_RESOURCE_LOCALITY, r, u);
						exp->add_term(var_id, L_x_u);

						lp.add_inequality(exp, L_x_u);
					}
				}
			}			
		}		
	}
}
//C13
void ILP_RTA_PFP_DPCP::constraint_13(LinearProgram& lp, ILPDPCPMapper& vars)
{
	uint p_num = processors.get_processor_num();
	foreach(tasks.get_tasks(), ti)
	{
		uint i = ti->get_id();

		LinearExpression *exp = new LinearExpression();
		uint var_id;

		var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_R, i);
		exp->add_term(var_id, 1);

		for(uint k = 1; k <= p_num; k++)
		{
			var_id = vars.lookup(ILPDPCPMapper::INTERFERENCE_TIME_R_PROCESSOR, i, k);
			exp->add_term(var_id, -1);
		}
		lp.add_equality(exp, 0);
	}
}
//C14
void ILP_RTA_PFP_DPCP::constraint_14(LinearProgram& lp, ILPDPCPMapper& vars);
//C15
void ILP_RTA_PFP_DPCP::constraint_15(LinearProgram& lp, ILPDPCPMapper& vars);
//C16
void ILP_RTA_PFP_DPCP::constraint_16(LinearProgram& lp, ILPDPCPMapper& vars);

bool ILP_RTA_PFP_DPCP::alloc_schedulable() {}































