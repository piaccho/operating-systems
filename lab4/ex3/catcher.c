#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


int catched_signals = 0;
int mode;

void print_current_time()
{
    time_t current_time = time(NULL);
    struct tm *timeinfo = localtime(&current_time);
    char time_str[9];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);
    printf("%s\n", time_str);
}

void handler(int sig, siginfo_t *sig_info, void *ucontext)
{
    printf("\nOdebrano sygnał od procesu PID %d\n", sig_info->si_pid);
    mode = sig_info->si_value.sival_int;
    printf("\tTryb: %d\n", mode);
    catched_signals++;


    kill(sig_info->si_pid, SIGUSR1);

    switch (mode)
    {
        case 1:
            for (int i = 1; i <= 100; ++i) {
                printf("%d ", i);
            }
            printf("\n");
            break;
        case 2:
            time_t current_time = time(NULL);
            printf("Aktualny czas: %s\n", ctime(&current_time));
            break;
        case 3:
            printf("Dotychczas otrzymano sygnałów: %d\n", catched_signals);
            break;
        case 4:
            while (1)
            {
                print_current_time();
                sleep(1);
            }
            break;
        case 5:
            printf("Zakończenie działania catcher'a\n");
            exit(0);
            break;
        default:
            printf("Nieznany tryb\n");
    }

    
}

int main(int argc, char *argv[])
{
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

    // Nasłuchiwanie sygnałów
    printf("Proces o PID %d oczekuje na sygnały...\n", getpid());

    while (1)
    {
        sleep(2);
    }
    return 0;
}
