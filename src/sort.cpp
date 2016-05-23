#include "sort.h"

int period_increase(Task t1, Task t2)
{
	return t1.get_period() < t2.get_period();
}
