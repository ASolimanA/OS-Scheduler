// scheduler.h
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <vector>
#include <string>
#include <functional>
#include "process.h"

// Forward declarations of comparator functors for different scheduling algorithms
struct FCFSComparator;
struct SJFComparator;
struct PriorityComparator;

class Scheduler {
protected:
    std::vector<Process*> ListOfProcesses;
    Process* currentProcess = nullptr;
    int time_counter = 0;
    float avgWaitingTime = 0.0f;
    float avgTurnaroundTime = 0.0f;
    bool preemptive = false;
    std::string schedulerAlgo;

    std::priority_queue<Process*, std::vector<Process*>, std::function<bool(Process*, Process*)>> readyQueue;

public:
    // Constructor takes a comparator function that determines scheduling priority
    Scheduler(std::function<bool(Process*, Process*)> comparator)
        : readyQueue(comparator)
    {}

    virtual ~Scheduler() {}

    virtual void addProcess(Process& p) = 0;
    virtual void deleteProcess(Process& p) = 0;
    virtual void updateProcess(Process& p) = 0;
    virtual void updateReadyQueue() = 0;
    virtual void run() = 0;                     // Strategy Pattern entry point
    virtual Process* selectNextProcess() = 0;   // choose from readyQueue
    virtual void calculateAvgWaitingTime() = 0;
    virtual void recalculateRemainingTime() = 0;
};

// Comparator for First-Come-First-Served (sorts by arrival time)
struct FCFSComparator {
    bool operator()(const Process* a, const Process* b) const {
        // Note: priority_queue puts the "largest" element at the top,
        // so we reverse the comparison to get the smallest arrival time at the top
        return a->getArrivalTime() > b->getArrivalTime();
    }
};

// Comparator for Shortest Job First (sorts by burst time)
struct SJFComparator {
    bool operator()(const Process* a, const Process* b) const {
        // If burst times are equal, use arrival time as a tiebreaker
        if (a->getRemainingTime() == b->getRemainingTime())
            return a->getArrivalTime() > b->getArrivalTime();
        return a->getRemainingTime() > b->getRemainingTime();
    }
};

// Comparator for Priority Scheduling (sorts by priority)
struct PriorityComparator {
    bool operator()(const Process* a, const Process* b) const {
        // Remember: smaller priority number means higher priority
        // If priorities are equal, use arrival time as a tiebreaker
        if (a->getPriority() == b->getPriority())
            return a->getArrivalTime() > b->getArrivalTime();
        return a->getPriority() > b->getPriority();
    }
};
