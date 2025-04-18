#include "scheduler.h"
#include <algorithm>

Scheduler::Scheduler(
    std::function<bool(std::shared_ptr<Process>, std::shared_ptr<Process>)> comp,
    bool isPreemptive,
    const std::string& schedulerName
    )
    : readyQueue(comp), processComparator(comp), preemptive(isPreemptive), name(schedulerName)
{}

// Add to scheduler.cpp
void Scheduler::runStatic(int runUntilTime) {
    // This will be overridden by each scheduler type
    // with the same implementation as their run() method
}

bool Scheduler::runOneStep() {
    // This will be overridden by each scheduler type
    return false;
}

void Scheduler::addNewProcesses(const std::vector<std::shared_ptr<Process>>& newProcesses) {
    for (const auto& p : newProcesses) {
        addProcess(p);
    }
}

bool Scheduler::isSimulationComplete() const {
    for (const auto& p : allProcesses) {
        if (!p->getIsComplete()) {
            return false;
        }
    }
    return true;
}

void Scheduler::addProcess(std::shared_ptr<Process> p) {
    allProcesses.push_back(p);
    readyQueue.push(p);
}

void Scheduler::deleteProcess(int pid) {
    // remove from allProcesses
    allProcesses.erase(
        std::remove_if(allProcesses.begin(), allProcesses.end(),
                       [pid](auto& p){ return p->getPid() == pid; }),
        allProcesses.end()
        );
    // rebuild readyQueue
    updateReadyQueue();
}

void Scheduler::updateProcess(std::shared_ptr<Process> p) {
    // find and replace in allProcesses
    for (auto& proc : allProcesses) {
        if (proc->getPid() == p->getPid()) {
            proc = p;
            break;
        }
    }
    updateReadyQueue();
}

void Scheduler::updateReadyQueue() {
    // Create a new queue with the saved comparator
    std::priority_queue<
        std::shared_ptr<Process>,
        std::vector<std::shared_ptr<Process>>,
        std::function<bool(std::shared_ptr<Process>, std::shared_ptr<Process>)>
        > newQueue(processComparator);

    // Add all eligible processes
    for (auto& p : allProcesses) {
        if (!p->getIsComplete() && p->getArrivalTime() <= timeCounter) {
            newQueue.push(p);
        }
    }

    // Replace the old queue
    readyQueue = std::move(newQueue);
}

void Scheduler::calculateAvgWaitingTime() {
    float sum = 0;
    for (auto& p : allProcesses) sum += p->getWaitingTime();
    avgWaitingTime = sum / allProcesses.size();
}

void Scheduler::calculateAvgTurnaroundTime() {
    float sum = 0;
    for (auto& p : allProcesses) sum += p->getTurnaroundTime();
    avgTurnaroundTime = sum / allProcesses.size();
}

float Scheduler::getAvgWaitingTime()    const { return avgWaitingTime; }
float Scheduler::getAvgTurnaroundTime() const { return avgTurnaroundTime; }
int   Scheduler::getCurrentTime()       const { return timeCounter; }
std::shared_ptr<Process> Scheduler::getCurrentProcess() const{return currentProcess;}

const std::string& Scheduler::getName() const { return name; }
