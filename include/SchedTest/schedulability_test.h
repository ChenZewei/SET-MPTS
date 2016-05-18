#ifndef SCHEDULABILITY_TEST_H
#define SCHEDULABILITY_TEST_H
#include "RMS.h"
#include "bcl.h"
#include "partitioned_sched.h"
#include "bin_packing.h"
#include "types.h"


bool is_schedulable(TaskSet taskset, ProcessorSet processorset, uint TEST_METHOD)
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
