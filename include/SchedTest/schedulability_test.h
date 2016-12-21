#ifndef SCHEDULABILITY_TEST_H
#define SCHEDULABILITY_TEST_H
#include "RMS.h"
#include "bcl.h"
#include "partitioned_sched.h"
#include "types.h"
#include "pfp_algorithms.h"
#include "rta_gfp.h"
#include "dag_gedf.h"
#include "lp_dpcp.h"

bool is_schedulable(TaskSet& taskset, ProcessorSet& processorset, ResourceSet& resourceset, uint TEST_METHOD, uint TEST_TYPE, uint ITER_BLOCKING)
{
	bool schedulable;
	switch(TEST_METHOD)
	{
		case P_EDF:
			schedulable = is_Partitioned_EDF_Schedulable(taskset, processorset);
			break;		
		case BCL_FTP:
			schedulable = is_bcl_ftp_schedulable(taskset, processorset);
			break;
		case BCL_EDF:
			schedulable = is_bcl_edf_schedulable(taskset, processorset);
			break;
		case WF_DM:
			schedulable = is_worst_fit_dm_schedulable(taskset, processorset, resourceset, TEST_TYPE, ITER_BLOCKING);
			break;
		case WF_EDF:
			schedulable = is_worst_fit_edf_schedulable(taskset, processorset, resourceset, TEST_TYPE, ITER_BLOCKING);
			break;
		case RTA_GFP:
			schedulable = is_gfp_rta_schedulable(taskset, processorset, TEST_TYPE);
			break;
		case FF_DM:
			schedulable = is_first_fit_dm_schedulable(taskset, processorset, resourceset, TEST_TYPE, ITER_BLOCKING);
			break;
		case WF_LP_PFP://7
			schedulable = is_worst_fit_pfp_schedulable(taskset, processorset, resourceset, TEST_TYPE, ITER_BLOCKING);
			break;
		default:
			schedulable = is_bcl_ftp_schedulable(taskset, processorset);
			break;
	}
	return schedulable;
}

/*
class Schedulability_Test
{
	public:
		int test;
		Schedulability_Test()
		{
			test = 5;
		}
		virtual bool is_Schedulable(TaskSet taskset, ProcessorSet processorset, int n, ...);
};
*/
#endif
