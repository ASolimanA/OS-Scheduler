#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <string>
#include <queue>
#include <vector>

using namespace std;

class Process {
private:
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int startTime;
    int completionTime;
    int waitingTime;
    bool isComplete;
    int turnaroundTime;
    int priority;
    string color; // For Gantt Chart Visualization

public:
    Process(); // Default constructor
    Process(int id, int aTime, int bTime); // Parameterized constructor
    Process(int id, int aTime, int bTime, int pri); // Parameterized constructor with priority

    // Getters and setters
    int getPid() const;
    void setPid(int p);
    
    int getArrivalTime() const;
    void setArrivalTime(int at);
    
    int getBurstTime() const;
    void setBurstTime(int bt);
    
    int getRemainingTime() const;
    void setRemainingTime(int rt);
    
    int getStartTime() const;
    void setStartTime(int st);
    
    int getCompletionTime() const;
    void setCompletionTime(int ct);
    
    int getWaitingTime() const;
    void setWaitingTime(int wt);
    
    bool getIsComplete() const;
    void setIsComplete(bool complete);
    
    int getTurnaroundTime() const;
    void setTurnaroundTime(int tat);
    
    int getPriority() const;
    void setPriority(int prio);
    
    std::string getColor() const;
    void setColor(const std::string& c);
};

// Comparison structures for different scheduling algorithms
struct ComparePriority {
    bool operator()(Process a, Process b);
};

struct CompareArrivalTime {
    bool operator()(Process a, Process b);
};

struct CompareBurstTime {
    bool operator()(Process a, Process b);
};

struct CompareRemainingTime {
    bool operator()(Process a, Process b);
};

#endif // PROCESS_H
