#include <stdio.h>
#include <stdlib.h>
#include <sys/scheduling.h>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: getGroup <pid>\n");
		return 1;
	}

	int pid = atoi(argv[1]);
	int group = getProcessGroup(pid);

	if (group < 0) {
		printf("Error: process %d not found.\n", pid);
		return 1;
	}

	const char *names[] = {"A", "B", "C"};
	printf("Process %d is in Group %s\n", pid, names[group]);
	return 0;
}
