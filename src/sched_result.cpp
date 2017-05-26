#include "sched_result.h"
#include "iteration-helper.h"
#include "math-helper.h"

template <typename TaskModel>
int increase_order(TaskModel t1, TaskModel t2)
{
	return t1.utilization < t2.utilization;
}

SchedResult::SchedResult(string name)
{
	test_name = name;
}


string SchedResult::get_test_name()
{
	return test_name;
}

void SchedResult::insert_result(double utilization, uint e_time, uint s_time)
{
	bool exist = false;

	foreach(results, result)
	{
		if(abs(result->utilization - utilization) <= _EPS)
		{
			exist = true;
			result->exp_num += e_time;
			result->success_num += s_time;
			break;
		}
	}

	if(!exist)
	{
		Result temp;
		temp.utilization = utilization;
		temp.exp_num = e_time;
		temp.success_num = s_time;

		results.push_back(temp);

		sort(results.begin(), results.end(), increase_order<Result>);
	}
}

vector<Result>& SchedResult::get_results()
{
	return results;
}

Result SchedResult::get_result_by_utilization(double utilization)
{
	Result empty;
	empty.utilization = 0;
	empty.exp_num = 0;
	empty.success_num = 0;

	foreach(results, result)
	{
		if(abs(result->utilization - utilization) <= _EPS)
		{
			return *result;
		}
	}
	return empty;
}

/////////////////////////////////////////////////////////

SchedResultSet::SchedResultSet() {}

uint SchedResultSet::size() {return sched_result_set.size();}

vector<SchedResult>& SchedResultSet::get_sched_result_set() {return sched_result_set;}

SchedResult& SchedResultSet::get_sched_result(string test_name)
{
	foreach(sched_result_set, sched_result)
	{
		if(0 == strcmp(sched_result->get_test_name().data(), test_name.data()))
		{
			return (*sched_result);
		}
	}

	SchedResult new_element(test_name);

	sched_result_set.push_back(new_element);

	return sched_result_set[sched_result_set.size()-1];
}

