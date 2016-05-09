#ifndef SCHEDULABILITY_TEST_H
#define SCHEDULABILITY_TEST_H

#define BCL_FTP 0
#define BCL_EDF 1
/*
bool is_schedulable(TaskSet taskset, ProcessorSet processorset, uint TEST_METHOD)
{
	bool schedulable;
	switch(TEST_METHOD)
	{
		case 0:
			schedulable = is_bcl_ftp_schedulable(taskset, processorset);
			break;
		case 1:
			schedulable = is_bcl_edf_schedulable(taskset, processorset);
			break;
		default:
			schedulable = is_bcl_ftp_schedulable(taskset, processorset);
			break;
	}
	return schedulable;
}
*/
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

#endif
