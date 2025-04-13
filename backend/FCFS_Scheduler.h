#ifndef FCFS_SCHEDULER_H
#define FCFS_SCHEDULER_H

#include <iostream>
#include "scheduler.h"
#include "Process.h"


class FCFS_Scheduler : public Scheduler {
public:
    // Comparator for sorting processes based on arrival time
    struct CompareArrivalTime {
        bool operator()(std::shared_ptr<Process> a, std::shared_ptr<Process> b) {
            if (a->getArrivalTime() == b->getArrivalTime()) {
                return a->getPid() > b->getPid(); // If arrival times are equal, use process ID to break ties
            }
            return a->getArrivalTime() > b->getArrivalTime(); // Otherwise, sort by arrival time
        }
    };
    // Constructor for FCFS_Scheduler
    FCFS_Scheduler();

    // The main function to run the scheduler
    virtual void run(int runUntilTime = -1) override;

    // Select the next process from the ready queue
    virtual std::shared_ptr<Process> selectNextProcess() override;
};

#endif // FCFS_SCHEDULER_H
