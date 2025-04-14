#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include "process.h"
#include "fcfs_scheduler.h"
#include "sjf_scheduler.h"
#include "priority_scheduler.h"
#include "rr_scheduler.h"

// Helper function to print process details
void printProcessDetails(const std::vector<std::shared_ptr<Process>>& processes) {
    std::cout << "+-----------+-------------+------------+-----------------+------------------+------------------+\n";
    std::cout << "| Process   | Arrival     | Burst      | Completion      | Turnaround       | Waiting          |\n";
    std::cout << "| ID        | Time        | Time       | Time            | Time             | Time             |\n";
    std::cout << "+-----------+-------------+------------+-----------------+------------------+------------------+\n";

    for (const auto& p : processes) {
        std::cout << "| " << std::setw(9) << p->getPid()
        << " | " << std::setw(11) << p->getArrivalTime()
        << " | " << std::setw(10) << p->getBurstTime()
        << " | " << std::setw(15) << (p->getIsComplete() ? std::to_string(p->getCompletionTime()) : "-")
        << " | " << std::setw(16) << (p->getIsComplete() ? std::to_string(p->getTurnaroundTime()) : "-")
        << " | " << std::setw(16) << (p->getIsComplete() ? std::to_string(p->getWaitingTime()) : "-")
        << " |\n";
    }

    std::cout << "+-----------+-------------+------------+-----------------+------------------+------------------+\n";
}

// Function to display a simple Gantt chart for the current time step
void printGanttChart(int time, const std::shared_ptr<Process>& currentProcess) {
    std::cout << "Time " << time << ": ";

    if (currentProcess) {
        std::cout << "[P" << currentProcess->getPid() << " Running]";
        std::cout << " (Remaining: " << currentProcess->getRemainingTime() << ")";
    } else {
        std::cout << "[IDLE]";
    }

    std::cout << std::endl;
}

// Function to print ready queue (approximate visualization since we can't see inside priority queue directly)
void printReadyQueue(const std::vector<std::shared_ptr<Process>>& processes, int currentTime) {
    std::cout << "Ready Queue: ";
    bool first = true;

    for (const auto& p : processes) {
        if (!p->getIsComplete() && p->getArrivalTime() <= currentTime && p->getRemainingTime() > 0) {
            // Skip the currently running process (this is an approximation)
            if (first) {
                first = false;
            } else {
                std::cout << " → ";
            }
            std::cout << "P" << p->getPid();
        }
    }

    if (first) { // No processes in ready queue
        std::cout << "Empty";
    }

    std::cout << std::endl;
}

// Function to create a set of test processes
std::vector<std::shared_ptr<Process>> createTestProcesses() {
    std::vector<std::shared_ptr<Process>> processes;

    // Process(ID, Arrival Time, Burst Time, Priority)
    processes.push_back(std::make_shared<Process>(1, 0, 5, 3));  // P1
    processes.push_back(std::make_shared<Process>(2, 2, 3, 1));  // P2
    processes.push_back(std::make_shared<Process>(3, 3, 3, 3));  // P3
    processes.push_back(std::make_shared<Process>(4, 4, 1, 4));  // P4
    processes.push_back(std::make_shared<Process>(5, 1, 4, 2));  // P5

    return processes;
}

// Additional processes to add during simulation
std::vector<std::shared_ptr<Process>> createAdditionalProcesses() {
    std::vector<std::shared_ptr<Process>> processes;

    // Process(ID, Arrival Time, Burst Time, Priority)
    processes.push_back(std::make_shared<Process>(6, 7, 2, 2));  // P6
    processes.push_back(std::make_shared<Process>(7, 8, 4, 1));  // P7

    return processes;
}

// Function to reset processes to initial state
void resetProcesses(std::vector<std::shared_ptr<Process>>& processes) {
    for (auto& p : processes) {
        p->setRemainingTime(p->getBurstTime());
        p->setStartTime(-1);
        p->setCompletionTime(-1);
        p->setWaitingTime(0);
        p->setTurnaroundTime(0);
        p->setIsComplete(false);
    }
}

// Function to test a scheduler using runStatic
template<typename T>
void testStaticScheduler(const std::string& name, std::vector<std::shared_ptr<Process>>& processes,
                         T& scheduler, bool isPreemptive = false) {
    std::cout << "\n======= Testing " << name << " with runStatic";
    if (isPreemptive) {
        std::cout << " (Preemptive)";
    }
    std::cout << " =======\n";

    // Reset processes for a fresh test
    resetProcesses(processes);

    // Add processes to scheduler
    for (auto& p : processes) {
        scheduler.addProcess(p);
    }

    // Run the scheduler until completion
    scheduler.runStatic();

    // Print results
    std::cout << "Process Execution Details:\n";
    printProcessDetails(processes);

    std::cout << "Average Waiting Time: " << scheduler.getAvgWaitingTime() << std::endl;
    std::cout << "Average Turnaround Time: " << scheduler.getAvgTurnaroundTime() << std::endl;
    std::cout << "=================================\n";
}

// Function to test a scheduler using runOneStep
template<typename T>
void testStepByStepScheduler(const std::string& name, std::vector<std::shared_ptr<Process>>& processes,
                             T& scheduler, bool isPreemptive = false) {
    std::cout << "\n======= Testing " << name << " with runOneStep";
    if (isPreemptive) {
        std::cout << " (Preemptive)";
    }
    std::cout << " =======\n";

    // Reset processes for a fresh test
    resetProcesses(processes);

    // Add processes to scheduler
    for (auto& p : processes) {
        scheduler.addProcess(p);
    }

    // Prepare additional processes
    auto additionalProcesses = createAdditionalProcesses();
    bool additionalAdded = false;

    std::cout << "Initial Process Information:\n";
    printProcessDetails(processes);
    std::cout << std::endl;

    // Track currently running process (for display only)
    std::shared_ptr<Process> currentRunningProcess = nullptr;
    for (auto& p : processes) {
        if (p->getArrivalTime() == 0) {
            currentRunningProcess = p;
            break;
        }
    }

    // Run step by step
    bool isComplete = false;
    int stepCount = 0;

    while (!isComplete && stepCount < 30) { // Prevent infinite loop with a maximum step count
        std::cout << "----------------------------------------\n";

        // // Add additional processes at time step 6
        // if (scheduler.getCurrentTime() == 6 && !additionalAdded) {
        //     std::cout << ">>> Adding new processes at time 6:\n";
        //     printProcessDetails(additionalProcesses);
        //     scheduler.addNewProcesses(additionalProcesses);
        //     additionalAdded = true;
        // }

        // Print the current state before the step
        std::cout << "Before step " << stepCount << " (Time " << scheduler.getCurrentTime() << "):\n";
        printGanttChart(scheduler.getCurrentTime(), currentRunningProcess);
        printReadyQueue(processes, scheduler.getCurrentTime());

        // Execute one step
        isComplete = scheduler.runOneStep();
        stepCount++;

        // Find currently running process (approximate - just for visualization)
        currentRunningProcess = nullptr;
        for (auto& p : processes) {
            if (!p->getIsComplete() && p->getRemainingTime() > 0 && p->getStartTime() >= 0) {
                currentRunningProcess = p;
                break;
            }
        }

        // Add additional processes to our display list if needed
        if (additionalAdded) {
            for (auto& p : additionalProcesses) {
                bool exists = false;
                for (auto& existing : processes) {
                    if (existing->getPid() == p->getPid()) {
                        exists = true;
                        break;
                    }
                }

                if (!exists) {
                    processes.push_back(p);
                }
            }
        }

        // Print the current state after the step
        std::cout << "After step " << stepCount << " (Time " << scheduler.getCurrentTime() << "):\n";
        printProcessDetails(processes);

        // Optional delay to make the output readable
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "\nSimulation complete after " << stepCount << " steps.\n";
    std::cout << "Final Process Execution Details:\n";
    printProcessDetails(processes);

    std::cout << "Average Waiting Time: " << scheduler.getAvgWaitingTime() << std::endl;
    std::cout << "Average Turnaround Time: " << scheduler.getAvgTurnaroundTime() << std::endl;
    std::cout << "=================================\n";
}

int main() {
    // Create test processes
    auto processes = createTestProcesses();

    // Print initial process information
    std::cout << "Initial Process Information:\n";
    std::cout << "+-----------+-------------+------------+------------+\n";
    std::cout << "| Process   | Arrival     | Burst      | Priority   |\n";
    std::cout << "| ID        | Time        | Time       |            |\n";
    std::cout << "+-----------+-------------+------------+------------+\n";

    for (const auto& p : processes) {
        std::cout << "| " << std::setw(9) << p->getPid()
        << " | " << std::setw(11) << p->getArrivalTime()
        << " | " << std::setw(10) << p->getBurstTime()
        << " | " << std::setw(10) << p->getPriority()
        << " |\n";
    }

    std::cout << "+-----------+-------------+------------+------------+\n\n";

    // Test FCFS Scheduler with runStatic
    FCFS_Scheduler fcfs_static;
    testStaticScheduler("FCFS", processes, fcfs_static);

    // Test FCFS Scheduler with runOneStep
    FCFS_Scheduler fcfs_step;
    testStepByStepScheduler("FCFS", processes, fcfs_step);

    // Test SJF Scheduler (Non-Preemptive) with runStatic
    SJF_Scheduler sjf_non_preemptive_static(false);
    testStaticScheduler("SJF", processes, sjf_non_preemptive_static);

    // Test SJF Scheduler (Preemptive) with runOneStep
    SJF_Scheduler sjf_preemptive_step(true);
    testStepByStepScheduler("SJF", processes, sjf_preemptive_step, true);

    // Test Priority Scheduler (Non-Preemptive) with runStatic
    Priority_Scheduler priority_non_preemptive_static(false);
    testStaticScheduler("Priority", processes, priority_non_preemptive_static);

    // Test Priority Scheduler (Preemptive) with runOneStep
    Priority_Scheduler priority_preemptive_step(true);
    testStepByStepScheduler("Priority", processes, priority_preemptive_step, true);

    // Test Round Robin Scheduler with runStatic (quantum = 2)
    RR_Scheduler rr_static(2);
    testStaticScheduler("Round Robin (Quantum = 2)", processes, rr_static);

    // Test Round Robin Scheduler with runOneStep (quantum = 2)
    RR_Scheduler rr_step(2);
    testStepByStepScheduler("Round Robin (Quantum = 2)", processes, rr_step);

    return 0;
}
