#include "main_header.h"

void *malloc_c(size_t size)
{
	void *p = malloc(size);
	if (!p)
	{
		fprintf(stderr, "Allocation error!\n");
		exit(-1);
	}

	return p;
}

FILE *fopen_c(char *filename, char *mode)
{
	FILE *file = fopen(filename, mode);
	if (!file)
	{
		fprintf(stderr, "File %s could not be opened!\n", filename);
		exit(-1);
	}

	return file;
}

void push(node **astack, node *new_node)
{
	node *stack = *astack;
	if (!stack)
	{
		*astack = new_node;
		return;
	}

	// insert
	new_node->next = stack;
	*astack = new_node;
}

node *pop(node **astack)
{
	node *stack = *astack;
	if (!stack)
	{
		fprintf(stderr, "Empty stack!\n");
		return NULL;
	}

	node *popped_node = stack;
	*astack = stack->next;
	popped_node->next = NULL;

	return popped_node;
}

void enqueue(queue *queue, node *new_node)
{
	if (!queue->front)
	{
		// if empty
		queue->front = new_node;
		queue->rear = new_node;
	}
	else
	{
		queue->rear->next = new_node;
		queue->rear = new_node;
	}
}

node *dequeue(queue *queue)
{
	if (!queue->front)
	{
		fprintf(stderr, "Empty queue!\n");
		return NULL;
	}

	node *dequeued_node = queue->front;
	queue->front = queue->front->next;
	dequeued_node->next = NULL;

	return dequeued_node;
}