#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _MINIOS_HEADER_
#define _MINIOS_HEADER_

#define MAXCHARS 1000
#define MAXFUNCTIONS 9
#define MAXMEMORY 3145728 // 3MiB in Bytes
#define MAXINT 32767
#define MAXEXEC 10000000

typedef struct node
{
	void *data;
	struct node *next;
} node;

typedef struct queue
{
	struct node *front, *rear;
} queue;

typedef struct process
{
	int PID;
	int priority;
	long int execution_time; // initial execution time
	long int remaining_time; // this will me modified
	long int memory_start;
	long int memory_occupied; // the memory that the stack occupies
	long int memory_size; // maximum memory that can be occupied by the stack

	node *stack;
} process;

#define PPID(A) (((process *)(A->data))->PID)
#define PPRIOR(A) (((process *)(A->data))->priority)
#define PEXEC(A) (((process *)(A->data))->execution_time)
#define PREM(A) (((process *)(A->data))->remaining_time)
#define PSTART(A) (((process *)(A->data))->memory_start)
#define PSIZE(A) (((process *)(A->data))->memory_size)
#define PSTACK(A) (((process *)(A->data))->stack)

typedef struct process_mem
{
	int PID;
	long int memory_start;
	long int memory_end;
} process_mem;

#define PMPID(A) (((process_mem *)(A->data))->PID)
#define PMSTART(A) (((process_mem *)(A->data))->memory_start)
#define PMEND(A) (((process_mem *)(A->data))->memory_end)

typedef struct function_map
{
	char *fname; // function mapping
	void (*f)();
} function_map;

typedef struct main_params
{
	// pass arguments to functions more easily
	int T; // time

	node *memory;
	queue *waiting_queue;
	queue *finished_queue;

	node *running;
	
	FILE *in_file, *out_file;
	char *buffer;
} main_params;

// custom_functions
void *malloc_c(size_t size);
FILE *fopen_c(char *filename, char *mode);
void push(node **astack, node *new_node);
node *pop(node **astack);
void enqueue(queue *queue, node *new_node);
node *dequeue(queue *queue);

// init_free_functions
node *init_node();
queue *init_queue();
void free_queue(queue *queue);
process *init_process();
void free_process(process *process);
main_params *init_params(char *in_name, char *out_name);
void free_params(main_params *params);
void free_memory(node *memory);

// primary_functions
void add_process(main_params *params);
void process_status(main_params *params);
void push_to_process(main_params *params);
void pop_from_process(main_params *params);
void print_stack(main_params *params);
void print_waiting(main_params *params);
void print_finished(main_params *params);
void run_jobs(main_params *params);
void finish_jobs(main_params *params);

// secondary functions
int find_PID(node *memory);
long int find_memory(node **amemory, long int memory_size, int PID);
void add_to_waiting(queue *main_queue, node *new_node);
process *find_process(main_params *params, int PID, int level);
long int do_jobs(main_params *params, long int time);

// support functions
process *find_in_queue(queue *main_queue, int PID, int is_finished, FILE *f);
void defrag(node **amemory);
void erase_from_memory(node **amemory, long int memory_start);
int enqueue_ordered(queue *new_queue, node *curr_node, node *new_node);

// source
void (*find_function(char *buffer))(main_params *);

#endif