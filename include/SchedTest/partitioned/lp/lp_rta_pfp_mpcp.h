#ifndef LP_RTA_PFP_MPCP_H
#define LP_RTA_PFP_MPCP_H

/*
** LinearProgramming approach for partitioned fix-priority scheduling under MPCP locking protocol
** 
** RTAS 2013 Bjorn B. Brandenburg [Improved Analysis and Evaluation of Real-Time Semaphore Protocols for P-FP 
** Scheduling]
*/

#include "p_sched.h"
#include "varmapper.h"
#include "tasks.h"
#include "processors.h"
#include "resources.h"

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
|(63-34)Reserved |(31-30) Blocking type|(29-20) Task          |(19-10) Resource      |(9-0) Request         |
|________________|_____________________|______________________|______________________|______________________|
*/
class MPCPMapper: public VarMapperBase
{
	public:
		enum var_type
		{
			BLOCKING_DIRECT,//0x000
			BLOCKING_INDIRECT,//0x001
			BLOCKING_PREEMPT,//0x010
			BLOCKING_OTHER,//0x011
		};
	private:
		
		static uint64_t encode_request(uint64_t task_id, uint64_t res_id, uint64_t req_id, uint64_t type);
		static uint64_t get_type(uint64_t var);
		static uint64_t get_task(uint64_t var);
		static uint64_t get_res_id(uint64_t var);
		static uint64_t get_req_id(uint64_t var);
	public:
		MPCPMapper(uint start_var = 0);
		uint lookup(uint task_id, uint res_id, uint req_id, var_type type);
		string key2str(uint64_t key, uint var) const;						
													
};

class LP_RTA_PFP_MPCP: public PartitionedSched
{
	private:
		TaskSet tasks;
		ProcessorSet processors;
		ResourceSet resources;

		ulong local_blocking(Task& task_i);
		ulong remote_blocking(Task& task_i);
		ulong total_blocking(Task& task_i);
		ulong interference(Task& task, ulong interval);
		ulong response_time(Task& task_i);
		bool alloc_schedulable();
		uint priority_ceiling(uint r_id, uint p_id);
		uint priority_ceiling(Task& ti);
		uint DD(Task& ti, Task& tx, uint r_id);
		uint PO(Task& ti, Task& tx);
		uint PO(Task& ti, Task& tx, uint r_id);
		ulong holding_time(Task& tx, uint r_id);
		ulong wait_time(Task& ti, uint r_id);

		void set_objective(Task& ti, LinearProgram& lp, MPCPMapper& vars, LinearExpression *local_obj, LinearExpression *remote_obj);

		void add_constraints(Task& ti, LinearProgram& lp, MPCPMapper& vars);
		
		//Constraint 15 [BrandenBurg 2013 RTAS Appendix-C]
		void constraint_1(Task& ti, LinearProgram& lp, MPCPMapper& vars);	
		//Constraint 16 [BrandenBurg 2013 RTAS Appendix-C]	
		void constraint_2(Task& ti, LinearProgram& lp, MPCPMapper& vars);	
		//Constraint 17 [BrandenBurg 2013 RTAS Appendix-C]	
		void constraint_3(Task& ti, LinearProgram& lp, MPCPMapper& vars);	
		//Constraint 18 [BrandenBurg 2013 RTAS Appendix-C]	
		void constraint_4(Task& ti, LinearProgram& lp, MPCPMapper& vars);
		//Constraint 19 [BrandenBurg 2013 RTAS Appendix-C]		
		void constraint_5(Task& ti, LinearProgram& lp, MPCPMapper& vars);	
		//Constraint 20 [BrandenBurg 2013 RTAS Appendix-C]	
		void constraint_6(Task& ti, LinearProgram& lp, MPCPMapper& vars);
	
	public:
		LP_RTA_PFP_MPCP();
		LP_RTA_PFP_MPCP(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
		~LP_RTA_PFP_MPCP();
		bool is_schedulable();
};

#endif
