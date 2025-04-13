#ifndef SJF_SCHEDULER_H
#define SJF_SCHEDULER_H

#include <iostream>
#include "scheduler.h"
#include "process.h"

class SJF_Scheduler : public Scheduler {
private:
    struct CompareRemainingTime {
        bool operator()(const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) const {
            if (a->getRemainingTime() == b->getRemainingTime()) {
                return a->getArrivalTime() > b->getArrivalTime();
            }
            return a->getRemainingTime() > b->getRemainingTime();
        }
    };

public:
    SJF_Scheduler();

    virtual void run(int runUntilTime = -1) override;

    virtual std::shared_ptr<Process> selectNextProcess() override;
};



#endif /* SJF_SCHEDULER_H_ */
