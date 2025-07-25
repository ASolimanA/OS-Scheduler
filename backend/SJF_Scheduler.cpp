#include "sjf_scheduler.h"
#include <algorithm>

SJF_Scheduler::SJF_Scheduler(bool isPreemptive)
    : Scheduler(
          [](auto a, auto b){
              // If remaining times are equal, sort by arrival time
              if (a->getRemainingTime() == b->getRemainingTime())
                  return a->getArrivalTime() > b->getArrivalTime();
              return a->getRemainingTime() > b->getRemainingTime();
          },
          isPreemptive,
          isPreemptive ? "SJF (Preemptive)" : "SJF (Non-Preemptive)")
{}

std::shared_ptr<Process> SJF_Scheduler::selectNextProcess() {
    if (readyQueue.empty()) return nullptr;
    return readyQueue.top();
}


void SJF_Scheduler::runStatic(int runUntilTime) {
    // This is the same as the current run method
    bool isLiveMode = (runUntilTime < 0);
    std::shared_ptr<Process> currentProcess = nullptr;

    while (isLiveMode || timeCounter < runUntilTime) {
        updateReadyQueue();

        if (readyQueue.empty() && !currentProcess) {
            // No process available at current time
            if (allProcessesComplete())
                break;

            // Advance time to the next process arrival
            int nextArrival = findNextArrivalTime();
            if (nextArrival > timeCounter)
                timeCounter = nextArrival;

            continue;
        }

        // Select the next process to run
        if (!currentProcess || currentProcess->getRemainingTime() == 0 ||
            (preemptive && !readyQueue.empty() &&
             readyQueue.top()->getRemainingTime() < currentProcess->getRemainingTime())) {

            // If current process is preempted or completed, push it back to the queue if it's not completed
            if (currentProcess && currentProcess->getRemainingTime() > 0) {
                readyQueue.push(currentProcess);
            }

            currentProcess = selectNextProcess();
            readyQueue.pop();

            // Set start time if this is first execution
            if (currentProcess->getStartTime() < 0)
                currentProcess->setStartTime(timeCounter);
        }

        // Execute for 1 time unit (or until completion if non-preemptive)
        int executeTime = preemptive ? 1 : currentProcess->getRemainingTime();

        // Clamp execution time if runUntilTime is specified
        if (!isLiveMode && timeCounter + executeTime > runUntilTime) {
            executeTime = runUntilTime - timeCounter;
        }

        // Execute the process
        timeCounter += executeTime;
        int remaining = currentProcess->getRemainingTime() - executeTime;
        currentProcess->setRemainingTime(remaining);

        // Check if the process is completed
        if (remaining <= 0) {
            currentProcess->setCompletionTime(timeCounter);
            currentProcess->setIsComplete(true);

            // Calculate metrics
            currentProcess->setTurnaroundTime(currentProcess->getCompletionTime() - currentProcess->getArrivalTime());
            currentProcess->setWaitingTime(currentProcess->getTurnaroundTime() - currentProcess->getBurstTime());

            currentProcess = nullptr;
        }

        // Break if we've reached the run until time
        if (!isLiveMode && timeCounter >= runUntilTime)
            break;
    }

    calculateAvgWaitingTime();
    calculateAvgTurnaroundTime();
}

bool SJF_Scheduler::runOneStep() {
    // Update ready queue with any newly arrived processes
    updateReadyQueue();

    // If current process just completed in the last step
    if (currentProcess && currentProcess->getRemainingTime() <= 0) {
        // Process already marked as complete, but we kept the reference
        // for UI display purposes. Now we'll actually handle the completion.
        currentProcess = nullptr;
    }

    // Check preemption if necessary
    if (currentProcess && preemptive && !readyQueue.empty() &&
        readyQueue.top()->getRemainingTime() < currentProcess->getRemainingTime()) {
        // Preempt current process
        readyQueue.push(currentProcess);
        currentProcess = nullptr;
    }

    // If no current process, get the next one from the queue
    if (!currentProcess) {
        if (!readyQueue.empty()) {
            currentProcess = selectNextProcess();
            readyQueue.pop();

            // Set start time if this is first execution
            if (currentProcess->getStartTime() < 0)
                currentProcess->setStartTime(timeCounter);
        } else {
            // No process available, check if simulation is complete
            if (isSimulationComplete())
                return true;

            // Advance time by just 1 unit if no processes are ready
            timeCounter++;
            updateReadyQueue(); // Check again after advancing time

            // Return but don't mark as complete - this is an idle step
            return false;
        }
    }

    // Execute the current process for one time unit
    timeCounter++;
    int remaining = currentProcess->getRemainingTime() - 1;
    currentProcess->setRemainingTime(remaining);

    // Check if the process is completed
    if (remaining <= 0) {
        currentProcess->setCompletionTime(timeCounter);
        currentProcess->setIsComplete(true);

        // Calculate metrics
        currentProcess->setTurnaroundTime(currentProcess->getCompletionTime() - currentProcess->getArrivalTime());
        currentProcess->setWaitingTime(currentProcess->getTurnaroundTime() - currentProcess->getBurstTime());
    }

    // Calculate metrics after each step
    calculateAvgWaitingTime();
    calculateAvgTurnaroundTime();

    return isSimulationComplete();
}

bool SJF_Scheduler::allProcessesComplete() const {
    for (const auto& p : allProcesses) {
        if (!p->getIsComplete())
            return false;
    }
    return true;
}

int SJF_Scheduler::findNextArrivalTime() const {
    int nextArrival = INT_MAX;
    for (const auto& p : allProcesses) {
        if (!p->getIsComplete() && p->getArrivalTime() > timeCounter) {
            nextArrival = std::min(nextArrival, p->getArrivalTime());
        }
    }
    return nextArrival;
}
