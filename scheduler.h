// Process Control Block (pid, size, registers)
struct PCB
{
    int pid;
    int size;
    int registers[7];
};

// ready queue structure
struct node
{
    struct PCB pcb;
    struct node *next;
};

// global variable declarations
extern struct node* ready_queue; // "extern" so it stops yelling at me for definitions here
extern int time_quantum;
// Process Table
struct PCB process_table[1024];
extern int process_table_count;

int schedule(int cycle_num, int process_status);
void new_process(int base, int size);
void remove_process(int pid);
int get_current_pid();
void print_queue();