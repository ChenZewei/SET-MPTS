// Copyright [2016] <Zewei Chen>
#ifndef INCLUDE_PROCESSORS_H_
#define INCLUDE_PROCESSORS_H_

#include <types.h>
#include <vector>
#include <set>

// using namespace std;
using std::vector;
using std::set;

class Task;
class TaskSet;
class DAG_Task;
class DAG_TaskSet;
class Resource;
class ResourceSet;
class Param;

class Processor {
 private:
    uint processor_id;
    double speedfactor;
    fraction_t utilization;
    fraction_t resource_utilization;
    fraction_t density;
    bool tryed_assign;
    // TaskQueue tQueue;
    // ResourceQueue rQueue;
    set<uint> tQueue;
    set<uint> rQueue;
    TaskSet* tasks;
    ResourceSet* resources;

 public:
    explicit Processor(uint id, double speedfactor = (1.0));
    ~Processor();
    uint get_processor_id() const;
    double get_speedfactor() const;
    fraction_t get_utilization();
    fraction_t get_density();
    fraction_t get_resource_utilization();
    bool get_tryed_assign() const;
    const set<uint>& get_taskqueue();
    bool add_task(uint t_id);
    bool remove_task(uint t_id);
    const set<uint>& get_resourcequeue();
    bool add_resource(uint r_id);
    bool remove_resource(uint r_id);
    void init();
    void update(const TaskSet* tasks, const ResourceSet* resources);
};

typedef vector<Processor> Processors;

class ProcessorSet {
 private:
    Processors processors;
 public:
    ProcessorSet();
    // for identical multiprocessor platform
    explicit ProcessorSet(Param param);
    uint get_processor_num() const;
    Processors& get_processors();
    void init();
    void sort_by_task_utilization(uint dir);
    void sort_by_resource_utilization(uint dir);
    void update(const TaskSet* tasks, const ResourceSet* resources);
};

#endif  // INCLUDE_PROCESSORS_H_
