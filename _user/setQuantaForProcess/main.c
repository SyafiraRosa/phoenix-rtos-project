#include <stdio.h>
#include <stdlib.h>
#include <sys/scheduling.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Incorrect number of parameters.\n");
        return 1;
    }

    int pid = atoi(argv[1]);
    int quanta = atoi(argv[2]);

    if (setQuantaForProcess(pid, quanta) == 0) {
        printf("Error setting quanta for process %d.\n", pid);
        return 1;
    }

    printf("Quanta for process %d set to: %d\n", pid, quanta);
    return 0;
}