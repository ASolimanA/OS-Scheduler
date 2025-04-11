// scheduler.h
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <vector>
#include <string>
#include "process.h"    // your Process class
#include "readyqueue.h" // to be implemented 

class Scheduler {
protected:
    std::vector<Process*> ListOfProcesses;
    Process* currentProcess = nullptr;
    int time_counter = 0;
    ReadyQueue* readyQueue = nullptr;
    float avgWaitingTime = 0.0f;
    float avgTurnaroundTime = 0.0f;
    bool preemptive = false;
    std::string schedulerAlgo;

public:
    Scheduler(ReadyQueue* rq)
      : readyQueue(rq)
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

#endif // SCHEDULER_H