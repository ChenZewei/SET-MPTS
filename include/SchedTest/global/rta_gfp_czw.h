// Copyright [2017] <Zewei Chen>
// ------By Zewei Chen------
// Email:czwking1991@gmail.com
#ifndef INCLUDE_SCHEDTEST_GLOBAL_RTA_GFP_CZW_H_
#define INCLUDE_SCHEDTEST_GLOBAL_RTA_GFP_CZW_H_

/*
**
**
**
*/

#include <g_sched.h>
#include <processors.h>
#include <resources.h>
#include <tasks.h>

class RTA_GFP_CZW : public GlobalSched {
 private:
  TaskSet tasks;
  ProcessorSet processors;
  ResourceSet resources;

  ulong workload(const Task& task, ulong interval);
  ulong response_time(const Task& ti);
  ulong interference(const Task& tk, const Task& ti, ulong interval);

 public:
  RTA_GFP_CZW();
  RTA_GFP_CZW(TaskSet tasks, ProcessorSet processors, ResourceSet resources);
  bool is_schedulable();
};

#endif  // INCLUDE_SCHEDTEST_GLOBAL_RTA_GFP_CZW_H_
