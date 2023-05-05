#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

char * FIFO_PATH = "/tmp/myfifo";

void parseArgs(int argc, char *argv[], double *rec_width, int *n) 
{
    if (argc != 3)
    {
        fprintf(stderr, "Invalid arguments number.\nUsage:\n\t%s <rectangle width> <processes number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    *rec_width = atof(argv[1]);
    *n = atoi(argv[2]);

    if (rec_width == 0 || n == 0)
    {
        fprintf(stderr, "Invalid arguments.\nUsage:\n\t%s <rectangle width> <processes number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int iterations = 1 / *rec_width;

    if (*n > iterations)
    {
        fprintf(stderr, "Processes number cannot be greater than iterations number.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    remove(FIFO_PATH);

    double rec_width;
    int n;

    parseArgs(argc, argv, &rec_width, &n);

    struct timespec timespec_start, timespec_end;
    clock_gettime(CLOCK_MONOTONIC, &timespec_start);

    if (mkfifo(FIFO_PATH, 0666) == -1)
    {
        perror("mkfifo error");
        return 1;
    }

    for (int i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            char arg1[32];
            char arg2[32];
            
            double a = (1 / (double)n) * i;
            double b = a + (1 / (double)n);

            sprintf(arg1, "%.6f", a);
            sprintf(arg2, "%.6f", b);
            char *args[] = {"./compute", arg1, arg2, argv[1], NULL};
            execv(args[0], args);
        }
        else if (pid < 0)
        {
            perror("fork error");
            return 1;
        }
    }

    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1)
    {
        perror("open error");
        return 1;
    }

    double total_sum = 0;
    char buffer[256];
    int read_values = 0;
    while (read_values < n)
    {
        size_t size = read(fd, buffer, 256);
        buffer[size] = 0;
        char *token;

        token = strtok(buffer, "\n");
        while(token) {
            total_sum += strtod(token, NULL);
            token = strtok(NULL, "\n");
            read_values++;
        }
    }

    close(fd);
    unlink(FIFO_PATH);

    clock_gettime(CLOCK_MONOTONIC, &timespec_end);
    double real_time = (timespec_end.tv_sec - timespec_start.tv_sec) + (timespec_end.tv_nsec - timespec_start.tv_nsec) / 1e9;

    printf("Value: %lf\nRectangle width: %.9f\nProcesses number: %d\nTime: %lfs\n\n", total_sum, rec_width, n, real_time);

    return 0;
}