#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "memory.h"
#include "cpu.h"
#include "disk.h"
#include "scheduler.h"
#include "smm.h"

/**
 * @author Kayla Tucker
 * @version 3.0 (12/6/2023)
 * I have a fuckton of comments so that C shenanigans dont drive me crazy
 * now featuring several new cries for help!
 * 
 * PATCH NOTES
 * current problems that I'll pass off as "quirks":
 * (see cycle #309 when running "program_list_valid.txt")
 * there is a flaw in cycle_count in that if a process finishes early, 
 * the next process may exit early as it resyncs with 
 * (cycle_count % time_quantum). This creates ineffeciencies.
 * 
 * fix: create a separate variable in cpu called cycles that counts up to a 
 * max of time_quantum, starts at 0, and resets to 0 after a context switch occurs.
 */
int main(int argc, char *argv[])
{
   // just in case the code runner tries something stupid
   if (argc > 2) 
   {
      printf("\nError: too many command line arguments given.\n"
         "Maybe read the fucking manual (readme.md) next time?\n\n");
      exit(1);
   }

   // initialize system configurations
   int cycle_count = 0;

   struct hole* new_hole = (struct hole*)malloc(sizeof(struct hole));
   new_hole->base = 0;
   new_hole->size = 1024;
   new_hole->next = NULL;
   holes = new_hole;

   print_holes(); // test
   
   // (optional) pass the program list as an argument in command line
   // (+ ternary operators cause it makes me look cool)
   (argc == 2) ? load_programs(argv[1]): load_programs("program_list_valid.txt");
   
   // prepare to run processes
   context_switch(ready_queue->pcb);

   // main execution loop
   while (1)
   {
      printf("cycle #%d | ", cycle_count);

      // execute 1 clock cycle
      int process_status = clock_cycle();
      cycle_count++;

      // call scheduler with the cycle count & process status
      int result = schedule(cycle_count, process_status);

      // check if the system should terminate
      if (result == 0) break;
   }

   // print hole_count + print locations {30, 150, 230}
   
   printf("\ntotal holes created: %d\n", hole_count);
   print_holes();
   printf("\nmemory locations {30, 150, 230}: {[%d %d],[%d %d],[%d %d]}\n\n",
      memory[30][0], memory[30][1], 
      memory[150][0], memory[30][1], 
      memory[230][0], memory[30][1]);

   // print first 20 memory locations
   // printf("memory (first 20 locations):\n ");
   // for (int i = 0; i < 20; i++)
   // {
   //    int* mem = mem_read(i);
   //    printf("%d: [%d, %d]\n", i, mem[0], mem[1]);
   //    if (i < 9) printf(" ");
   // }

   return 0;
}