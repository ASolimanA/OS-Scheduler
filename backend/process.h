#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
private:
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int startTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
    int priority;
    bool isComplete;
    std::string color; // for Gantt chart

public:
    Process();
    Process(int id, int arrival, int burst);
    Process(int id, int arrival, int burst, int priority);

    // getters
    int  getPid() const;
    int  getArrivalTime() const;
    int  getBurstTime() const;
    int  getRemainingTime() const;
    int  getStartTime() const;
    int  getCompletionTime() const;
    int  getWaitingTime() const;
    int  getTurnaroundTime() const;
    int  getPriority() const;
    bool getIsComplete() const;
    const std::string& getColor() const;

    // setters
    void setPid(int);
    void setArrivalTime(int);
    void setBurstTime(int);
    void setRemainingTime(int);
    void setStartTime(int);
    void setCompletionTime(int);
    void setWaitingTime(int);
    void setTurnaroundTime(int);
    void setPriority(int);
    void setIsComplete(bool);
    void setColor(const std::string&);
};

#endif // PROCESS_H
