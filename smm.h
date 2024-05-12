// hole node structure
struct hole 
{
    int base;
    int size;
    struct hole* next;
};

extern struct hole* holes; // hole linked list
extern int alloc_table[256][3]; // [PID, base, size]
extern int hole_count; // keeps track of total holes made

void print_holes();
void initialize_smm();
int allocate(int pid, int size);
void deallocate(int pid);
void add_hole(int base, int size);
void remove_hole(int base);
void merge_holes();
int find_hole(int size);
int get_base_address(int pid);
int find_empty_row();
int is_allowed_address(int pid, int addr);