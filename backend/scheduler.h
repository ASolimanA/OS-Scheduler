#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <queue>
#include <functional>
#include <memory>
#include "process.h"

class Scheduler {
protected:
    int timeCounter = 0;
    float avgWaitingTime    = 0.0f;
    float avgTurnaroundTime = 0.0f;
    bool preemptive         = false;
    std::string name;

    std::shared_ptr<Process> currentProcess = nullptr;

    // all processes ever added
    std::vector<std::shared_ptr<Process>> allProcesses;
    std::function<bool(std::shared_ptr<Process>, std::shared_ptr<Process>)> processComparator;

    // ready queue
    std::priority_queue<
        std::shared_ptr<Process>,
        std::vector<std::shared_ptr<Process>>,
        std::function<bool(std::shared_ptr<Process>, std::shared_ptr<Process>)>
        > readyQueue;

public:
    Scheduler(std::function<bool(std::shared_ptr<Process>, std::shared_ptr<Process>)> comp,
              bool isPreemptive, const std::string& schedulerName);
    virtual ~Scheduler() = default;

    // dynamic process management
    virtual void addProcess(std::shared_ptr<Process> p);
    virtual void deleteProcess(int pid);
    virtual void updateProcess(std::shared_ptr<Process> p);

    // queue maintenance
    virtual void updateReadyQueue();

    // scheduling
    // Static simulation (same as run but renamed)
    virtual void runStatic(int runUntilTime = -1);

    // Run one time step and return whether simulation is complete
    virtual bool runOneStep();

    // Add new processes while simulation is running
    virtual void addNewProcesses(const std::vector<std::shared_ptr<Process>>& newProcesses);

    // Check if simulation is complete
    virtual bool isSimulationComplete() const;
    virtual std::shared_ptr<Process> selectNextProcess() = 0;

    // metrics
    virtual void calculateAvgWaitingTime();
    virtual void calculateAvgTurnaroundTime();

    // getters
    float getAvgWaitingTime()    const;
    float getAvgTurnaroundTime() const;
    int   getCurrentTime()       const;
    std::shared_ptr<Process> getCurrentProcess() const;
    const std::string& getName() const;
};

#endif // SCHEDULER_H
