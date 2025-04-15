#ifndef SJF_SCHEDULER_H
#define SJF_SCHEDULER_H

#include "scheduler.h"
#include <climits> // For INT_MAX

class SJF_Scheduler : public Scheduler
{
public:
    SJF_Scheduler(bool isPreemptive = false);
    void runStatic(int runUntilTime = -1) override;
    bool runOneStep() override;
    std::shared_ptr<Process> selectNextProcess() override;

private:
    bool allProcessesComplete() const;
    int findNextArrivalTime() const;
    void sortReadyQueueByShortest();
};

#endif // SJF_SCHEDULER_H
