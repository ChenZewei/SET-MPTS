#ifndef ITER_HELPER_H
#define ITER_HELPER_H

#define foreach(tasks, condition) 		\
	for(int i; i < tasks.size(); i++)	\
	{					\
		if(condition)			\
			return false;		\
	}

#endif
