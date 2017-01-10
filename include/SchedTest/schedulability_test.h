#ifndef SCHEDULABILITY_TEST_H
#define SCHEDULABILITY_TEST_H
#include "RMS.h"
#include "bcl.h"
#include "partitioned_sched.h"
#include "types.h"
#include "pfp_algorithms.h"
#include "rta_gfp.h"
#include "dag_gedf.h"
#include "lp_gfp.h"
#include "ro_pfp.h"
#include "ilp_spinlock.h"
#include "gedf_non_preempt.h"

bool is_schedulable(TaskSet& taskset, ProcessorSet& processorset, ResourceSet& resourceset, uint TEST_METHOD, uint TEST_TYPE, uint ITER_BLOCKING)
{
//cout<<"TEST METHOD:"<<TEST_METHOD<<endl;
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
		case LP_PFP://7
			taskset.RM_Order();
			schedulable = is_worst_fit_pfp_schedulable(taskset, processorset, resourceset, TEST_TYPE, ITER_BLOCKING);
			break;
		case LP_GFP://8
			taskset.RM_Order();
			schedulable = is_lp_gfp_schedulable(taskset, processorset, resourceset, TEST_TYPE, ITER_BLOCKING);
			break;
		case RO_PFP:
			schedulable = is_ro_pfp_schedulable(taskset, processorset, resourceset, TEST_TYPE, ITER_BLOCKING);
			break;
		case ILP_SPINLOCK:
			schedulable = is_ILP_SpinLock_schedulable(taskset, processorset, resourceset);
			break;
		case GEDF_NON_PREEMPT:
			taskset.RM_Order();
			schedulable = is_gedf_non_preempt_schedulable(taskset, processorset, resourceset);
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
