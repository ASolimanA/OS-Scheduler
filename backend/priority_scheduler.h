#ifndef PRIORITY_SCHEDULER_H
#define PRIORITY_SCHEDULER_H

#include "scheduler.h"
#include <climits>  // For INT_MAX

class Priority_Scheduler : public Scheduler {
public:
    Priority_Scheduler(bool isPreemptive = false);
    void runStatic(int runUntilTime = -1) override;
    bool runOneStep() override;
    std::shared_ptr<Process> selectNextProcess() override;

private:
    bool allProcessesComplete() const;
    int findNextArrivalTime() const;
};

#endif // PRIORITY_SCHEDULER_H
