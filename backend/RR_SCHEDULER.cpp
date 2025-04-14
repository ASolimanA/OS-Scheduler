#include "RR_SCHEDULER.h"

RoundRobin_Scheduler::RoundRobin_Scheduler()
    : Scheduler([](std::shared_ptr<Process> a, std::shared_ptr<Process> b) {
        return a->getArrivalTime() > b->getArrivalTime();
      }, true, "Round Robin") {}

void RoundRobin_Scheduler::setQuantumTime(int time) {
        quantum_time = time;
    }

    int RoundRobin_Scheduler::getQuantumTime() {
        return quantum_time;
    }

    std::shared_ptr<Process> RoundRobin_Scheduler::selectNextProcess() override {
        if (readyQueue.empty()) return nullptr;

        auto next = readyQueue.top();
        readyQueue.pop();
        return next;
    }

    void RoundRobin_Scheduler::run(int runUntilTime = -1) override {
        std::vector<std::shared_ptr<Process>> processList;

        // Sort processes by arrival time
        for (auto& p : allProcesses) {
            processList.push_back(p);
        }

        std::sort(processList.begin(), processList.end(),
            [](const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) {
                return a->getArrivalTime() < b->getArrivalTime();
            });

        std::vector<std::shared_ptr<Process>> readyProcesses;
        std::size_t nextProcessIndex = 0;

        // Continue until all processes are complete
        while (true) {
            // Add newly arrived processes to ready queue
            while (nextProcessIndex < processList.size() &&
                   processList[nextProcessIndex]->getArrivalTime() <= timeCounter) {
                readyProcesses.push_back(processList[nextProcessIndex]);
                nextProcessIndex++;
            }

            if (readyProcesses.empty()) {
                if (nextProcessIndex >= processList.size()) {
                    break; // All processes completed
                }
                // Jump to next arrival time
                timeCounter = processList[nextProcessIndex]->getArrivalTime();
                continue;
            }

            // Get next process from ready queue (round robin)
            auto currentProcess = readyProcesses.front();
            readyProcesses.erase(readyProcesses.begin());

            // Set start time if not started yet
            if (currentProcess->getStartTime() == 0 && !currentProcess->getIsComplete()) {
                currentProcess->setStartTime(timeCounter);
            }

            // Execute for quantum time or until completion
            int executeTime = std::min(quantum_time, currentProcess->getRemainingTime());
            timeCounter += executeTime;
            currentProcess->setRemainingTime(currentProcess->getRemainingTime() - executeTime);

            // Add newly arrived processes during this quantum
            while (nextProcessIndex < processList.size() &&
                   processList[nextProcessIndex]->getArrivalTime() <= timeCounter) {
                readyProcesses.push_back(processList[nextProcessIndex]);
                nextProcessIndex++;
            }

            // Check if process is complete
            if (currentProcess->getRemainingTime() <= 0) {
                currentProcess->setIsComplete(true);
                currentProcess->setCompletionTime(timeCounter);
                currentProcess->setTurnaroundTime(currentProcess->getCompletionTime() -
                                                 currentProcess->getArrivalTime());
                currentProcess->setWaitingTime(currentProcess->getTurnaroundTime() -
                                             currentProcess->getBurstTime());
            } else {
                // Put back in ready queue if not complete
                readyProcesses.push_back(currentProcess);
            }
        }

        // Calculate metrics
        calculateAvgWaitingTime();
        calculateAvgTurnaroundTime();
    }