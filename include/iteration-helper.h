#ifndef ITER_HELPER_H
#define ITER_HELPER_H

#define foreach(tasks, condition) 		\
	for(uint i = 0; i < tasks.size(); i++)	\
	{					\
		if(condition)			\
			return false;		\
	}

#endif
