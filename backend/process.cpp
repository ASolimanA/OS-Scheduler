#include "process.h"

int Process::nextId = 1;

// Default
Process::Process()
    : pid(nextId++), arrivalTime(0), burstTime(0), remainingTime(0),
      startTime(-1), completionTime(-1), waitingTime(0),
      turnaroundTime(0), priority(0), isComplete(false)
{
}

// Without priority
Process::Process(std::string p_name, int arrival, int burst)
    : pid(nextId++), arrivalTime(arrival), burstTime(burst),
      remainingTime(burst), startTime(-1), completionTime(-1),
      waitingTime(0), turnaroundTime(0), priority(0),
      isComplete(false), name(p_name)
{
}

// With priority
Process::Process(std::string p_name, int arrival, int burst, int prio)
    : pid(nextId++), arrivalTime(arrival), burstTime(burst),
      remainingTime(burst), startTime(-1), completionTime(-1),
      waitingTime(0), turnaroundTime(0), priority(prio),
      isComplete(false), name(p_name)
{
}

float Process::getProgress()
{
  if (remainingTime == 0)
    return 100.0f; // Completed
  if (burstTime == 0)
    return 0.0f; // Not started
  return (static_cast<float>(burstTime - remainingTime) / burstTime) * 100.0f;
}

// Getters
int Process::getPid() const { return pid; }
int Process::getArrivalTime() const { return arrivalTime; }
int Process::getBurstTime() const { return burstTime; }
int Process::getRemainingTime() const { return remainingTime; }
int Process::getStartTime() const { return startTime; }
int Process::getCompletionTime() const { return completionTime; }
int Process::getWaitingTime() const { return waitingTime; }
int Process::getTurnaroundTime() const { return turnaroundTime; }
int Process::getPriority() const { return priority; }
bool Process::getIsComplete() const { return isComplete; }
std::string Process::getName() { return name; }

// Setters
void Process::setPid(int v) { pid = v; }
void Process::setArrivalTime(int v) { arrivalTime = v; }
void Process::setBurstTime(int v) { burstTime = v; }
void Process::setRemainingTime(int v) { remainingTime = v; }
void Process::setStartTime(int v) { startTime = v; }
void Process::setCompletionTime(int v) { completionTime = v; }
void Process::setWaitingTime(int v) { waitingTime = v; }
void Process::setTurnaroundTime(int v) { turnaroundTime = v; }
void Process::setPriority(int v) { priority = v; }
void Process::setIsComplete(bool v) { isComplete = v; }
void Process::setName(std::string &name) { this->name = name; }
