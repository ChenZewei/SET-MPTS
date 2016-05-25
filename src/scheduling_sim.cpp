#include "scheduling_sim.h"

/////////////////////////////SimJob///////////////////////////////

SimJob::SimJob(uint id,	
	ulong cost, 
	const &Task task,
	ulong release,
	ulong allocation = 0,
	ulone seqnum = 0): Job(id, cost)
{
	this->task = task;
	this->release = release;
	this->deadline = release + task.get_deadline();
	this->allocation = allocation;
	this->seqnum = seqnum;
}

const &Task SimJob::get_task() const
{
	return task;
}

ulong SimJob::get_release() const
{
	return release;
}
ulong SimJob::get_deadline() const
{
	return deadline;
}

ulong SimJob::get_sequence() const
{
	return seqnum;
}

ulong SimJob::get_remaining() const
{
	if(cost > allocation)
		return cost - allocation;
	else
		return 0;
}

void SimJob::add_allocation(simtime_t time)
{
	allocation += time;
}

bool SimJob::is_complete() const
{
	return cost <= allocation;
}

void SimJob::init_next(simtime_t cost, simtime_t interval);
{
	allocation = 0;
	if(0 != cost)
		this->cost = cost;
	release += task.get_period() + interval;
	deadline = release + task.get_deadline();
	seqno++;
}

/////////////////////////////SimProcessor///////////////////////////////

SimProcessor::SimProcessor(): Processor() {}

SimJob* SimProcessor::get_current_job() const
{
	return current_job;
}
void SimProcessor::allocate(SimJob *new_job)
{
	current_job = new_job;
}
void SimProcessor::idle()
{
	current_job = NULL;
}
bool SimProcessor::is_idle()
{
	return (NULL == current_job);
}
bool SimProcessor::advance_time(simtime_t time)
{
	if (current_job)
        {
            current_job->add_allocation(time);
            return current_job->is_complete();
        }
        else
            return false;
}

/////////////////////////////SimProcessorSet///////////////////////////////

uint SimProcessorSet::get_processor_num() const
{
	return simprocessors.size();
}

//////////////////////////////////Event////////////////////////////////////

Object* Event::object() const
{
	return object;
}
const &Time Event::time() const
{
	return time;
}
bool Event::operator<(const Event<Object,Time> &that) const
{
	return this->time() < that.time();
}

bool Event::operator>(const Event<Object,Time> &that) const
{
	return this->time() > that.time();
}

///////////////////////////EarliestDeadlineFirst/////////////////////////////

bool EarliestDeadlineFirst::operator()(const SimJob* a, const SimJob* b)//判断job b是否需要执行操作
{
if (a && b)
    return a->get_deadline() > b->get_deadline();
else if (b && !a)
    return true;
else
    return false;
}

///////////////////////////PreemptionOrderTemplate/////////////////////////////

bool PreemptionOrderTemplate::operator()(const Processor& a, const Processor& b)//返回更高的优先级工作
{
	JobPriority higher_prio;
	return higher_prio(a.get_scheduled(), b.get_scheduled());
}

/////////////////////////////GlobalScheduling///////////////////////////////

GlobalScheduling::GlobalScheduling(SimProcessorSet &processorset)
{
	current_time = 0;
	abort = false;
	this->processorset = processorset;
}

GlobalScheduling::~GlobalScheduling() {}

SimJob* GlobalScheduling::lowest_priority()
{

}

simtime_t GlobalScheduling::get_current_time() const
{
	return current_time;
}
void GlobalScheduling::abort() const
{
	abort = true;
}
void GlobalScheduling::simulate_until(simtime_t end_of_simulation)
{
	while (current_time <= end_of_simulation &&//进行到最近一个事件发生的时刻
               !aborted &&
               !events.empty()) 
	{
            simtime_t next = events.top().time();
            advance_time(next);
        }
}

void GlobalScheduling::advance_time(simtime_t time);
