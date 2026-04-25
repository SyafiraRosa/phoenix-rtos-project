/*
 * Last modified by Syafira
 */
 
#include <stdio.h>
#include <unistd.h>
#include <sys/maxchildreninranges.h>

int main(void)
{
    int pid = -1;
    int count = -1;

    printf("Creating child processes...\n");

    // Parent will create 3 children
    for (int i = 0; i < 3; i++)
    {
        if (fork() == 0)
        {
            // Each child creates 2 children
            for (int j = 0; j < 2; j++)
            {
                if (fork() == 0)
                {
                    while (1)
                    {
                        // keep child processes alive
                    }
                }
            }

            while (1)
            {
                // keep child alive
            }
        }
    }

    // Give time for all child processes to be created
    sleep(1);

    printf("Main process PID: %d\n", getpid());

    // ===== TEST CASES =====

    printf("\n=== Test 1: Full range (0–1000) ===\n");
    if (maxChildrenInRanges(0, 1000, &pid, &count) == 0)
    {
        printf("PID: %d, Children: %d\n", pid, count);
    }    
    else
    {
        printf("No process found\n");
    }

    printf("\n=== Test 2: Small range (0–10) ===\n");
    if (maxChildrenInRanges(0, 10, &pid, &count) == 0)
    {
        printf("PID: %d, Children: %d\n", pid, count);
    }
    else
    {
        printf("No process found\n");
    }

    printf("\n=== Test 3: Range (2–3 children) ===\n");
    if (maxChildrenInRanges(2, 3, &pid, &count) == 0)
    {
        printf("PID: %d, Children: %d\n", pid, count);
    }
    else
    {
        printf("No process found\n");
    }

    printf("\n=== Test 4: Leaf processes (0 children) ===\n");
    if (maxChildrenInRanges(0, 0, &pid, &count) == 0)
    {
        printf("PID: %d, Children: %d\n", pid, count);
    }
    else
    {
        printf("No process found (expected)\n");
    }

    printf("\n=== Test 5: Exact children count (2 children) ===\n");

    if (maxChildrenInRanges(2, 2, &pid, &count) == 0)
    {
        printf("PID: %d, Children: %d\n", pid, count);
    }
    else
    {
        printf("No process found\n");
    }

    printf("\n=== Test 6: Invalid range (50–10) ===\n");
    if (maxChildrenInRanges(50, 10, &pid, &count) == 0)
    {
        printf("PID: %d, Children: %d\n", pid, count);
    }
    else
    {
        printf("No process found (invalid range)\n");
    }
    
    printf("\n=== Test 7: No process with large children count ===\n");
    if (maxChildrenInRanges(999, 1000, &pid, &count) == 0)
    {
        printf("PID: %d, Children: %d\n", pid, count);
    }
    else
    {
        printf("No process found\n");
    }
    
    return 0;
}