#include "process.h"

// Default
Process::Process()
    : pid(0), arrivalTime(0), burstTime(0), remainingTime(0),
    startTime(-1), completionTime(-1), waitingTime(0),
    turnaroundTime(0), priority(0), isComplete(false),
    color("grey")
{}

// Without priority
Process::Process(int id, int arrival, int burst)
    : pid(id), arrivalTime(arrival), burstTime(burst),
    remainingTime(burst), startTime(-1), completionTime(-1),
    waitingTime(0), turnaroundTime(0), priority(0),
    isComplete(false), color("grey")
{}

// With priority
Process::Process(int id, int arrival, int burst, int prio)
    : pid(id), arrivalTime(arrival), burstTime(burst),
    remainingTime(burst), startTime(-1), completionTime(-1),
    waitingTime(0), turnaroundTime(0), priority(prio),
    isComplete(false), color("grey")
{}

// Getters
int Process::getPid()            const { return pid; }
int Process::getArrivalTime()    const { return arrivalTime; }
int Process::getBurstTime()      const { return burstTime; }
int Process::getRemainingTime()  const { return remainingTime; }
int Process::getStartTime()      const { return startTime; }
int Process::getCompletionTime() const { return completionTime; }
int Process::getWaitingTime()    const { return waitingTime; }
int Process::getTurnaroundTime() const { return turnaroundTime; }
int Process::getPriority()       const { return priority; }
bool Process::getIsComplete()    const { return isComplete; }
const std::string& Process::getColor() const { return color; }

// Setters
void Process::setPid(int v)             { pid = v; }
void Process::setArrivalTime(int v)     { arrivalTime = v; }
void Process::setBurstTime(int v)       { burstTime = v; }
void Process::setRemainingTime(int v)   { remainingTime = v; }
void Process::setStartTime(int v)       { startTime = v; }
void Process::setCompletionTime(int v)  { completionTime = v; }
void Process::setWaitingTime(int v)     { waitingTime = v; }
void Process::setTurnaroundTime(int v)  { turnaroundTime = v; }
void Process::setPriority(int v)        { priority = v; }
void Process::setIsComplete(bool v)     { isComplete = v; }
void Process::setColor(const std::string& v) { color = v; }
