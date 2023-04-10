#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void handler_siginfo(int sig, siginfo_t *si, void *ucontext)
{
    printf("Sygnał %d odebrany wysłany przez proces o PID %d i o UID %d: ", sig, si->si_pid, si->si_uid);

    switch (sig)
    {
    case SIGINT:
        printf("Otrzymano sygnał SIGINT\n");
        break;
    case SIGTERM:
        printf("Otrzymano sygnał SIGTERM\n");
        break;
    case SIGUSR1:
        printf("Otrzymano sygnał SIGUSR1\n");
        break;
    default:
        printf("Otrzymano nieznany sygnał\n");
    }
}

void handler_resethand(int sig_no)
{
    printf("Otrzymałem sygnał %d.\n", sig_no);
}

void handler_nodefer(int sig_no)
{
    printf("Odliczam 5 sekund...\n");
    sleep(5);
    printf("Koniec odliczania\n");
}

void test_siginfo() 
{
    struct sigaction act;

    act.sa_sigaction = &handler_siginfo;
    act.sa_flags = SA_SIGINFO;

    // Obsługa sygnału SIGINT
    if (sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("Błąd w sigaction dla SIGINT");
        exit(1);
    } else
    {
        printf("Proces obsługuje sygnał SIGINT\n");
    }

    // Obsługa sygnału SIGTERM
    if (sigaction(SIGTERM, &act, NULL) == -1)
    {
        perror("Błąd w sigaction dla SIGTERM");
        exit(1);
    }
    else
    {
        printf("Proces obsługuje sygnał SIGTERM\n");
    }

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

    printf("Proces o PID %d oczekuje na sygnały...\n", getpid());

    while (1)
    {
        sleep(1);
    }
}

void test_resethand()
{
    struct sigaction act;

    act.sa_handler = &handler_resethand;
    act.sa_flags = SA_RESETHAND;

    // Obsługa sygnału SIGTERM
    if (sigaction(SIGTERM, &act, NULL) == -1)
    {
        perror("Błąd w sigaction dla SIGTERM");
        exit(1);
    }
    else
    {
        printf("Proces obsługuje sygnał SIGTERM\n");
    }

    // Nasłuchiwanie sygnałów
    printf("Proces o PID %d oczekuje na sygnały...\n", getpid());

    while (1)
    {
        sleep(1);
    }
}

void test_nodefer()
{
    struct sigaction act;

    act.sa_handler = &handler_nodefer;
    act.sa_flags = SA_NODEFER;

    // Obsługa sygnału SIGINT
    if (sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("Błąd w sigaction dla SIGINT");
        exit(1);
    }
    else
    {
        printf("Proces obsługuje sygnał SIGINT\n");
    }

    // Nasłuchiwanie sygnałów
    printf("Proces o PID %d oczekuje na sygnały...\n", getpid());

    while (1)
    {
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Nieprawidłowa liczba argumentów:\nMożliwe argumenty:\n\t(1): siginfo || resethand || nodefer\n");
        return 1;
    }

    if (strcmp(argv[1], "siginfo") == 0)
    {
        test_siginfo();
    }
    else if (strcmp(argv[1], "resethand") == 0)
    {
        test_resethand();
    }
    else if (strcmp(argv[1], "nodefer") == 0)
    {
        test_nodefer();
    } else {
        printf("Nieprawidłowy argument");
    }

    return 0;
}
