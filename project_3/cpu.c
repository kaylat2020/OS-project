#include <stdio.h>
#include "cpu.h"
#include "scheduler.h"
#include "memory.h"

// CPU registers (in array for PCB)
int Base;     // Base Register
int PC;       // Program Counter
int IR0;      // Instruction Register 0 (OP Code)
int IR1;      // Instruction Register 1 (Argument)
int AC;       // Accumulator Register
int MAR;      // Memory Address Register
int MBR;      // Memory Buffer Register

// Receives as input the new register values to write into the CPU registers, 
// and returns the old register values that were previously in the CPU registers.
struct PCB context_switch(struct PCB new_pcb)
{
    printf("\ncontext switch time! (╯°□°）╯︵ ┻━━┻\n");

    // create struct to hold the old PCB
    struct PCB old_pcb;

    // save the old register values in PCB
    old_pcb.pid = ready_queue->pcb.pid;
    old_pcb.size = ready_queue->pcb.size;
    old_pcb.registers[0] = Base;
    old_pcb.registers[1] = PC;
    old_pcb.registers[2] = IR0;
    old_pcb.registers[3] = IR1;
    old_pcb.registers[4] = AC;
    old_pcb.registers[5] = MAR;
    old_pcb.registers[6] = MBR;

    printf("old registers: %d %d %d %d %d %d %d\n", 
        Base, PC, IR0, IR1, AC, MAR, MBR);

    // write new PCB values to CPU
    Base = new_pcb.registers[0];
    PC = new_pcb.registers[1];
    IR0 = new_pcb.registers[2];
    IR1 = new_pcb.registers[3];
    AC = new_pcb.registers[4];
    MAR = new_pcb.registers[5];
    MBR = new_pcb.registers[6];

    printf("new registers: %d %d %d %d %d %d %d\n\n", 
        Base, PC, IR0, IR1, AC, MAR, MBR);

    // return old PCB
    return old_pcb;
}

// Read the instruction in memory at location addr, 
// and place the OP Code in IR0 and the argument (if applicable) in IR1.
void fetch_instruction(int addr)
{
	int* instruction = mem_read(mem_address(addr));
    if (instruction != NULL)
    {
        IR0 = instruction[0]; // op code -> IR0
        IR1 = instruction[1]; // arg (if applicable) -> IR1
    }
}

// Returns true memory address by computing Base + l_addr
int mem_address(int l_addr)
{
    return Base + l_addr;
}

// Execute the instruction in the IR0 and IR1 registers.
void execute_instruction()
{
	switch (IR0) 
	{
        case 0:
            // exit
            IR0 = 0;
            printf("\nEnd of program reached, yippee! (ﾉ◕ヮ◕)ﾉ*:･ﾟ✧\n\n");
            break;
        case 1:
            // load_const
            AC = IR1;
            break;
        case 2:
            // move_from_mbr
            AC = MBR;
            break;
        case 3:
            // move_from_mar
            AC = MAR;
            break;
		case 4:
			// move_to_mbr
            MBR = AC;
            break;
		case 5:
			// move_to_mar
            MAR = AC;
            break;
		case 6:
			// load_at_addr
            MBR = *mem_read(MAR);
            break;
		case 7:
            // write_at_addr
            mem_write(MAR, &MBR);
            break;
		case 8:
            // add
            AC += MBR;
            break;
		case 9:
            // multiply
            AC *= MBR;
            break;
		case 10:
            // and
            AC = (AC && MBR) ? 1 : 0;
            break;
        case 11:
            // or
            AC = (AC || MBR) ? 1 : 0;
            break;
        case 12:
            // ifgo
            if (AC != 0) PC = IR1 - 1;
            break;
        case 13:
            // sleep
            break;
        default:
            // handler for unknown instructions
            printf("well that's a whole lotta nothing\n");
            break;
    }
}

/**
 * Iterate through one clock cycle of the CPU, 
 * i.e. fetch an instruction and execute the command. 
 * Returns 0 when the exit command is encountered, returns 1 otherwise. 
 */ 
int clock_cycle() 
{
    // OP af testing tool 11/10
    printf("PID: %d | ", ready_queue->pcb.pid);
    printf("registers: %d %d %d %d %d %d %d\n", Base, PC, IR0, IR1, AC, MAR, MBR);
    // printf("mem address: %d\n", mem_address(PC));

    fetch_instruction(PC);
    execute_instruction();

    if (IR0 == 0) return 0; // exit

    PC++;

    return 1; // continue executing
}