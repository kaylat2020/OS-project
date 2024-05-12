#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "scheduler.h"
#include "cpu.h"
#include "smm.h"

// global variable definitions
struct node* ready_queue = NULL;
int time_quantum = 10;
int process_table_count = 0;

// creates a new process and adds it to the process table
// & adds it at the end of the ready queue.
void new_process(int base, int size)
{
    // is there enough minimum space to add the process?
    if (process_table_count < 1024)
    {
        struct PCB new_pcb;
        new_pcb.pid = process_table_count;
        new_pcb.size = size;
        new_pcb.registers[0] = base;
        new_pcb.registers[1] = 0;
        new_pcb.registers[2] = 0;
        new_pcb.registers[3] = 0;
        new_pcb.registers[4] = 0;
        new_pcb.registers[5] = 0;
        new_pcb.registers[6] = 0;

        // op testing tool of choice
        printf("PID: %d\n", new_pcb.pid);
        printf("size: %d lines\n", new_pcb.size);
        printf("registers: %d %d %d %d %d %d %d\n", 
            new_pcb.registers[0], 
            new_pcb.registers[1], 
            new_pcb.registers[2], 
            new_pcb.registers[3],
            new_pcb.registers[4], 
            new_pcb.registers[5], 
            new_pcb.registers[6]);

        process_table[process_table_count] = new_pcb;
        process_table_count++;

        // id prefer not to but ok malloc u do u (allocate memory)
        struct node* new_node = (struct node*)malloc(sizeof(struct node));
        new_node->pcb = new_pcb;
        new_node->next = NULL;


        // is the ready queue empty?
        if (ready_queue == NULL)
        {
            // set head node
            ready_queue = new_node;
        }
        else
        {
            // link node to end of queue
            struct node* current = ready_queue;
            while (current->next != NULL) current = current->next;
            current->next = new_node;
        }
    }
}

// takes item at the front of the ready queue linked list and adds it to the back.
void next_process(int process_status)
{
    if (ready_queue == NULL || ready_queue->next == NULL) return;

    struct node* temp = ready_queue;
    ready_queue = ready_queue->next;
    temp->next = NULL;

    // traverse the queue to find the last node
    struct node* last = ready_queue;
    while (last->next != NULL) last = last->next;

    // add the dequeued node to the end of the queue
    if (process_status != 0)
    {
        last->next = temp;
    } 
    else 
    {
        free(temp);
    }

    // print current state of queue
    print_queue();
}

// helper function to dequeue front process from the ready queue
// void remove_process() 
// {
//     // queue is empty, nothing to dequeue
//     if (ready_queue == NULL) return; 

//     struct node* temp = ready_queue;

//     ready_queue = ready_queue->next;
//     free(temp);
// }

// im literally afraid of touching anything that works from part 2

// function to remove a process from the ready queue based on its PID
void remove_process(int pid) 
{
    struct node* current = ready_queue;
    struct node* previous = NULL;

    // traverse the ready queue
    while (current != NULL) 
    {
        if (current->pcb.pid == pid) 
        {
            // process found, remove it from the ready queue
            if (previous == NULL) 
            {
                // if the process is the first in the queue
                ready_queue = current->next;
            } 
            else 
            {
                previous->next = current->next;
            }

            free(current);
            printf("process with PID %d removed from the ready queue.\n", pid);
            return;
        }

        previous = current;
        current = current->next;
    }
    deallocate(pid);

    // 404 process not found
    printf("process with PID %d not found in the ready queue.\n", pid);
}

// function to get the PID of the currently running process
int get_current_pid() 
{
    if (ready_queue != NULL) 
    {
        return ready_queue->pcb.pid;
    } 
    else 
    {
        return -1;  // no process is currently running
    }
}

// helper function to print the contents of the ready queue
void print_queue() 
{
    struct node* temp = ready_queue;
    printf("ready queue: ");

    while (temp != NULL) 
    {
        printf("%d ", temp->pcb.pid);
        temp = temp->next;
    }
    printf("\n\n");
}

/**
 * TODO: BESTIE PLS FIX THIS IT IS SO SCUFFED
 * 
 * Receives as input the number of clock cycles there has been, and calls 
 * context_switch() + next_process() if the current time quantum has expired.
 * Also receives as input the process_status returned by the clock_cycle() function.
 * When process_status is 0, the process has terminated and should be removed from the ready queue.
 * Returns 0 when the ready queue is empty. Otherwise it returns 1.
 */
int schedule(int cycle_num, int process_status) 
{
    int exit_status = 1; // 0 means the ready queue is empty
    int pid = -1;

    // no more processes in the ready queue
    if (ready_queue == NULL) return 0;

    // check if time quantum has expired or process is finished
    if (exit_status == 1 && (cycle_num % time_quantum == 0 || process_status == 0))
    {
        // does the ready queue have >1 node? (context switch)
        if (ready_queue->next != NULL)
        {
            // grab pcb values about to be switched to
            struct PCB new_pcb = ready_queue->next->pcb;
            
            // capture old pid
            pid = ready_queue->pcb.pid;

            // perform context switch (save pcb values of current process)
            ready_queue->pcb = context_switch(new_pcb);

            // move the process to the end of the ready queue
            next_process(process_status); // a necessary evil for now
        } 
        else 
        {
            // has the last process finished?
            if (process_status == 0)
            {
                // grab old pid
                pid = ready_queue->pcb.pid;

                remove_process(ready_queue->pcb.pid);

                // dammit malloc ;-;
                free(ready_queue);

                // ready queue is now empty
                exit_status = 0;
            }
            // no context switches necessary when 1 process left
        }
    }

    // TODo: check this logic
    if ( pid != -1 && process_status == 0 )
    {
        deallocate(pid);
    }

    // neither time quantum expired nor process terminated
    return exit_status; // there are more processes left in the ready queue
}