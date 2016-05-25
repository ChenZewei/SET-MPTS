#ifndef SCHED_SIM_H
#define SCHED_SIM_H

#include "tasks.h"
#include "processors.h"
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

typedef unsigned long simtime_t;

class SimJob: public Job
{
	private:
		const &Task task;
		ulong release;
		ulong deadline;
		ulong allocation;
		ulone seqnum;
	public:
		SimJob(uint id,	
			ulong cost, 
			const &Task task,
			ulong release,
			ulong allocation = 0,
			ulone seqnum = 0);

		const &Task get_task() const;
		ulong get_release() const;
		ulong get_deadline() const;
		ulong get_sequence() const;
		ulong get_remaining() const;
		void add_allocation(simtime_t time);
		bool is_complete() const;
		void init_next(simtime_t cost = 0, simtime_t interval = 0);
};

class SimProcessor: public Processor
{
	private:
		SimJob *current_job;
	public:
		SimProcessor();
		SimJob* get_current_job() const;
		void allocate(SimJob *new_job);
		void idle();
		bool is_idle();
		bool advance_time(simtime_t time);
};

typedef vector<SimProcessor> SimProcessors;

class SimProcessorSet
{
	private:
		SimProcessors simprocessors;
	public:
		uint get_processor_num() const;
};

template<typename Object,typename Time>
class Event
{
	private:
		Object *object;
		Time time;
	public:
		Object* object() const;
		const &Time time() const;
		bool operator<(const Event<Object,Time> &that) const;
		bool operator>(const Event<Object,Time> &that) const;
};

class EarliestDeadlineFirst {//EDF
	public:
		bool operator()(const SimJob* a, const SimJob* b);//判断job b是否需要执行操作
};

template <typename JobPriority, typename Processor>
class PreemptionOrderTemplate
{
	public:
		bool operator()(const Processor& a, const Processor& b);//返回更高的优先级工作
};

typedef priority_queue<Event<SimJob,simtime_t>,
			vector<Event<SimJob,simtime_t>>,
			greater<Event<SimJob,simtime_t>>
			> Events_Queue;



template <typename JobPriority>
class GlobalScheduling
{
	typedef priority_queue<SimJob*,
			vector<SimJob*>,
			JobPriority
			> Ready_Queue;
	private:
		Events_Queue events;
		Ready_Queue ready;
		SimProcessorSet &processorset;
		simtime_t  current_time;
		bool aborted;

	private:
		void advance_time(simtime_t time);
	public:
		GlobalScheduling(SimProcessorSet &processorset);
		~GlobalScheduling();
		SimJob* lowest_priority();
		simtime_t get_current_time() const;
		void abort() const;
		void simulate_until(simtime_t end_of_simulation);
};




#endif
