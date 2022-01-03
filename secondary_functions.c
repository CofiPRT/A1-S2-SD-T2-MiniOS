#include "main_header.h"

int find_PID(node *memory)
{
	int PID = 1;
	
	node *curr_node = memory;
	while (curr_node)
	{
		if (PMPID(curr_node) == PID)
		{
			PID++;
			curr_node = memory;
			continue;
		}

		curr_node = curr_node->next;
	}

	return PID;
}

long int find_memory(node **amemory, long int memory_size, int PID)
{
	node *memory = *amemory;
	long int memory_start = 0;

	node *curr_node = memory;
	node *prev_node = NULL;
	while (curr_node)
	{
		if (memory_start < PMSTART(curr_node) &&
				memory_start + memory_size <
				PMSTART(curr_node) + 1)
		{
			// it fits
			break;
		}
		// any other case
		memory_start = PMEND(curr_node) + 1;
		prev_node = curr_node;
		curr_node = curr_node->next;
	}

	if (memory_start + memory_size > MAXMEMORY)
	{
		// not enough memory range at the end
		return -1;
	}

	// if it reached here, add the process
	node *new_node = init_node();

	if (!memory)
	{
		// if empty
		*amemory = new_node;
	}
	else if (!prev_node)
	{
		// insert at the beginning
		new_node->next = curr_node;
		*amemory = new_node;
	}
	else
	{
		new_node->next = prev_node->next;
		prev_node->next = new_node;
	}

	process_mem *new_p_mem = malloc_c(sizeof(process_mem));

	new_p_mem->PID = PID;
	new_p_mem->memory_start = memory_start;
	new_p_mem->memory_end = memory_start + memory_size - 1;

	new_node->data = (void *)new_p_mem;

	return memory_start;
}

void add_to_waiting(queue *main_queue, node *new_node)
{
	queue *new_queue = init_queue();

	int done = 0;

	while (main_queue->front)
	{
		node *curr_node = dequeue(main_queue);

		if (done)
		{
			enqueue(new_queue, curr_node);
			continue;
		}

		done = enqueue_ordered(new_queue, curr_node, new_node);
	}

	if (!done)
	{
		enqueue(new_queue, new_node);
	}

	while (new_queue->front)
	{
		// reupdate the main queue
		node *curr_node = dequeue(new_queue);
		enqueue(main_queue, curr_node);
	}

	free_queue(new_queue);
}

process *find_process(main_params *params, int PID, int level)
{
	/* 	level - search codes (e.g.: 6 = 4+2 means running AND waiting)
			4 - running
			2 - waiting
			1 - finished */
	FILE *f = params->out_file;

	process *searched_process = NULL;
	// checking the running process
	if (level - 4 >= 0)
	{
		level -= 4;
		if (params->running && PPID(params->running) == PID)
		{
			searched_process = (process *)params->running->data;
		}
	}
	// checking the waiting processes
	if (level - 2 >= 0 && !searched_process)
	{
		level -= 2;
		if (!searched_process)
		{
			// not yet found
			searched_process = find_in_queue(params->waiting_queue, PID, 0, f);
		}
	}
	// checking the finished processes
	if (level - 1 >= 0 && !searched_process)
	{
		level -= 1;
		if (!searched_process)
		{
			// still not found
			searched_process = find_in_queue(params->finished_queue, PID, 1, f);
		}
	}

	return searched_process;
}

long int do_jobs(main_params *params, long int time)
{
	long int total_time = 0;
	while (params->running && time)
	{
		PREM(params->running)--;
		time--;
		total_time++;

		if (!PREM(params->running))
		{
			// process done
			erase_from_memory(&params->memory, PSTART(params->running));
			enqueue(params->finished_queue, params->running);

			if (params->waiting_queue->front)
			{
				// next process
				params->running = dequeue(params->waiting_queue);
			}
			else
			{
				// no other processes in the system
				params->running = NULL;
			}
			continue;
		}

		if ((PEXEC(params->running) - PREM(params->running)) &&
			(PEXEC(params->running) - PREM(params->running)) % params->T == 0)
		{
			// the process has been running for T miliseconds
			if (params->waiting_queue->front)
			{
				// there are other processes waiting
				node *aux_node = dequeue(params->waiting_queue);
				// send this process to the waiting queue
				add_to_waiting(params->waiting_queue, params->running);
				// run the new process
				params->running = aux_node;
			}
			/* if there are no other processes, it simply
			continues with the current one */
		}
	}

	return total_time;
}