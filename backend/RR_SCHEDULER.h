// rr_scheduler.h
#ifndef RR_SCHEDULER_H
#define RR_SCHEDULER_H

#include "scheduler.h"
#include <queue>
#include <climits>  // For INT_MAX

class RR_Scheduler : public Scheduler {
private:
    std::shared_ptr<Process> currentProcess = nullptr;
    int quantumCounter = 0; // To track how much of the quantum has been used
    int timeQuantum;
    std::queue<std::shared_ptr<Process>> roundRobinQueue;

    bool allProcessesComplete() const;
    int findNextArrivalTime() const;

public:
    RR_Scheduler(int quantum = 2);
    void runStatic(int runUntilTime = -1) override;
    bool runOneStep() override;
    std::shared_ptr<Process> selectNextProcess() override;

    // Queue maintenance is different for RR
    void updateReadyQueue() override;

    // Additional methods for RR
    void addProcess(std::shared_ptr<Process> p) override;
    void deleteProcess(int pid) override;
    void updateProcess(std::shared_ptr<Process> p) override;

    // Getter/setter for time quantum
    int getTimeQuantum() const;
    void setTimeQuantum(int quantum);
};

#endif // RR_SCHEDULER_H
