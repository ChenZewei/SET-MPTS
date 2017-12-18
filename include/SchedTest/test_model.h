// Copyright [2017] <Zewei Chen>
// ------By Zewei Chen------
// Email:czwking1991@gmail.com
#ifndef INCLUDE_SCHEDTEST_TEST_MODEL_H_
#define INCLUDE_SCHEDTEST_TEST_MODEL_H_

#include <sched_test_base.h>

class TestModel : public SchedTestBase {
 public:
  TestModel();
  bool is_schedulable(TaskSet tasks, ProcessorSet processors,
                      ResourceSet resourcese, uint TEST_TYPE = 0,
                      uint ITER_BLOCKING = 0);
};

TestModel::TestModel()
    : SchedTestBase(false, 0, 0, 0, 0, "Testing", "Testing") {}

bool TestModel::is_schedulable(TaskSet tasks, ProcessorSet processors,
                               ResourceSet resourcese, uint TEST_TYPE = 0,
                               uint ITER_BLOCKING = 0) {
  return 1;
}

#endif  // INCLUDE_SCHEDTEST_TEST_MODEL_H_
