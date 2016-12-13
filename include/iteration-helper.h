#ifndef ITER_HELPER_H
#define ITER_HELPER_H

#define foreach_condition(tasks, condition) 		\
	for(uint i = 0; i < tasks.size(); i++)	\
	{					\
		if(condition)			\
			return false;		\
	}

#define foreach(collection, it)						\
	for (typeof(collection.begin()) it = (collection).begin();	\
	     it != (collection).end();					\
		     it++)

#define enumerate(collection, it, i)					\
	for (typeof(collection.begin()) it = ({i = 0; (collection).begin();}); \
	     it != (collection).end();					\
	     it++, i++)

#define apply_foreach(collection, fun, ...)				\
	foreach(collection, __apply_it_ ## collection) {		\
		fun(*__apply_it_ ## collection, ## __VA_ARGS__);	\
	}

#define map_ref(from, to, init, fun, ...)					\
	{								\
		(to).clear();						\
		(to).reserve((from).size());				\
		foreach(from, __map_ref_it) {				\
			(to).push_back(init());				\
			fun(*__map_ref_it, (to).back(),			\
			    ## __VA_ARGS__);				\
		}							\
	}


#endif
