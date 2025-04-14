#include <iostream>
#include "algorithm"
#include "FCFS_Scheduler.h"
#include "scheduler.h"
#include "process.h"


int ticks = 0;

FCFS_Scheduler::FCFS_Scheduler() : Scheduler(CompareArrivalTime(), false, "First-Come-First-Served Scheduler")
{

}

void FCFS_Scheduler::run(int runUntilTime)  // -1 = live
{
	updateReadyQueue();


	while(readyQueue.empty())
	{
		ticks++;
	}

	auto current_process = readyQueue.top();


	current_process->setStartTime(ticks);

	while(current_process->getRemainingTime() != 0)
	{
		current_process->setRemainingTime(current_process->getRemainingTime() - 1);
		ticks++;
	}
	current_process->setCompletionTime(ticks);
	current_process->setIsComplete(true);
	current_process->setTurnaroundTime(current_process->getCompletionTime() - current_process->getArrivalTime());
	current_process->setWaitingTime(current_process->getTurnaroundTime() - current_process->getBurstTime());
	readyQueue.pop();

	calculateAvgWaitingTime();
	calculateAvgTurnaroundTime();
	cout << "Avg Waiting Time: " << getAvgWaitingTime() << endl;
	cout << "Avg Turnaround Time: " << getAvgTurnaroundTime() << endl;


}

std::shared_ptr<Process> FCFS_Scheduler::selectNextProcess()
{
	if(!readyQueue.empty())
	{
		auto next = readyQueue.top();
		readyQueue.pop();
		return next;
	}
	return nullptr;
}



/**
int main()
{
	Process P1(1, 0, 4);
	Process P2(2, 1, 5);
	std::shared_ptr<Process> processPtr1 = std::make_shared<Process>(1, 0, 4);
	std::shared_ptr<Process> processPtr2 = std::make_shared<Process>(2, 1, 5);
	std::shared_ptr<Process> processPtr3 = std::make_shared<Process>(2, 4, 10);
	Scheduler* Sch1 = new FCFS_Scheduler();
	Sch1->addProcess(processPtr1);
	Sch1->addProcess(processPtr2);
	Sch1->addProcess(processPtr3);
	cout << Sch1->selectNextProcess()->getArrivalTime();
	cout << Sch1->selectNextProcess()->getBurstTime();
	cout << Sch1->selectNextProcess()->getPid();
	Sch1->run();
}*/
