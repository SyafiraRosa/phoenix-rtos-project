#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Incorrect number of parameters.\n");
        return 1;
    }

    int children = atoi(argv[1]);

    printf("Create %d children.\n", children);

    for (int i = 0; i < children; ++i) {
        if (fork() == 0) {
            while (1) {
                for (volatile int j = 0; j < 800000; ++j);
            }
        }
    }

    // parent
    while (1) {
        for (volatile int j = 0; j < 200000; ++j);
    }

    return 0;
}