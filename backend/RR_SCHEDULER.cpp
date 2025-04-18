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
    // Check if all processes are complete
    if (isSimulationComplete()) {
        return true;
    }

    // Check for new arrivals at the current time
    for (auto& p : allProcesses) {
        if (!p->getIsComplete() && p->getArrivalTime() == timeCounter) {
            roundRobinQueue.push(p);
        }
    }

    // Store previous process for tracking purposes
    std::shared_ptr<Process> previousProcess = currentProcess;
    bool processJustCompleted = false;

    // If no current process or current process's quantum expired or completed
    if (!currentProcess || quantumCounter >= timeQuantum || currentProcess->getRemainingTime() <= 0) {
        // If there was a current process and it still has work, put it back in queue
        if (currentProcess && currentProcess->getRemainingTime() > 0) {
            roundRobinQueue.push(currentProcess);
        }

        // If current process completed, mark it as such
        if (currentProcess && currentProcess->getRemainingTime() <= 0) {
            currentProcess->setCompletionTime(timeCounter);
            currentProcess->setIsComplete(true);
            currentProcess->setTurnaroundTime(currentProcess->getCompletionTime() - currentProcess->getArrivalTime());
            currentProcess->setWaitingTime(currentProcess->getTurnaroundTime() - currentProcess->getBurstTime());
            processJustCompleted = true;
            // Don't set currentProcess to nullptr yet for tracking purposes
        }

        // Get next process from queue
        if (!roundRobinQueue.empty()) {
            // Now we can set currentProcess to nullptr before getting next process
            if (processJustCompleted) {
                currentProcess = nullptr;
            }

            currentProcess = roundRobinQueue.front();
            roundRobinQueue.pop();

            // Set start time if first execution
            if (currentProcess->getStartTime() < 0) {
                currentProcess->setStartTime(timeCounter);
            }

            quantumCounter = 0;
        } else if (processJustCompleted) {
            // If a process just completed and there's nothing in the queue, keep currentProcess for display
            // It will be handled in the next iteration
        }
    }

    // If we have a current process, execute it for one time unit
    if (currentProcess) {
        currentProcess->setRemainingTime(currentProcess->getRemainingTime() - 1);
        quantumCounter++;

        // Check if process completed after this time unit
        if (currentProcess->getRemainingTime() <= 0) {
            currentProcess->setCompletionTime(timeCounter + 1);
            currentProcess->setIsComplete(true);
            currentProcess->setTurnaroundTime(currentProcess->getCompletionTime() - currentProcess->getArrivalTime());
            currentProcess->setWaitingTime(currentProcess->getTurnaroundTime() - currentProcess->getBurstTime());
            // Keep currentProcess non-null until next iteration for tracking
        }
    }

    // Increase time counter
    timeCounter++;

    // Calculate metrics
    calculateAvgWaitingTime();
    calculateAvgTurnaroundTime();

    // Return whether simulation is complete
    return isSimulationComplete();
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
