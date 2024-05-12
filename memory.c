#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "smm.h"
#include "scheduler.h"

int memory[1024][2];

// read from memory at location addr and return the data
int* mem_read(int addr)
{
    int current_pid = get_current_pid();
    if (is_allowed_address(current_pid, addr) == 1)
    {
        return memory[addr];
    }
    else
    {
        printf("Error: memory read by PID %d "
            "out of bounds at address %d "
            "\nTerminating process...\n\n",
             current_pid, addr);
        deallocate(current_pid);
        remove_process(current_pid);
        return NULL;
    }
}

// write the given data into memory at the location given by addr.
void mem_write(int addr, int* data)
{
    int current_pid = get_current_pid();
	if (is_allowed_address(current_pid, addr) == 1) 
    {
        // copy data into memory at the specified address
        memcpy(memory[addr], data, 2 * sizeof(int));
    } 
    else 
    {
        printf("Error: memory write by PID %d "
            "out of bounds at address %d. "
            "\nTerminating process...\n\n",
             current_pid, addr);
        deallocate(current_pid);
        remove_process(current_pid);
    }
}
