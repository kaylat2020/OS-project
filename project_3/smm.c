#include <stdio.h>
#include <stdlib.h>
#include "smm.h"

struct hole* holes;
int alloc_table[256][3];
int hole_count = 1;

// helper function to print the holes
void print_holes() 
{
    struct hole* temp = holes;
    printf("holes: ");

    while (temp != NULL) 
    {
        printf("[base: %d size: %d] -> ", temp->base, temp->size);
        temp = temp->next;
    }
    printf("NULL\n\n");
}

int allocate(int pid, int size) 
{
    // TODO: something's going wrong in here logic wise bestie
    int base = find_hole(size);
    if (base == -1) 
    {
        printf("Error: allocation failed for PID %d of size %d.\n", 
            pid, size);
        return 0; // reject process
    }

    // modifies current hole while allocating space
    struct hole* new_hole = (struct hole*)malloc(sizeof(struct hole));
    new_hole = holes;
    new_hole->base = base + size;
    new_hole->size = new_hole->size - size;

    print_holes();

    // update alloc_table
    int row = find_empty_row(); 
    if (row != -1) 
    {
        alloc_table[row][0] = pid;
        alloc_table[row][1] = base;
        alloc_table[row][2] = size;
    }

    return 1; // process allocated successfully
}

void deallocate(int pid) 
{
    // add a new hole with the same base address and size
    add_hole(alloc_table[pid][1], alloc_table[pid][2]);

    // set the size of the corresponding row in the alloc_table to 0
    alloc_table[pid][2] = 0;
}

void add_hole(int base, int size) 
{
    // create a new hole
    struct hole* new_hole = (struct hole*)malloc(sizeof(struct hole));
    new_hole->base = base;
    new_hole->size = size;
    new_hole->next = NULL;
    hole_count++;

    // if the holes linked list is empty (it shouldn't but eh),
    // add the new hole as the first node
    if (holes == NULL) 
    {
        holes = new_hole;
        return;
    }

    // if the new hole should be inserted at the beginning
    if (base < holes->base) 
    {
        new_hole->next = holes;
        holes = new_hole;
        merge_holes(); // merge adjacent holes
        return;
    }

    // search for where to insert the new hole
    struct hole* current = holes;
    struct hole* previous = NULL;

    while (current != NULL && current->base < base) 
    {
        previous = current;
        current = current->next;
    }

    // Insert the new hole between previous and current
    if (previous != NULL) 
    {
        previous->next = new_hole;
        new_hole->next = current;
    }

    merge_holes(); // Merge adjacent holes

    print_holes();
}

void remove_hole(int base) 
{
    struct hole* current = holes;
    struct hole* prev = NULL;

    // Find the hole to remove
    while (current != NULL && current->base != base) 
    {
        prev = current;
        current = current->next;
    }

    // Remove the hole from the linked list
    if (prev == NULL) 
    {
        holes = current->next;
    } 
    else 
    {
        prev->next = current->next;
    }

    // yeah um dont forget to do this bestie
    free(current); 
}

void merge_holes() 
{
    struct hole* current = holes;

    // >1 hole in holes?
    while (current != NULL && current->next != NULL) 
    {
        if ((current->base + current->size) == current->next->base) 
        {
            // merge adjacent holes
            current->size += current->next->size;
            struct hole* temp = current->next;
            current->next = current->next->next;
            free(temp);
        } 
        else 
        {
            // carry on
            current = current->next;
        }
    }
}

int find_hole(int size) 
{
    struct hole* current = holes;
    int base = -1;

    // first-fit algorithm iterating through holes
    while (current != NULL) 
    {
        if (current->size >= size) 
        {
            base = current->base;
            break;
        }
        current = current->next;
    }

    return base;
}

int get_base_address(int pid) 
{
    for (int i = 0; i < 256; ++i) 
    {
        if (alloc_table[i][0] == pid) 
        {
            return alloc_table[i][1];
        }
    }
    return -1;  // PID not found
}

int find_empty_row() 
{
    for (int i = 0; i < 256; ++i) 
    {
        if (alloc_table[i][2] == 0) 
        {
            return i;
        }
    }
    return -1;  // no empty row found
}

int is_allowed_address(int pid, int addr) 
{
    for (int i = 0; i < 256; ++i) 
    {
        if (alloc_table[i][0] == pid) 
        {
            int base = alloc_table[i][1];
            int size = alloc_table[i][2];

            // is it within the bounds of the program allocation?
            return (addr >= base && addr < (base + size));
        }
    }
    return 0;  // PID not found
}
