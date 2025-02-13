#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "memory.h"
#include "cpu.h"
#include "scheduler.h"
#include "smm.h"

// translate the given instruction into its associated integer OP Code. 
// return the argument too if applicable.
int* translate(char* instruction)
{
	static int translation[2] = {0, 0};
	char* token = strtok(instruction, " \t\r\n");

	// covering my "cases" ha get it cause its supposed to be bases pls kill me ;-; (#3)
	if (token == NULL) return translation;

	if (strcmp(token, "exit") == 0)
	{
		translation[0] = 0;
		translation[1] = 0;
	} 
	else if (strcmp(token, "load_const") == 0)
	{
		translation[0] = 1;
		token = strtok(NULL, " \t\r\n");
	if (token != NULL) translation[1] = atoi(token);
	}
	else if (strcmp(token, "move_from_mbr") == 0)
	{
		translation[0] = 2;
		translation[1] = 0;
	}
	else if (strcmp(token, "move_from_mar") == 0)
	{
		translation[0] = 3;
		translation[1] = 0;
	}
	else if (strcmp(token, "move_to_mbr") == 0)
	{
		translation[0] = 4;
		translation[1] = 0;
	}
	else if (strcmp(token, "move_to_mar") == 0)
	{
		translation[0] = 5;
		translation[1] = 0;
	}
	else if (strcmp(token, "load_at_addr") == 0)
	{
		translation[0] = 6;
		translation[1] = 0;
	}
	else if (strcmp(token, "write_at_addr") == 0)
	{
		translation[0] = 7;
		translation[1] = 0;
	}
	else if (strcmp(token, "add") == 0)
	{
		translation[0] = 8;
		translation[1] = 0;
	}
	else if (strcmp(token, "multiply") == 0)
	{
		translation[0] = 9;
		translation[1] = 0;
	}
	else if (strcmp(token, "and") == 0)
	{
		translation[0] = 10;
		translation[1] = 0;
	}
	else if (strcmp(token, "or") == 0)
	{
		translation[0] = 11;
		translation[1] = 0;
	}
	else if (strcmp(token, "ifgo") == 0)
	{
		translation[0] = 12;
		token = strtok(NULL, " \t\r\n");
	if (token != NULL) translation[1] = atoi(token);
	}
	else if (strcmp(token, "sleep") == 0)
	{
		translation[0] = 13;
		translation[1] = 0;
	}
	else
	{
		// handler for unrecognized instructions, assuming no arguments
		translation[0] = -1;
		translation[1] = 0;
		printf("OP code not recognized: %s\n", token);
	}
	// printf("%d %d\n", translation[0], translation[1]); // testing
	return translation;
}

// function to extract the argument from an instruction
int extract_argument(char* instruction) 
{
	int argument = 0;
	char* arg_token = strtok(instruction, " ");

	while ( arg_token != NULL ) 
	{
		if ( sscanf(arg_token, "%d", &argument) == 1) 
		{
			// successfully extracted an integer argument
			break;
		}
		arg_token = strtok(NULL, " ");
	}
	return argument;
}

// helper function to calculate the "size" of a program in memory
int size(char program_fname[]) 
{
	// F U WINDOWS (#5) (it's personal. seriously. this took several days to debug)
	program_fname[strcspn(program_fname, "\r\n")] = 0;

	FILE* file = fopen(program_fname, "r");

    if (file == NULL) 
	{
        perror("Error opening program file to get size");
        return -1;
    }

    int size = 0; // initialize the size to 0

    char line[256]; // buffer

	// count each line as an instruction or memory space
    while (fgets(line, sizeof(line), file)) size++; 

    fclose(file);

    return size;
}

// Load the list of programs given in the text file specified by fname. 
// Each program will request the space allocation (alloc) 
// required to be loaded into memory as specified in the list.
void load_programs(char fname[]) 
{
	FILE* file = fopen(fname, "r");

	if (file == NULL) 
	{
		printf("Error: couldn't open file %s. Exiting now...\n\n", fname);
		exit(1);
	}
	else
	{
		printf("File \"%s\" opened! Reading now...\n\n", fname);
	}

	char line[256]; // buffer

	// temp storage (cause filestream management is whack)
	int program_alloc_list[10];
	char program_fname_list[10][256];
	int program_list_size = 0;

	// read/store list of programs (MAX 10)
	for (int i = 0; fgets(line, sizeof(line), file) && i < 10; i++)
	{
		// remove trailing newline character (eww don't look)
		size_t len = strlen(line);
		if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

		// tokenize the line to extract memory address and program filename
		char* token = strtok(line, " ");
		if (token == NULL) continue; // skip empty lines
		int alloc = atoi(token);
		program_alloc_list[i] = alloc; // store

		// testing
		printf("%d ", alloc);

		token = strtok(NULL, " ");
		if (token == NULL) continue; // skip lines without a filename
		strcpy(program_fname_list[i], token); // store

		// testing
		printf("%s\n", program_fname_list[i]);
		
		program_list_size++;
	}
	fclose(file);
	printf("\n");

	// create/load processes (i represents PID)
	for (int i = 0; i < program_list_size; i++)
	{
		int program_size = size(program_fname_list[i]);

		if (program_size >= 0) 
		{

			// ask if enough memory (a hole of size >= alloc) is available
			int allocation_request = allocate(i, program_alloc_list[i]);

			// if not, skip adding this program
			if (allocation_request == 0) 
			{
				printf("program '%s' rejected.\n", program_fname_list[i]);
				continue;
			}

			// load program into memory
			load_prog(program_fname_list[i], alloc_table[i][1]);

			// create new process
			new_process(alloc_table[i][1], program_size);
		}
		printf("\n");
	}
}

// Load the program with the name fname, translate it into integer OP Codes, 
// and then store it in memory at address addr
void load_prog(char* fname, int addr)
{
	// fname[strcspn(fname, "\r\n")] = 0; // sigh
	FILE* file = fopen(fname, "r"); 

	if (file == NULL) 
	{
		printf("Error: couldn't open file \"%s\"\nIs the program in the room with us right now?\n", fname);
		exit(1);
	}
	else
	{
		printf("File \"%s\" opened! Reading now...\n", fname);
	}

	char line[256]; // buffer
	int translation[2] = {0, 0};

	while (fgets(line, sizeof(line), file)) 
	{
		// remove trailing newline character (eww don't look)
		size_t len = strlen(line);
		if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

		// skip lines starting with //
		if (strncmp(line, "//", 2) == 0) continue;

		// translate the instruction and store it in memory
		int* translation = translate(line);
		// i have to use memory directly now since mem_write was changed
		memory[addr][0] = translation[0];
		memory[addr][1] = translation[1];

		// move to next memory address
		addr++;
	}
	fclose(file); 
}