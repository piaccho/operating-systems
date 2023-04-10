#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void sig_handler(int sig_num)
{
    printf("Otrzymano sygnał %d\n", sig_num);
}

int main(int argc, char *argv[])
{
    // Sprawdzanie argumentów
    if (argc > 3 || argc <= 1)
    {
        printf("Nieprawidłowa liczba argumentów:\nMożliwe argumenty:\n\t(1): ignore || handler || mask || pending;\n\t(2): exec\n");
        return 1;
    }

    // Ustawienie dyspozycji dla procesu macierzystego
    if (strcmp(argv[1], "ignore") == 0)
    {
        signal(SIGUSR1, SIG_IGN);
        printf("Ignorowanie SIGUSR1 dla procesu macierzystego\n");
    }
    else if (strcmp(argv[1], "handler") == 0)
    {
        signal(SIGUSR1, sig_handler);
        printf("Instalacja handlera dla SIGUSR1 dla procesu macierzystego\n");
    }
    else if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &mask, NULL);
        printf("Maskowanie SIGUSR1 dla procesu macierzystego\n");
    }
    else
    {
        printf("Nieprawidłowy argument\n");
        return 1;
    }

    raise(SIGUSR1);

    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
    {
        sigset_t new_mask;
        if (sigpending(&new_mask) == -1)
        {
            perror("sigpending");
            exit(EXIT_FAILURE);
        }
        if (sigismember(&new_mask, SIGUSR1))
        {
            printf("SIGUSR1 jest pending w procesie macierzystym\n");
        }
        else
        {
            printf("SIGUSR1 nie jest pending w procesie macierzystym\n");
        }
    }

    if (argc > 2) {
        if (strcmp(argv[2], "exec") == 0)
        {
            execl("./exec", "./exec", argv[1], NULL);
        }
    }
    else
    // Forkowanie i sprawdzanie ustawień dla procesu potomka
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            // Proces potomka
            printf("Proces potomka...\n");
            if (strcmp(argv[1], "pending") != 0) {
                raise(SIGUSR1);
            }
            if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
            {
                sigset_t new_mask;
                sigemptyset(&new_mask);
                if (sigpending(&new_mask) == -1)
                {
                    perror("sigpending");
                    exit(EXIT_FAILURE);
                }
                if (sigismember(&new_mask, SIGUSR1))
                {
                    printf("SIGUSR1 jest widoczny w procesie potomka\n");
                }
                else
                {
                    printf("SIGUSR1 nie jest widoczny w procesie potomka\n");
                }
            }
        }
    }
    return 0;        
}
