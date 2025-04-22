#include "fcfs_scheduler.h"

FCFS_Scheduler::FCFS_Scheduler()
    : Scheduler(
          [](auto a, auto b){
              return a->getArrivalTime() > b->getArrivalTime();
          },
          false,
          "FCFS")
{}

std::shared_ptr<Process> FCFS_Scheduler::selectNextProcess() {
    if (readyQueue.empty()) return nullptr;
    return readyQueue.top();
}

void FCFS_Scheduler::runStatic(int runUntilTime) {
    // This is the same as the current run method
    bool isLiveMode = (runUntilTime < 0);

    while ((isLiveMode || timeCounter < runUntilTime)) {
        updateReadyQueue();

        if (readyQueue.empty()) {
            // No process available at current time
            if (allProcessesComplete())
                break;

            // Advance time to the next process arrival
            int nextArrival = findNextArrivalTime();
            if (nextArrival > timeCounter)
                timeCounter = nextArrival;

            continue;
        }

        auto p = selectNextProcess();
        readyQueue.pop();

        // Set start time if this is first execution
        if (p->getStartTime() < 0)
            p->setStartTime(timeCounter);

        // Run to completion (FCFS is non-preemptive)
        timeCounter += p->getRemainingTime();
        p->setCompletionTime(timeCounter);
        p->setRemainingTime(0);
        p->setIsComplete(true);

        // Calculate metrics
        p->setTurnaroundTime(p->getCompletionTime() - p->getArrivalTime());
        p->setWaitingTime(p->getTurnaroundTime() - p->getBurstTime());
    }

    calculateAvgWaitingTime();
    calculateAvgTurnaroundTime();
}
// For FCFS_Scheduler
bool FCFS_Scheduler::runOneStep() {
    // Update ready queue with any newly arrived processes
    updateReadyQueue();

    // Store info about process completion for tracking purposes
    bool processJustCompleted = false;

    // If current process just completed in the last step
    if (currentProcess && currentProcess->getRemainingTime() <= 0) {
        // Process already marked as complete, but we kept the reference
        // for UI display purposes. Now we'll actually handle the completion.
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

        // DON'T reset current process yet - keep it for display purposes
        // It will be reset at the beginning of the next step
        processJustCompleted = true;
    }

    // Calculate metrics after each step
    calculateAvgWaitingTime();
    calculateAvgTurnaroundTime();

    return isSimulationComplete();
}

bool FCFS_Scheduler::allProcessesComplete() const {
    for (const auto& p : allProcesses) {
        if (!p->getIsComplete())
            return false;
    }
    return true;
}

int FCFS_Scheduler::findNextArrivalTime() const {
    int nextArrival = INT_MAX;
    for (const auto& p : allProcesses) {
        if (!p->getIsComplete() && p->getArrivalTime() > timeCounter) {
            nextArrival = std::min(nextArrival, p->getArrivalTime());
        }
    }
    return nextArrival;
}
