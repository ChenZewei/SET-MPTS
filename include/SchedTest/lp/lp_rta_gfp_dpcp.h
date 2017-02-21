#ifndef LP_RTA_GFP_DPCP_H
#define LP_RTA_GFP_DPCP_H

/*
** LinearProgramming approach for global fix-priority scheduling under DPCP locking protocol
** 
** RTSS 2015 Maolin Yang et al. [Global Real-Time Semaphore Protocols: A Survey, Unified Analysis, and Comparison]
*/

#include "lp_sched.h"
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
class DPCPMapper: public VarMapperBase
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
		DPCPMapper(uint start_var = 0);
		uint lookup(uint task_id, uint res_id, uint req_id, var_type type);
		string key2str(uint64_t key, uint var) const;						
													
};

class LP_RTA_GFP_DPCP: public LPSched
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
		ulong get_max_wait_time(Task& ti, Request& rq);
		void lp_dpcp_objective(Task& ti, LinearProgram& lp, DPCPMapper& vars, LinearExpression *local_obj, LinearExpression *remote_obj);
//		void lp_dpcp_local_objective(Task& ti, LinearProgram& lp, DPCPMapper& vars);
//		void lp_dpcp_remote_objective(Task& ti, LinearProgram& lp, DPCPMapper& vars);
		void lp_dpcp_add_constraints(Task& ti, LinearProgram& lp, DPCPMapper& vars);
		//Constraint 1 [BrandenBurg 2013 RTAS] Xd(x,q,v) + Xi(x,q,v) + Xp(x,q,v) <= 1
		void lp_dpcp_constraint_1(Task& ti, LinearProgram& lp, DPCPMapper& vars);
		//Constraint 2 [BrandenBurg 2013 RTAS] for any remote resource lq and task tx except ti Xp(x,q,v) = 0
		void lp_dpcp_constraint_2(Task& ti, LinearProgram& lp, DPCPMapper& vars);
		//Constraint 3 [BrandenBurg 2013 RTAS]
		void lp_dpcp_constraint_3(Task& ti, LinearProgram& lp, DPCPMapper& vars);
		//Constraint 6 [BrandenBurg 2013 RTAS]
		void lp_dpcp_constraint_4(Task& ti, LinearProgram& lp, DPCPMapper& vars);
		//Constraint 7 [BrandenBurg 2013 RTAS]
		void lp_dpcp_constraint_5(Task& ti, LinearProgram& lp, DPCPMapper& vars);
		//Constraint 8 [BrandenBurg 2013 RTAS]
		void lp_dpcp_constraint_6(Task& ti, LinearProgram& lp, DPCPMapper& vars);

	public:
		LP_RTA_GFP_DPCP();
		LP_RTA_GFP_DPCP(TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources);
		~LP_RTA_GFP_DPCP();
		bool is_schedulable();
};

#endif
