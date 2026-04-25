#include <stdio.h>
#include <stdlib.h>
#include <sys/scheduling.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Incorrect number of parameters.\n");
        return 1;
    }

    int pid = atoi(argv[1]);
    int quanta = getQuantaForProcess(pid);

    if (quanta == -1) {
        printf("Error getting quanta for process %d.\n", pid);
        return 1;
    }

    printf("Process %d has quanta %d\n", pid, quanta);
    return 0;
}