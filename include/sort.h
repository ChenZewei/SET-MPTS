#ifndef SORT_H
#define SORT_H

#include "tasks.h"

#define PRIORITY 0

#define INCREASE 0
#define DECREASE 1

class Task;

int period_increase(Task t1, Task t2);
int utilization_decrease(Task t1, Task t2);

#endif
