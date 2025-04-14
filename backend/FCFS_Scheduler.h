#ifndef FCFS_SCHEDULER_H
#define FCFS_SCHEDULER_H

#include "scheduler.h"
#include <climits>  // For INT_MAX

class FCFS_Scheduler : public Scheduler {
public:
    FCFS_Scheduler();
    void runStatic(int runUntilTime = -1) override;
    bool runOneStep() override;
    std::shared_ptr<Process> selectNextProcess() override;

private:
    std::shared_ptr<Process> currentProcess = nullptr;
    bool allProcessesComplete() const;
    int findNextArrivalTime() const;
};

#endif // FCFS_SCHEDULER_H
