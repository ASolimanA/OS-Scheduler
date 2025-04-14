#ifndef RR_SCHEDULER_H
#define RR_SCHEDULER_H

#include <iostream>
#include "scheduler.h"

class RoundRobin_Scheduler : public Scheduler {
    private:
        int quantum_time = 2; // Default value
    public:
        RoundRobin_Scheduler();
        void setQuantumTime(int time);
        int getQuantumTime();
        std::shared_ptr<Process> selectNextProcess() override;
        void run(int runUntilTime = -1) override;
    };

#endif // RR_SCHEDULER_H