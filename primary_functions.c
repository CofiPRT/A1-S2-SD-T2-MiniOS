#include "main_header.h"

void add_process(main_params *params)
{
	process *new_process = init_process();

	if (sscanf(params->buffer, 	"%*s %ld %ld %d ", 
								&new_process->memory_size,
								&new_process->execution_time,
								&new_process->priority) != 3)
	{
		fprintf(stderr, "Error while reading from buffer in 'add_process'!\n");
		exit(-1);
	}
	new_process->remaining_time = new_process->execution_time;
	new_process->PID = find_PID(params->memory);

	new_process->memory_start = find_memory(&params->memory,
											new_process->memory_size,
											new_process->PID);
	if (new_process->memory_start == -1)
	{
		// first attempt
		defrag(&params->memory);
		new_process->memory_start = find_memory(&params->memory,
												new_process->memory_size,
												new_process->PID);
	}

	if (new_process->memory_start == -1)
	{
		// second attempt
		fprintf(params->out_file,
				"Cannot reserve memory for PID %d.\n",
				new_process->PID);
		free_process(new_process);
		return;
	}

	node *new_node = init_node();
	new_node->data = (void *)new_process;
	// if it reached here, it is a success
	if (!params->running)
	{
		// no process running
		params->running = new_node;
	}
	else
	{
		// a process is already running, add this one to the waiting queue
		add_to_waiting(params->waiting_queue, new_node);
	}

	fprintf(params->out_file,
			"Process created successfully: PID: %d, Memory starts at 0x%lx.\n",
			new_process->PID,
			new_process->memory_start);
}

void process_status(main_params *params)
{
	int PID;
	if (sscanf(params->buffer, 	"%*s %d ", &PID) != 1)
	{
		fprintf(stderr,
				"Error while reading from buffer in 'process_status'!\n");
		exit(-1);
	}

	int found = 0;
	process *found_process = NULL;
	// checking running process
	if ((found_process = find_process(params, PID, 4)))
	{
		fprintf(params->out_file,
				"Process %d is running (remaining_time: %ld).\n",
				found_process->PID,
				found_process->remaining_time);
		found = 1;
	}
	// checking waiting processes
	if ((found_process = find_process(params, PID, 2)))
	{
		fprintf(params->out_file,
				"Process %d is waiting (remaining_time: %ld).\n",
				found_process->PID,
				found_process->remaining_time);
		found = 1;
	}
	// checking finished processes
	if ((found_process = find_process(params, PID, 1)))
	{
		found = 1;
	}

	if (!found)
	{
		fprintf(params->out_file, "Process %d not found.\n", PID);
	}
}

void push_to_process(main_params *params)
{
	int PID;
	long int Bdata;
	if (sscanf(params->buffer, "%*s %d %ld ", &PID, &Bdata) != 2)
	{
		fprintf(stderr,
				"Error while reading from buffer in 'push_to_process'!\n");
		exit(-1);
	}

	// searching in running and waiting processes
	process *found_process = find_process(params, PID, 4+2);

	if (found_process)
	{
		if (found_process->memory_occupied >= found_process->memory_size)
		{
			fprintf(params->out_file, "Stack overflow PID %d.\n", PID);
			return;
		}

		node *new_node = init_node();
		new_node->data = (void *)Bdata;

		push(&found_process->stack, new_node);
		found_process->memory_occupied += 4;
		return;
	}

	fprintf(params->out_file, "PID %d not found.\n", PID);
}

void pop_from_process(main_params *params)
{
	int PID;
	if (sscanf(params->buffer, "%*s %d ", &PID) != 1)
	{
		fprintf(stderr,
				"Error while reading from buffer in 'pop_from_process'!\n");
		exit(-1);
	}

	// searching in running and waiting processes
	process *found_process = find_process(params, PID, 4+2);

	if (found_process)
	{
		if (found_process->memory_occupied == 0)
		{
			fprintf(params->out_file, "Empty stack PID %d.\n", PID);
			return;
		}

		node *popped_node = pop(&found_process->stack);
		free(popped_node);
		found_process->memory_occupied -= 4;
		return;
	}

	fprintf(params->out_file, "PID %d not found.\n", PID);
}

void print_stack(main_params *params)
{
	int PID;
	if (sscanf(params->buffer, "%*s %*s %d ", &PID) != 1)
	{
		fprintf(stderr,
				"Error while reading from buffer in 'print_stack'!\n");
		exit(-1);
	}

	// searching in running and waiting processes
	process *found_process = find_process(params, PID, 4+2);

	if (found_process)
	{
		if (found_process->memory_occupied == 0)
		{
			fprintf(params->out_file, "Empty stack PID %d.\n", PID);
			return;
		}

		node *new_stack = NULL;
		while (found_process->stack)
		{
			// flip over the stack
			node *curr_node = pop(&found_process->stack);
			push(&new_stack, curr_node);
		}

		fprintf(params->out_file, "Stack of PID %d:", PID);
		while (new_stack)
		{
			node *curr_node = pop(&new_stack);
			fprintf(params->out_file, " %ld", (long int)(curr_node->data));
			push(&found_process->stack, curr_node);
		}
		fprintf(params->out_file, ".\n");

		return;
	}

	fprintf(params->out_file, "PID %d not found.\n", PID);
}

void print_waiting(main_params *params)
{
	queue *new_queue = init_queue();

	fprintf(params->out_file, "Waiting queue:\n[");
	while (params->waiting_queue->front)
	{
		node *curr_node = dequeue(params->waiting_queue);
		fprintf(params->out_file, 	"(%d: priority = %d, remaining_time = %ld)",
									PPID(curr_node),
									PPRIOR(curr_node),
									PREM(curr_node));
		if (params->waiting_queue->front)
		{
			fprintf(params->out_file, ",\n");
		}
		enqueue(new_queue, curr_node);
	}
	fprintf(params->out_file, "]\n");

	while (new_queue->front)
	{
		// recover the main queue
		node *curr_node = dequeue(new_queue);
		enqueue(params->waiting_queue, curr_node);
	}

	free_queue(new_queue);
}

void print_finished(main_params *params)
{
	queue *new_queue = init_queue();

	fprintf(params->out_file, "Finished queue:\n[");
	while (params->finished_queue->front)
	{
		node *curr_node = dequeue(params->finished_queue);
		fprintf(params->out_file, 	"(%d: priority = %d, executed_time = %ld)",
									PPID(curr_node),
									PPRIOR(curr_node),
									PEXEC(curr_node));
		if (params->finished_queue->front)
		{
			fprintf(params->out_file, ",\n");
		}
		enqueue(new_queue, curr_node);
	}
	fprintf(params->out_file, "]\n");

	while (new_queue->front)
	{
		// recover the main queue
		node *curr_node = dequeue(new_queue);
		enqueue(params->finished_queue, curr_node);
	}

	free_queue(new_queue);
}

void run_jobs(main_params *params)
{
	long int time;
	sscanf(params->buffer, "%*s %ld ", &time);

	do_jobs(params, time);
}

void finish_jobs(main_params *params)
{
	long int total_time = 0;
	while (params->running)
	{
		total_time += do_jobs(params, MAXEXEC);
	}
	fprintf(params->out_file, "Total time: %ld\n", total_time);
}