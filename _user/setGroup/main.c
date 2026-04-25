#include <stdio.h>
#include <stdlib.h>
#include <sys/scheduling.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: setGroup <pid> <group>\n");
		printf("  group: 0=A (default), 1=B (2x quanta), 2=C (4x quanta)\n");
		return 1;
	}

	int pid = atoi(argv[1]);
	int group = atoi(argv[2]);

	if (setProcessGroup(pid, group) == 0) {
		printf("Error setting group for process %d.\n", pid);
		return 1;
	}

	const char *names[] = {"A", "B", "C"};
	printf("Process %d set to Group %s\n", pid, names[group]);
	return 0;
}
