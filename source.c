#include "main_header.h"

void (*find_function(char *buffer))(main_params *)
{
	function_map map[] = {
		{"add", add_process},
		{"get", process_status},
		{"push", push_to_process},
		{"pop", pop_from_process},
		{"print stack", print_stack},
		{"print waiting", print_waiting},
		{"print finished", print_finished},
		{"run", run_jobs},
		{"finish", finish_jobs}
	};

	char *operation = malloc_c(MAXCHARS);
	sscanf(buffer, "%s ", operation); // first string of the command

	if (!strcmp(operation, "print"))
	{
		char *print_mode = malloc_c(MAXCHARS);
		sscanf(buffer, "%*s %s ", print_mode);

		// update the operation name
		strcat(operation, " ");
		strcat(operation, print_mode);

		free(print_mode);
	}

	int i;
	for (i = 0; i < MAXFUNCTIONS; i++)
	{
		if (!strcmp(map[i].fname, operation))
		{
			free(operation);
			return map[i].f;
		}
	}

	// should not reach here
	fprintf(stderr, "Operation '%s' not found!\n", operation);
	free(operation);
	return NULL;
}

int main(int argc, char *argv[])
{
	(void)argc; // suppress the warning

	main_params *params = init_params(argv[1], argv[2]);

	while (fgets(params->buffer, MAXCHARS, params->in_file))
	{
		find_function(params->buffer)(params);
	}

	free_params(params);
	return 0;
}