// rr_scheduler.cpp
#include "rr_scheduler.h"
#include <algorithm>

RR_Scheduler::RR_Scheduler(int quantum)
    : Scheduler(
          [](auto a, auto b){
              return a->getArrivalTime() > b->getArrivalTime();
          },
          true,  // RR is always preemptive
          "Round Robin"),
    timeQuantum(quantum)
{}

void RR_Scheduler::updateReadyQueue() {
    // For RR, we use the roundRobinQueue directly
    // This is just here to fulfill the interface
}

std::shared_ptr<Process> RR_Scheduler::selectNextProcess() {
    if (roundRobinQueue.empty()) return nullptr;

    auto next = roundRobinQueue.front();
    roundRobinQueue.pop();
    return next;
}

void RR_Scheduler::addProcess(std::shared_ptr<Process> p) {
    allProcesses.push_back(p);

    // Add to RR queue if it has already arrived
    if (p->getArrivalTime() <= timeCounter) {
        roundRobinQueue.push(p);
    }
}

void RR_Scheduler::deleteProcess(int pid) {
    // Remove from allProcesses
    allProcesses.erase(
        std::remove_if(allProcesses.begin(), allProcesses.end(),
                       [pid](auto& p){ return p->getPid() == pid; }),
        allProcesses.end()
        );

    // Remove from currentProcess if it's the current one
    if (currentProcess && currentProcess->getPid() == pid) {
        currentProcess = nullptr;
        quantumCounter = 0;
    }

    // Rebuild RR queue
    std::queue<std::shared_ptr<Process>> tempQueue;
    while (!roundRobinQueue.empty()) {
        auto p = roundRobinQueue.front();
        roundRobinQueue.pop();

        if (p->getPid() != pid) {
            tempQueue.push(p);
        }
    }

    roundRobinQueue = std::move(tempQueue);
}

void RR_Scheduler::updateProcess(std::shared_ptr<Process> p) {
    // Find and replace in allProcesses
    for (auto& proc : allProcesses) {
        if (proc->getPid() == p->getPid()) {
            proc = p;
            break;
        }
    }

    // Update currentProcess if it's the one being updated
    if (currentProcess && currentProcess->getPid() == p->getPid()) {
        currentProcess = p;
    }

    // Rebuild RR queue to reflect updated process
    std::queue<std::shared_ptr<Process>> tempQueue;
    while (!roundRobinQueue.empty()) {
        auto proc = roundRobinQueue.front();
        roundRobinQueue.pop();

        if (proc->getPid() == p->getPid()) {
            tempQueue.push(p);
        } else {
            tempQueue.push(proc);
        }
    }

    roundRobinQueue = std::move(tempQueue);
}

void RR_Scheduler::runStatic(int runUntilTime) {
    // Reset state for a fresh simulation
    timeCounter = 0;

    // Clear the queue and reset the current process
    std::queue<std::shared_ptr<Process>> emptyQueue;
    roundRobinQueue = emptyQueue;
    currentProcess = nullptr;
    quantumCounter = 0;

    // Reset process states
    for (auto& p : allProcesses) {
        p->setIsComplete(false);
        p->setRemainingTime(p->getBurstTime());
        p->setStartTime(-1);
        p->setCompletionTime(-1);
        p->setTurnaroundTime(0);
        p->setWaitingTime(0);
    }

    // Sort processes by arrival time for consistent processing
    std::sort(allProcesses.begin(), allProcesses.end(),
              [](const auto& a, const auto& b) { return a->getArrivalTime() < b->getArrivalTime(); });

    // Add processes that arrive at time 0
    for (auto& p : allProcesses) {
        if (p->getArrivalTime() == 0) {
            roundRobinQueue.push(p);
        }
    }

    bool isLiveMode = (runUntilTime < 0);
    std::vector<std::shared_ptr<Process>> completedProcesses;

    while (!allProcessesComplete()) {
        // Handle CPU idle time when no process is ready
        if (roundRobinQueue.empty() && !currentProcess) {
            // Find the next arrival time
            int nextArrival = findNextArrivalTime();
            if (nextArrival == INT_MAX) break; // No more processes will arrive

            // Jump to next arrival
            timeCounter = nextArrival;

            // Add newly arrived processes to the queue
            for (auto& p : allProcesses) {
                if (!p->getIsComplete() && p->getArrivalTime() <= timeCounter) {
                    bool alreadyInQueue = false;
                    // Check if the process is already in the queue
                    std::queue<std::shared_ptr<Process>> tempQueue = roundRobinQueue;
                    while (!tempQueue.empty()) {
                        if (tempQueue.front()->getPid() == p->getPid()) {
                            alreadyInQueue = true;
                            break;
                        }
                        tempQueue.pop();
                    }

                    if (!alreadyInQueue) {
                        roundRobinQueue.push(p);
                    }
                }
            }
            continue;
        }

        // Get next process if needed
        if (!currentProcess) {
            currentProcess = roundRobinQueue.front();
            roundRobinQueue.pop();

            // Set start time if first execution
            if (currentProcess->getStartTime() < 0) {
                currentProcess->setStartTime(timeCounter);
            }

            quantumCounter = 0;
        }

        // Execute process for one time unit
        int executeTime = std::min(timeQuantum, currentProcess->getRemainingTime());

        // Check for new arrivals during this time slice
        int endTime = timeCounter + executeTime;
        std::vector<std::shared_ptr<Process>> newArrivals;

        for (auto& p : allProcesses) {
            if (!p->getIsComplete() && p->getArrivalTime() > timeCounter && p->getArrivalTime() <= endTime) {
                newArrivals.push_back(p);
            }
        }

        // Sort new arrivals by arrival time
        std::sort(newArrivals.begin(), newArrivals.end(),
                  [](const auto& a, const auto& b) { return a->getArrivalTime() < b->getArrivalTime(); });

        // Process executes until quantum or completion
        currentProcess->setRemainingTime(currentProcess->getRemainingTime() - executeTime);
        timeCounter += executeTime;

        // Add new arrivals to queue
        for (auto& p : newArrivals) {
            roundRobinQueue.push(p);
        }

        // Check if current process completed
        if (currentProcess->getRemainingTime() <= 0) {
            // Process completed
            currentProcess->setCompletionTime(timeCounter);
            currentProcess->setIsComplete(true);
            currentProcess->setTurnaroundTime(currentProcess->getCompletionTime() - currentProcess->getArrivalTime());
            currentProcess->setWaitingTime(currentProcess->getTurnaroundTime() - currentProcess->getBurstTime());
            completedProcesses.push_back(currentProcess);
            currentProcess = nullptr;
        } else {
            // Process used its time quantum, put it back in queue
            roundRobinQueue.push(currentProcess);
            currentProcess = nullptr;
        }

        // Check if we've reached the time limit in non-live mode
        if (!isLiveMode && timeCounter >= runUntilTime) {
            break;
        }
    }

    calculateAvgWaitingTime();
    calculateAvgTurnaroundTime();
}

bool RR_Scheduler::runOneStep() {
   this->updateReadyQueue();
    if (allProcessesComplete()) {
        return true;
    }

    // 1. Add arriving processes to the queue
    for (auto& p : allProcesses) {
        if (p->getArrivalTime() == timeCounter && !p->getIsComplete()) {
            bool inQueue = false;
            std::queue<std::shared_ptr<Process>> temp = roundRobinQueue;
            while (!temp.empty()) {
                if (temp.front()->getPid() == p->getPid()) {
                    inQueue = true;
                    break;
                }
                temp.pop();
            }
            if (!inQueue) {
                roundRobinQueue.push(p);
            }
        }
    }

    // 2. Check process state changes

    // If we had a process that completed in previous step, we need to select a new one
    // But don't clear currentProcess yet - we need it for display
    bool needNewProcess = false;

    if (currentProcess) {
        if (currentProcess->getRemainingTime() <= 0) {
            // Process is completed (from previous step)
            // Don't nullify it yet - just note we need a new one
            needNewProcess = true;
        } else if (quantumCounter >= timeQuantum) {
            // Process used up its quantum
            roundRobinQueue.push(currentProcess);
            needNewProcess = true;
        }
    } else {
        // No current process, need a new one if available
        needNewProcess = true;
    }

    // Select new process if needed
    std::shared_ptr<Process> nextProcess = nullptr;
    if (needNewProcess && !roundRobinQueue.empty()) {
        nextProcess = roundRobinQueue.front();
        roundRobinQueue.pop();

        if (nextProcess->getStartTime() < 0) {
            nextProcess->setStartTime(timeCounter);
        }
    }

    // Advance time counter
    timeCounter++;

    // Execute process for this time step
    if (needNewProcess) {
        // We're switching processes
        currentProcess = nextProcess;
        quantumCounter = 0;
    }

    // Execute the current process for this time unit
    if (currentProcess) {
        currentProcess->setRemainingTime(currentProcess->getRemainingTime() - 1);
        quantumCounter++;

        // Check if the process just finished with this time step
        if (currentProcess->getRemainingTime() <= 0) {
            currentProcess->setCompletionTime(timeCounter);
            currentProcess->setIsComplete(true);
            currentProcess->setTurnaroundTime(currentProcess->getCompletionTime() - currentProcess->getArrivalTime());
            currentProcess->setWaitingTime(currentProcess->getTurnaroundTime() - currentProcess->getBurstTime());
            // We keep the reference to currentProcess for display purposes
        }
    }

    // Calculate metrics
    calculateAvgWaitingTime();
    calculateAvgTurnaroundTime();

    return allProcessesComplete();
}

bool RR_Scheduler::allProcessesComplete() const {
    for (const auto& p : allProcesses) {
        if (!p->getIsComplete())
            return false;
    }
    return true;
}

int RR_Scheduler::findNextArrivalTime() const {
    int nextArrival = INT_MAX;
    for (const auto& p : allProcesses) {
        if (!p->getIsComplete() && p->getArrivalTime() > timeCounter) {
            nextArrival = std::min(nextArrival, p->getArrivalTime());
        }
    }
    return nextArrival;
}

int RR_Scheduler::getTimeQuantum() const {
    return timeQuantum;
}

void RR_Scheduler::setTimeQuantum(int quantum) {
    timeQuantum = quantum;
}
