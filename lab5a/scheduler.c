/*******************************************************************************
*
* CprE 308 Scheduling Lab
*
* scheduling.c
*******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define NUM_PROCESSES 20

struct process
{
  /* Values initialized for each process */
  int arrivaltime;  /* Time process arrives and wishes to start */
  int runtime;      /* Time process requires to complete job */
  int priority;     /* Priority of the process */

  /* Values algorithm may use to track processes */
  int starttime;
  int endtime;
  int flag;
  int completionFlag;
  int remainingtime;
};

/* Forward declarations of Scheduling algorithms */
void first_come_first_served(struct process *proc);
void shortest_remaining_time(struct process *proc);
void round_robin(struct process *proc);
void round_robin_priority(struct process *proc);

int main()
{
  int i;
  struct process proc[NUM_PROCESSES],      /* List of processes */
                 proc_copy[NUM_PROCESSES]; /* Backup copy of processes */

  /* Seed random number generator */
  // srand(time(0));  /* Use this seed to test different scenarios */
  srand(0xC0FFEE);     /* Used for test to be printed out */

  /* Initialize process structures */
  for(i=0; i<NUM_PROCESSES; i++)
  {
    proc[i].arrivaltime = rand()%100;
    proc[i].runtime = (rand()%30)+10;
    proc[i].priority = rand()%3;
    proc[i].starttime = 0;
    proc[i].endtime = 0;
    proc[i].flag = 0;
    proc[i].completionFlag = 0;
    proc[i].remainingtime = proc[i].runtime;
  }

  /* Show process values */
  printf("Process\tarrival\truntime\tpriority\n");
  for(i=0; i<NUM_PROCESSES; i++)
    printf("%d\t%d\t%d\t%d\n", i, proc[i].arrivaltime, proc[i].runtime,
           proc[i].priority);

  /* Run scheduling algorithms */
  printf("\n\nFirst come first served\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  first_come_first_served(proc_copy);

  printf("\n\nShortest remaining time\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  shortest_remaining_time(proc_copy);

  printf("\n\nRound Robin\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  round_robin(proc_copy);

  printf("\n\nRound Robin with priority\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  round_robin_priority(proc_copy);

  return 0;
}

void first_come_first_served(struct process *proc)
{
  int time = 100;
  // Need a starting point for time
  for (int i = 0; i < NUM_PROCESSES; i++)
  {
  	if(proc[i].arrivaltime < time)
  	{
  		time = proc[i].arrivaltime;
  	}
  }
  int index = 0;
  int average = 0;
  int complete = 0;

  // Keep going until all processes are completed
  while(complete < NUM_PROCESSES)
  {
  	// Want to just find first arriving process
    int min = 100;
    for(int j = 0; j < NUM_PROCESSES; j++)
    {
      if(proc[j].arrivaltime < min && proc[j].flag != 1)
      {
        min = proc[j].arrivaltime;
        index = j;
      }
    }
    // Run process in arrival order until they reach completion
    proc[index].starttime = time;
    printf("Process %d started at time  %d\n", index, time);
    time += proc[index].runtime;
    proc[index].endtime = time;
    printf("Process %d finished at time %d\n", index, time);
    average += (proc[index].endtime - proc[index].arrivaltime);
    proc[index].flag = 1;
    complete++;
  }
  printf("Average time from arrival to finish is %d seconds\n", average/NUM_PROCESSES);
}

void shortest_remaining_time(struct process *proc)
{
  int time = 100;
  // Need a starting point for time
  for (int i = 0; i < NUM_PROCESSES; i++)
  {
  	if(proc[i].arrivaltime < time)
  	{
  		time = proc[i].arrivaltime;
  	}
  }
  int index = 0;
  int average = 0;
  int complete = 0;

  // Keep going until all processes are completed
  while(complete < NUM_PROCESSES)
  {
    int min = 100;
    for(int j = 0; j < NUM_PROCESSES; j++)
    {
      // Find process with shortest time remaining, which has already arrived
      if(proc[j].runtime < min && proc[j].flag != 1 && proc[j].arrivaltime <= time)
      {
        min = proc[j].runtime;
        index = j;
      }
    }

    // Simply run process until ccompletion 
    proc[index].starttime = time;
    printf("Process %d started at time  %d\n", index, time);
    time += proc[index].runtime;
    proc[index].endtime = time;
    printf("Process %d finished at time %d\n", index, time);
    average += (proc[index].endtime - proc[index].arrivaltime);
    proc[index].flag = 1;
    complete++;
  }
  printf("Average time from arrival to finish is %d seconds\n", average/NUM_PROCESSES);
}

void round_robin(struct process *proc)
{
  int time = 100;
  // Need a starting point for time
  for (int i = 0; i < NUM_PROCESSES; i++)
  {
  	if(proc[i].arrivaltime < time)
  	{
  		time = proc[i].arrivaltime;
  	}
  }
  int index = 0;
  int average = 0;
  int QUANTUM = 10;
  int complete = 0;

  // Keep going until all processes are completed
  while(complete < NUM_PROCESSES)
  {
    for(int j = 0; j < NUM_PROCESSES; j++)
    {
      // If the process has arrived, and it is not complete 
      if(proc[j].arrivaltime <= time && proc[j].flag != 1)
      {
      	// If the process is only just starting, assign start time
        if(proc[j].starttime == 0) 
        {
        	proc[j].starttime = time;
        	printf("Process %d started at time  %d\n", j, proc[j].starttime);
        }

        // Subtract QUANTUM from time remaining and increment time accordingly
        proc[j].remainingtime -= QUANTUM;
        time += QUANTUM;

        // If the process completes
        if(proc[j].remainingtime <= 0)
        {
          // Assign end time (adjust for QUANTUM bringing remaining time to negative value)
          proc[j].endtime = time + proc[j].remainingtime;
          time += proc[j].remainingtime;
          proc[j].remainingtime = 0;
          printf("Process %d finished at time %d\n", j, proc[j].endtime);

          // Add time to completion to our average
          average += (proc[j].endtime - proc[j].arrivaltime);
          proc[j].flag = 1;
          complete++;
        }
      }
    }
  }
  printf("Average time from arrival to finish is %d seconds\n", average/NUM_PROCESSES);
}

void round_robin_priority(struct process *proc)
{
  int time = 100;
  int priority = 2;

  // Need to start time at first arrival
  for (int i = 0; i < NUM_PROCESSES; i++)
  {
  	if(proc[i].arrivaltime < time && proc[i].priority == priority)
  	{
  		time = proc[i].arrivaltime;
  	}
  }
  int index = 0;
  int average = 0;
  int QUANTUM = 10;
  int complete = 0;
  int priorityCount = 0;

  // Keep going until all processes are completed
  while(complete < NUM_PROCESSES)
  {
    for(int j = 0; j < NUM_PROCESSES; j++)
    {
      // Once time has advanced, if there is now a high priority process
      // we must run the high priority process first
      if (proc[j].priority > priority && proc[j].arrivaltime <= time)
      {
      	priority = proc[j].priority;
      }
      // If the process has arrived, is not complete, and is of the current priority
      if(proc[j].arrivaltime <= time && proc[j].flag != 1 && proc[j].priority == priority)
      {
      	// Assign start time if process is just starting
        if(proc[j].starttime == 0) 
        {
        	proc[j].starttime = time;
        	printf("Process %d started at time  %d\n", j, proc[j].starttime);
        }
        // Subtract QUANTUM from remaining time of current process and add to time
        proc[j].remainingtime -= QUANTUM;
        time += QUANTUM;

        // If process completes
        if(proc[j].remainingtime <= 0)
        {
          // Adjust time to only move up by remaining time of process
          proc[j].endtime = time + proc[j].remainingtime;
          time += proc[j].remainingtime;
          proc[j].remainingtime = 0;
          printf("Process %d finished at time %d\n", j, proc[j].endtime);

          // Assign process as completed
          average += (proc[j].endtime - proc[j].arrivaltime);
          proc[j].flag = 1;
          complete++;
          proc[j].priority = -1;
        }
      }
      else
      {
      	// This statement lowers the priority level if no process which
      	// has arrived can be run, therefor the priority must be lowered
       	priorityCount++;
       	if(priorityCount == NUM_PROCESSES)
       	{
       		priorityCount = 0;
       		priority--;
       	}
      }
    }
    priorityCount = 0;
  }
  printf("Average time from arrival to finish is %d seconds\n", average/NUM_PROCESSES);
}
