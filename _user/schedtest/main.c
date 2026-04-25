#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/scheduling.h>

/*
 * schedtest - Scheduling group verification program
 *
 * Creates 1 process in Group B and 2 processes in Group C,
 * then prints group/pid to demonstrate scheduling order and
 * allow CPU time verification via 'ps'.
 *
 * Expected output pattern (B=2x quanta, C=4x quanta):
 *   B/<pid>  B/<pid>
 *   C1/<pid>  C1/<pid>  C1/<pid>  C1/<pid>
 *   C2/<pid>  C2/<pid>  C2/<pid>  C2/<pid>
 *   B/<pid>  B/<pid>
 *   ...
 *
 * Verify CPU time ratio: C should use ~2x more CPU time than B.
 */
int main(void)
{
	pid_t pid;
	int i;

	printf("=== Scheduling Group Test: 1B + 2C ===\n");
	printf("Main process PID: %d\n\n", getpid());

	for (i = 0; i < 3; i++) {
		pid = fork();
		if (pid == 0) {
			/* Child process */
			pid_t mypid = getpid();
			const char *name;
			int group;

			if (i == 0) {
				/* First child: Group B */
				group = PROCESS_GROUP_B;
				name = "B";
			}
			else {
				/* Second and third child: Group C */
				group = PROCESS_GROUP_C;
				name = (i == 1) ? "C1" : "C2";
			}

			/* Assign scheduling group */
			if (setProcessGroup(mypid, group) == 0) {
				printf("ERROR: setProcessGroup failed for %s (pid %d)\n",
				       name, mypid);
			}
			else {
				printf("Started: Group %s  PID: %d\n", name, mypid);
			}

			/* Busy loop: print group/pid to show scheduling order */
			while (1) {
				printf("Group %s / PID %d\n", name, mypid);

				/* Delay to avoid flooding output completely */
				volatile int j;
				for (j = 0; j < 500000; j++)
					;
			}

			return 0;
		}
	}

	/* Parent: print instructions */
	printf("\nAll children started.\n");
	printf("To verify scheduling:\n");
	printf("  1. Observe 'Group X / PID Y' print order above\n");
	printf("  2. Run 'ps' now to see initial CPU times\n");
	printf("  3. Wait 10-15 seconds\n");
	printf("  4. Run 'ps' again - ratio should be B:C1:C2 ≈ 1:2:2\n\n");

	/* Parent busy-waits */
	while (1)
		;

	return 0;
}
