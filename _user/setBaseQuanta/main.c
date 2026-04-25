#include <stdio.h>
#include <stdlib.h>
#include <sys/scheduling.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Incorrect number of parameters.\n");
        return 1;
    }

    int quanta = atoi(argv[1]);

    if (setBaseQuanta(quanta) == 0) {
        printf("Error setting base quanta.\n");
        return 1;
    }
    else {
        printf("Base quanta set to: %d\n", quanta);
        return 0;
    }
}