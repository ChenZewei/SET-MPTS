#ifndef LP_PIP_H
#define LP_PIP_H

#include <types.h>
#include <varmapper.h>

class Task;
class TaskSet;
class Request;
class Resource;
class ResourceSet;
class ProcessorSet;
class LinearExpression;
class LinearProgram;

/*
|________________|_____________________|______________________|______________________|______________________|
|                |                     |                      |                      |                      |
|(63-34)Reserved |(32-30) var type     |(29-20) Task          |(19-10) Resource      |(9-0) Request         |
|________________|_____________________|______________________|______________________|______________________|
*/
class PIPMapper: public VarMapperBase
{
	public:
		enum var_type
		{
			BLOCKING_DIRECT,//0x000
			BLOCKING_INDIRECT,//0x001
			BLOCKING_PREEMPT,//0x010
			BLOCKING_OTHER,//0x011
			INTERF_REGULAR,//0x100
			INTERF_CO_BOOSTING,//0x101
			INTERF_STALLING,//0x110
			INTERF_OTHER//0x111
		};
	
	private:	
		static uint64_t encode_request(uint64_t task_id, uint64_t res_id, uint64_t req_id, uint64_t type);
		static uint64_t get_type(uint64_t var);
		static uint64_t get_task(uint64_t var);
		static uint64_t get_res_id(uint64_t var);
		static uint64_t get_req_id(uint64_t var);
	public:
		PIPMapper(uint start_var = 0);
		uint lookup(uint task_id, uint res_id, uint req_id, var_type type);
		string key2str(uint64_t key, uint var) const;															
};

bool is_global_pip_schedulable(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources);

ulong get_response_time(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources);

void lp_pip_directed_blocking(Task& ti, Task& tx, TaskSet& tasks, PIPMapper& vars, LinearExpression *exp, double coef = 1);

void lp_pip_indirected_blocking(Task& ti, Task& tx, TaskSet& tasks, PIPMapper& vars, LinearExpression *exp, double coef = 1);

void lp_pip_preemption_blocking(Task& ti, Task& tx, TaskSet& tasks, PIPMapper& vars, LinearExpression *exp, double coef = 1);

void lp_pip_OD(Task& ti, TaskSet& tasks, ProcessorSet& processors, PIPMapper& vars, LinearExpression *exp, double coef = 1);

void lp_pip_objective(Task& ti, TaskSet& tasks, ProcessorSet& processors, LinearProgram& lp, PIPMapper& vars, LinearExpression *obj);

ulong workload_bound(Task& tx, ulong Ri);

void lp_pip_add_constraints(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars);

//Constraint 1 [Maolin 2015 RTSS]
void lp_pip_constraint_1(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars);

//Constraint 2 [Maolin 2015 RTSS]
void lp_pip_constraint_2(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars);

//Constraint 3 [Maolin 2015 RTSS]
void lp_pip_constraint_3(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars);

//Constraint 4 [Maolin 2015 RTSS]
void lp_pip_constraint_4(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars);

//Constraint 5 [Maolin 2015 RTSS]
void lp_pip_constraint_5(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars);

//Constraint 6 [Maolin 2015 RTSS]
void lp_pip_constraint_6(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars);

//Constraint 7 [Maolin 2015 RTSS]
void lp_pip_constraint_7(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars);

//Constraint 9 [Maolin 2015 RTSS]
void lp_pip_constraint_8(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars);

//Constraint 10 [Maolin 2015 RTSS]
void lp_pip_constraint_9(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, PIPMapper& vars);
#endif
