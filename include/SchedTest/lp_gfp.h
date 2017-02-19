#ifndef LP_GFP_H
#define LP_GFP_H

#include "lp_pip.h"
#include "lp_fmlp.h"

bool is_lp_gfp_schedulable(TaskSet& taskset, ProcessorSet& processorset, ResourceSet& resourceset, uint TEST_TYPE, uint ITER_BLOCKING)
{
	bool schedulable;
	switch(TEST_TYPE)
	{
		case 0://PIP
			schedulable = is_global_pip_schedulable(taskset, processorset, resourceset);
			break;
		case 1://FMLP
			schedulable = is_global_fmlp_schedulable(taskset, processorset, resourceset);
		default:
			schedulable = is_global_pip_schedulable(taskset, processorset, resourceset);
	}
	return schedulable;
}

#endif
