#include <stdio.h>
#include <sys/scheduling.h>

int main(int argc, char *argv[])
{
    int quanta = getBaseQuanta();
    printf("Base quanta: %d\n", quanta);
    return 0;
}
