#include <iostream>
#include <string>
#include <math.h>
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
        Process(){} // Default constructor

        Process(int id, int aTime, int bTime){
            pid=id;
            arrivalTime=aTime;
            burstTime=bTime;
            remainingTime=bTime;//initially
        } // Parameterized constructor
        
        Process(int id, int aTime, int bTime, int pri){
            pid=id;
            arrivalTime=aTime;
            burstTime=bTime;
            remainingTime=bTime;//initially
            priority=pri;
        } // Parameterized constructor

        // Getters and setters 
        // Getter and Setter for pid
        int getPid()  { return pid; }
        void setPid(int p) { pid = p; }

        // Getter and Setter for arrivalTime
        int getArrivalTime()  { return arrivalTime; }
        void setArrivalTime(int at) { arrivalTime = at; }

        // Getter and Setter for burstTime
        int getBurstTime()  { return burstTime; }
        void setBurstTime(int bt) { burstTime = bt; }

        // Getter and Setter for remainingTime
        int getRemainingTime()  { return remainingTime; }
        void setRemainingTime(int rt) { remainingTime = rt; }

        // Getter and Setter for startTime
        int getStartTime()  { return startTime; }
        void setStartTime(int st) { startTime = st; }

        // Getter and Setter for completionTime
        int getCompletionTime()  { return completionTime; }
        void setCompletionTime(int ct) { completionTime = ct; }

        // Getter and Setter for waitingTime
        int getWaitingTime()  { return waitingTime; }
        void setWaitingTime(int wt) { waitingTime = wt; }

        // Getter and Setter for isComplete
        bool getIsComplete()  { return isComplete; }
        void setIsComplete(bool complete) { isComplete = complete; }

        // Getter and Setter for turnaroundTime
        int getTurnaroundTime()  { return turnaroundTime; }
        void setTurnaroundTime(int tat) { turnaroundTime = tat; }

        // Getter and Setter for priority
        int getPriority()  { return priority; }
        void setPriority(int prio) { priority = prio; }

        // Getter and Setter for color
        string getColor()  { return color; }
        void setColor(const string& c) { color = c; }

        
    };
    

    //smaller first
    struct ComparePriority { //for  Priority  scheduling
        bool operator()(Process a, Process b) {
            if(a.getPriority() == b.getPriority()){
                return a.getArrivalTime() > b.getArrivalTime(); 
            }
            return a.getPriority() > b.getPriority(); 
        }
    };  //"If a has greater priority value than b, a is lower priority, so it should come after b."

    
    struct CompareArrivalTime { //FCFS 
        bool operator()(Process a, Process b) {
            if(a.getArrivalTime() == b.getArrivalTime()){ 
                return a.getPid() > b.getPid(); 
            }
            return a.getArrivalTime() > b.getArrivalTime(); 
        }
    };  //"If a has greater ArrivalTime value than b, so it should come after b."


    struct CompareBurstTime { //SJF i.e. Non-Preemptive
        bool operator()(Process a, Process b) {
            if(a.getBurstTime() == b.getBurstTime()){
                return a.getArrivalTime() > b.getArrivalTime(); 
            }
            return a.getBurstTime() > b.getBurstTime(); 
        }
    };  //"If a has greater BurstTime value than b, so it should come after b."


    struct CompareRemainingTime { //SRTF i.e. Preemptive
        bool operator()(Process a, Process b) {
            if(a.getRemainingTime() == b.getRemainingTime()){
                return a.getArrivalTime() > b.getArrivalTime(); 
            }
            return a.getRemainingTime() > b.getRemainingTime(); 
        }
    };  //"If a has greater BurstTime value than b, so it should come after b."


    //RoundRobin you will use circular queue not priority queue


    //**********************************************************************************************************************************

    //****** Just a main function to declare the way to use priority queue in the scheduler algorithms implementation *******///
    /*
    
    int main() {



        priority_queue<Process, vector<Process>, ComparePriority> pq_Priority;
            
        priority_queue<Process, vector<Process>, CompareArrivalTime> pq_FCFS;
            
        priority_queue<Process, vector<Process>, CompareBurstTime> pq_SJF;
        
        priority_queue<Process, vector<Process>, CompareRemainingTime> pq_SRTF;


        //Process(int id, int aTime, int bTime, int pri)

        Process p1 (1, 2, 7, 1);
        Process p2 (2, 2, 9, 2);
        Process p3 (3, 3, 7, 3);
        Process p4 (4, 4, 3, 4);
        Process p5 (5, 5, 10, 1);
        Process p6 (6, 5, 4, 2);

        //replace "pq_Priority" with "pq_FCFS" or "pq_SJF" or "pq_SRTF" as you need
        pq_FCFS.push(p1);
        pq_FCFS.push(p2);
        pq_FCFS.push(p3);
        pq_FCFS.push(p4);
        pq_FCFS.push(p5);
        pq_FCFS.push(p6);
    
        while (!pq_FCFS.empty()) {
            Process p = pq_FCFS.top();
            cout << "PID: " << p.getPid() << ", Priority: " << p.getPriority() << ", arrival time: " << p.getArrivalTime() << "\n";
            pq_FCFS.pop();
        }
    
        return 0;
    }
        
    */
