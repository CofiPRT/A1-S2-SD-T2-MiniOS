#include "main_header.h"

process *find_in_queue(queue *main_queue, int PID, int is_finished, FILE *f)
{
	/* 	is_finished - given queue is finished_queue?
			1 - yes
			0 - no
			(in order print all matching PIDs) */

	process *searched_process = NULL;
	queue *new_queue = init_queue();

	while (main_queue->front)
	{
		node *curr_node = dequeue(main_queue);

		if (PPID(curr_node) == PID)
		{
			searched_process = (process *)(curr_node->data);
			if (is_finished)
			{
				fprintf(f, "Process %d is finished.\n", searched_process->PID);
			}
		}
		enqueue(new_queue, curr_node);
	}

	while (new_queue->front)
	{
		node *curr_node = dequeue(new_queue);

		enqueue(main_queue, curr_node);
	}
	free_queue(new_queue);

	return searched_process;
}

void defrag(node **amemory)
{
	node *memory = *amemory;
	node *aux_memory = NULL;
	node *aux_curr_node = NULL;

	long int curr_memory_start = 0;

	while (memory)
	{
		// find the process with the lowest PID
		int min_PID = MAXINT;
		node *min_node = NULL;
		node *min_prev_node = NULL; // for linking

		node *curr_node = memory;
		node *prev_node = NULL;
		while (curr_node)
		{
			if (PMPID(curr_node) < min_PID)
			{
				min_PID = PMPID(curr_node);
				min_prev_node = prev_node; // for linking
				min_node = curr_node;
			}
			prev_node = curr_node;
			curr_node = curr_node->next;
		}

		if (!min_prev_node)
		{
			// it was the first node of the list
			memory = min_node->next;
		}
		else
		{
			// it wasn't, link the adjacent nodes
			min_prev_node->next = min_node->next;
		}

		// MOVE the found process to the new memory storage
		if (!aux_memory)
		{
			// if empty
			aux_memory = min_node;
			aux_curr_node = aux_memory;
		}
		else
		{
			// not empty
			aux_curr_node->next = min_node;
			aux_curr_node = min_node;
		}
		min_node->next = NULL;

		// change the memory ranges
		PMEND(min_node) -= PMSTART(min_node) - curr_memory_start;

		PMSTART(min_node) = curr_memory_start;
		curr_memory_start = PMEND(min_node) + 1;
	}

	// update the memory that is being worked with
	*amemory = aux_memory;
}

void erase_from_memory(node **amemory, long int memory_start)
{
	node *memory = *amemory;

	node *curr_node = memory;
	node *prev_node = NULL;

	while (curr_node)
	{
		if (PMSTART(curr_node) == memory_start)
		{
			if (!prev_node)
			{
				// first node
				*amemory = curr_node->next;
				free(curr_node->data);
				free(curr_node);
			}
			else
			{
				prev_node->next = curr_node->next;
				free(curr_node->data);
				free(curr_node);
			}
			break;
		}
		prev_node = curr_node;
		curr_node = curr_node->next;
	}
}

int enqueue_ordered(queue *new_queue, node *curr_node, node *new_node)
{
	int done = 0;

	// highest priority
	if (PPRIOR(curr_node) > PPRIOR(new_node))
	{
		enqueue(new_queue, curr_node);
	}
	else if (PPRIOR(curr_node) < PPRIOR(new_node))
	{
		enqueue(new_queue, new_node);
		enqueue(new_queue, curr_node);
		done = 1;
	}
	else
	{
		// lowest remaining_time
		if (PREM(curr_node) < PREM(new_node))
		{
			enqueue(new_queue, curr_node);
		}
		else if (PREM(curr_node) > PREM(new_node))
		{
			enqueue(new_queue, new_node);
			enqueue(new_queue, curr_node);
			done = 1;
		}
		else
		{
			// lowest PID
			if (PPID(curr_node) < PPID(new_node))
			{
				enqueue(new_queue, curr_node);
			}
			else
			{
				enqueue(new_queue, new_node);
				enqueue(new_queue, curr_node);
				done = 1;
			}
		}
	}

	return done;
}