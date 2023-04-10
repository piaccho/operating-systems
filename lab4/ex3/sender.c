#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>



void handler(int sig, siginfo_t *sig_info, void *ucontext)
{
    printf("Otrzymano odpowiedź od procesu %d\n", sig_info->si_pid);
}

int main(int argc, char *argv[])
{
    // Sprawdzanie argumentów
    if (argc <= 2)
    {
        printf("Nieprawidłowa liczba argumentów. Użycie:\n\t%s catcher_pid mode1 mode2 mode3...\n",argv[0]);
        return 1;
    }

    int catcher_pid = atoi(argv[1]);

    struct sigaction act;

    act.sa_sigaction = &handler;
    act.sa_flags = SA_SIGINFO;

    // Obsługa sygnału SIGUSR1
    if (sigaction(SIGUSR1, &act, NULL) == -1)
    {
        perror("Błąd w sigaction dla SIGUSR1");
        exit(1);
    }
    else
    {
        printf("Proces obsługuje sygnał SIGUSR1\n");
    }

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    printf("Maskowanie SIGUSR1\n\n");

    

    int arg_idx = 2;

    while (arg_idx < argc)
    {
        union sigval value;
        int mode = atoi(argv[arg_idx]);
        value.sival_int = mode;

        printf("Wysłano sygnał SIGUSR1 o trybie %d do procesu %d\n", mode, catcher_pid);
        sigqueue(catcher_pid, SIGUSR1, value);

        sigset_t tmp_mask;
        sigemptyset(&tmp_mask);
        printf("Oczekiwanie na odpowiedź procesu %d\n", catcher_pid);
        sigsuspend(&tmp_mask);

        printf("\n");
        arg_idx++;
    }
    return 0;
}