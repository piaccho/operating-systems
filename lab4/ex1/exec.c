#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(int argc, char **argv)
{
    printf("Funkcja exec\n");
    if (strcmp(argv[1], "ignore") == 0 || strcmp(argv[1], "mask") == 0)
    {
        raise(SIGUSR1);
    }
    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
    {
        sigset_t new_mask;
        sigpending(&new_mask);
        if (sigismember(&new_mask, SIGUSR1))
        {
            printf("SIGUSR1 jest pending w exec\n");
        }
        else
        {
            printf("SIGUSR1 nie jest pending w exec\n");
        }
    }
    return 0;
}