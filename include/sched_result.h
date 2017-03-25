#ifndef SCHED_RESULT_H
#define SCHED_RESULT_H

#include "types.h"

template <typename TaskModel>
int increase_order(TaskModel t1, TaskModel t2);

class SchedResult
{
	private:
		string test_name;
		vector<Result> results;
	public:
		SchedResult(string name);
		string get_test_name();
		void insert_result(double utilization, uint e_time, uint s_time);
		vector<Result>& get_results();
		Result get_result_by_utilization(double utilization);
};


class SchedResultSet
{
	private:
		vector<SchedResult> sched_result_set;
	public:
		SchedResultSet();

		uint size();
		vector<SchedResult>& get_sched_result_set();
		SchedResult& get_sched_result(string test_name);
};



#endif
