#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * infiniteProcesses - Create N child processes that loop forever
 * Used to test scheduling with setGroup/getGroup and ps CPU time.
 *
 * Usage: infiniteProcesses <number_of_children>
 */
int main(int argc, char *argv[])
{
	int i, children;

	if (argc != 2) {
		printf("Incorrect number of parameters.\n");
		printf("Usage: infiniteProcesses <children>\n");
		return 1;
	}

	children = atoi(argv[1]);
	printf("Create %d children.\n", children);

	for (i = 0; i < children; ++i) {
		if (fork() == 0) {
			/* Child: loop forever */
			while (1)
				;
		}
	}

	/* Parent: loop forever */
	while (1)
		;

	return 0;
}