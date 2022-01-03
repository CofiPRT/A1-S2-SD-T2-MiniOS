#include "main_header.h"

node *init_node()
{
	node *new_node = malloc_c(sizeof(node));

	new_node->data = NULL;
	new_node->next = NULL;

	return new_node;
}

queue *init_queue()
{
	queue *new_queue = malloc_c(sizeof(queue));

	new_queue->front = NULL;
	new_queue->rear = NULL;

	return new_queue;
}

void free_queue(queue *queue)
{
	while(queue->front)
	{
		node *node_to_free = dequeue(queue);
		free_process((process *)(node_to_free->data));

		free(node_to_free);
	}

	free(queue);
}

process *init_process()
{
	process *new_process = malloc_c(sizeof(process));

	new_process->stack = NULL;
	new_process->memory_occupied = 0;

	return new_process;
}

void free_process(process *process)
{
	while (process->stack)
	{
		node *node_to_free = pop(&process->stack);
		free(node_to_free);
	}

	free(process);
}

main_params *init_params(char *in_name, char *out_name)
{
	main_params *params = malloc_c(sizeof(main_params));

	params->in_file = fopen_c(in_name, "r");
	params->out_file = fopen_c(out_name, "w");

	params->buffer = malloc_c(MAXCHARS);

	if (fscanf(params->in_file, "%d ", &params->T) != 1)
	{
		fprintf(stderr, "Error while reading 'T' from file!\n");
		exit(-1);
	}

	params->memory = NULL;
	params->waiting_queue = init_queue();
	params->finished_queue = init_queue();
	params->running = NULL;

	return params;
}

void free_params(main_params *params)
{
	if (params->running)
	{
		free_process((process *)(params->running->data));
		free(params->running);
	}

	free_queue(params->waiting_queue);
	free_queue(params->finished_queue);
	free_memory(params->memory);

	free(params->buffer);
	fclose(params->in_file);
	fclose(params->out_file);

	free(params);
}

void free_memory(node *memory)
{
	while (memory)
	{
		free(memory->data);

		node *prev_memory = memory;
		memory = memory->next;

		free(prev_memory);
	}

	free(memory);
}