# READ ME

Kayla Tucker  
KAT190004

## How to Run

1\. extract zip file to UTD Giant Server or UNIX environment  
2\. enter these commands while in the `/project` directory:  

```text
make  
./project [a program_list.txt file]
```

### OR

```text
make
./project
```

(^ this will pass "program_list_valid.txt" as an argument by default)

3\. ???  
4\. profit  

(btw if u want to add extra txt programs make sure they're in the project directory.  
I should probably make a separate folder for programs later but it complicates paths)

## Instruction Set Programs

This simulated computer will run programs on a very limited instruction set.  
The programs are `.txt` files made up of these instructions.  
They will be loaded by the computer, translated into their integer OP Codes, and then stored in memory with their arguments.  
Each instruction will consist of a command, and will potentially be followed by a space and a single argument and then a newline character.  
If no argument is needed for the command, then it will be immediately followed by a newline.

### The possible commands and their arguments are as follows

Command  
OP Code | Argument | Description

```text  
exit
0 | None | Signals the end of the current program

load_const
1 | int | Load the integer given by int into the AC register

move_from_mbr
2 | None | Move the integer in the MBR register into the AC register

move_from_mar
3 | None | Move the integer in the MAR register into the AC register

move_to_mbr
4 | None | Move the integer in the AC register into the MBR register

move_to_mar
5 | None | Move the integer of the AC register into the MAR register

load_at_addr
6 | None | Load an integer in memory using the address in the MAR register into the MBR register

write_at_addr
7 | None | Write the integer in the MBR register to memory at the address in the MAR register

add
8 | None | Add the contents of the MBR register to the AC register

multiply
9 | None | Multiply the contents of the MBR register by the AC register and store the result in the AC register

and
10 | None | Perform the logical AND operation between the MBR register and the AC register. 0 is False, all other numbers are True. 
Store the result in the AC register.

or 
11 | None | Perform the logical OR operation between the MBR
register and the AC register. 0 is False, all other
numbers are True. Store the result in the AC register.

ifgo 
12 | addr | If the contents of the AC register is not 0, then load the integer (addr - 1) into the PC register. Otherwise, do nothing. 

sleep 
13 | None | Do nothing.

//
None | None | Comment. Ignore this line during loading.
```

### Example programs *loop50.txt*, *loop100.txt*, *loop200_invalid.txt* and *loop200_valid.txt* are provided
