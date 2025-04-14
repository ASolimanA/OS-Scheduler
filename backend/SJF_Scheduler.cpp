#include <iostream>
#include "algorithm"
#include "SJF_Scheduler.h"
#include "scheduler.h"
#include "process.h"


SJF_Scheduler::SJF_Scheduler(bool isPreemptive)
: Scheduler(CompareRemainingTime(), isPreemptive(isPreemptive), "Shortest-Job-First Scheduling")
{

}

void SJF_Scheduler::run(int runUntilTime) 
{
	updateReadyQueue();

	while(readyQueue.empty())
	{
		ticks++;
	}
	
	auto current_process = readyQueue.top();

	
	if(SJF_Scheduler->Preemptive == true)
	{
		//Preemptive
	}	
	else
	{
		while(!readyQueue->empty())
		{
			int time_slice = quantum_time;
			while(time_slice != 0)
			{
				current_process->setRemainingTime(current_process->getRemainingTime() - 1);
				ticks++;
				time_slice--;
			}
			if(getRemainingTime() == 0)
			{

				current_process->setCompletionTime(ticks);
				current_process->setIsComplete(true);
				current_process->setTurnaroundTime(current_process->getCompletionTime() - current_process->getArrivalTime());
				current_process->setWaitingTime(current_process->getTurnaroundTime() - current_process->getBurstTime());
			}
			readyQueue.pop();
		}
	}
}


std::shared_ptr<Process> SJF_Scheduler::selectNextProcess() 
{
	if(!readyQueue.empty())
	{
		auto next = readyQueue.top();
		readyQueue.pop();
		return next;
	}
	return nullptr;
}