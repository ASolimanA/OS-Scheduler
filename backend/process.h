#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process
{
private:
    int pid;
    static int nextId;
    std::string name = "";
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int startTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
    int priority;
    bool isComplete;

public:
    Process();
    Process(std::string p_name, int arrival, int burst);
    Process(std::string p_name, int arrival, int burst, int priority);

    // getters
    int getPid() const;
    int getArrivalTime() const;
    int getBurstTime() const;
    int getRemainingTime() const;
    int getStartTime() const;
    int getCompletionTime() const;
    int getWaitingTime() const;
    int getTurnaroundTime() const;
    int getPriority() const;
    bool getIsComplete() const;
    std::string getName();

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
    void setName(std::string &name);

    float getProgress();
};

#endif // PROCESS_H
