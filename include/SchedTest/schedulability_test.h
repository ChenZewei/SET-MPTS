#ifndef SCHEDULABILITY_TEST_H
#define SCHEDULABILITY_TEST_H
#include "RMS.h"
#include "bcl.h"
#include "partitioned_sched.h"
#include "bin_packing.h"
#include "types.h"


bool is_schedulable(TaskSet taskset, ProcessorSet processorset, ResourceSet resourceset, uint TEST_METHOD, uint TEST_TYPE, uint ITER_BLOCKING)
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
			schedulable = is_worst_fit_u_schedulable(taskset, processorset,	resourceset, processorset.get_processor_num(), TEST_TYPE, ITER_BLOCKING);
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
