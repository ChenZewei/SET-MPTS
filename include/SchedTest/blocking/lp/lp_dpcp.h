#ifndef LP_DPCP_H
#define LP_DPCP_H

class Task;
class TaskSet;
class Resource;
class ResourceSet;
class ProcessorSet;
class VarMapper;
class LinearExpression;
class LinearProgram;

void lp_dpcp_objective(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars, LinearExpression *local_obj, LinearExpression *remote_obj);

void lp_dpcp_local_objective(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars);

void lp_dpcp_remote_objective(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars);

void lp_dpcp_add_constraints(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, VarMapper& vars);

//Constraint 1 [BrandenBurg 2013 RTAS] Xd(x,q,v) + Xi(x,q,v) + Xp(x,q,v) <= 1
void lp_dpcp_constraint_1(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars);

//Constraint 2 [BrandenBurg 2013 RTAS] for any remote resource lq and task tx except ti Xp(x,q,v) = 0
void lp_dpcp_constraint_2(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars);

//Constraint 3 [BrandenBurg 2013 RTAS]
void lp_dpcp_constraint_3(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars);

//Constraint 6 [BrandenBurg 2013 RTAS]
void lp_dpcp_constraint_4(Task& ti, TaskSet& tasks, ResourceSet& resources, LinearProgram& lp, VarMapper& vars);

//Constraint 7 [BrandenBurg 2013 RTAS]
void lp_dpcp_constraint_5(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, VarMapper& vars);

//Constraint 8 [BrandenBurg 2013 RTAS]
void lp_dpcp_constraint_6(Task& ti, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources, LinearProgram& lp, VarMapper& vars);

#endif
