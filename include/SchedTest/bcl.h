// Copyright [2016] <Zewei Chen>
// ------By Zewei Chen------
// Email:czwking1991@gmail.com
#ifndef INCLUDE_SCHEDTEST_BCL_H_
#define INCLUDE_SCHEDTEST_BCL_H_

#include <processors.h>
#include <schedulability_test.h>
#include <stdarg.h>
#include <tasks.h>
#include <algorithm>  // for min

bool is_bcl_ftp_schedulable(TaskSet taskset, ProcessorSet processorset);
bool is_bcl_edf_schedulable(TaskSet taskset, ProcessorSet processorset);
/*
class BCL_Test: public Schedulability_Test
{
        public:
                BCL_Test(int n)
                {
                        test = 2*n;
                }
                bool is_Schedulable(TaskSet taskset, ProcessorSet processorset,
int METHOD)
                {
                        /*
                        va_list arg_ptr;
                        va_start (arg_ptr,n);
                        int algorithm = 0;
                        if(0 != n)
                                algorithm = va_arg(arg_ptr, int);
                        va_end(arg_ptr);

                        switch(METHOD)
                        {
                                default:
                                case 0:
                                        return is_bcl_ftp_schedulable(taskset,
processorset); case 1: return is_bcl_edf_schedulable(taskset, processorset);
                        }
                }
};
*/
bool is_bcl_ftp_schedulable(TaskSet taskset, ProcessorSet processorset) {
  int m = processorset.get_processor_num();
  for (int k = 0; k < taskset.get_taskset_size(); k++) {
    int sum = 0;
    int Ck = taskset.get_task_wcet(k);
    int Dk = taskset.get_task_deadline(k);
    int rhs = m * (Dk - Ck);
    for (int i = 0; i < taskset.get_taskset_size(); i++) {
      if (taskset.get_task_density(i) < taskset.get_task_density(k)) {
        int Ci = taskset.get_task_wcet(i);
        int Di = taskset.get_task_deadline(i);
        int Ti = taskset.get_task_period(i);
        int m = processorset.get_processor_num();
        int temp =
            floor((Dk + Di - Ci) / Ti) * Ci + min(Ci, (Dk + Di - Ci) % Ti);
        sum += min((Dk - Ck), temp);
      }
    }

    if (sum >= rhs) {
      return false;
    }
  }
  return true;
}

bool is_bcl_edf_schedulable(TaskSet taskset, ProcessorSet processorset) {
  int m = processorset.get_processor_num();
  for (int k = 0; k < taskset.get_taskset_size(); k++) {
    int sum = 0;
    int Ck = taskset.get_task_wcet(k);
    int Dk = taskset.get_task_deadline(k);
    int rhs = m * (Dk - Ck);
    for (int i = 0; i < taskset.get_taskset_size(); i++) {
      if (i != k) {
        int Ci = taskset.get_task_wcet(i);
        int Di = taskset.get_task_deadline(i);
        int Ti = taskset.get_task_period(i);
        int temp = floor(Dk / Ti) * Ci + min(Ci, Dk % Ti);
        sum += min((Dk - Ck), temp);
      }
    }
    if (sum >= rhs) {
      return false;
    }
  }
  return true;
}

#endif  // INCLUDE_SCHEDTEST_BCL_H_
