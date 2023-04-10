#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Uruchamianie: %s <ścieżka_katalogu>\n", argv[0]);
        exit(1);
    }

    pid_t child_pid;
    child_pid = fork();
    if (child_pid == 0)
    {
        printf("%s ", argv[0]);
        fflush(stdout);
        execl("/bin/ls", "ls", "-l", argv[1], NULL);
    }

    return 0;
}
