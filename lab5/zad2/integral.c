#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <regex.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

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

double func(double x) 
{
    return 4 / (x * x + 1);
}

double integral(double a, double b, double rec_width)
{
    double result = 0;
    for (double x = a; x + rec_width <= b; x += rec_width)
    {
        double m = ((x + rec_width) - x) / 2 + x;
        result += rec_width * func(m);
    }
    return result;
}

int main(int argc, char *argv[])
{
    double rec_width;
    int n;

    parseArgs(argc, argv, &rec_width, &n);

    struct timespec timespec_start, timespec_end;
    clock_gettime(CLOCK_MONOTONIC, &timespec_start);

    // pipe for each child process
    int fd[n][2];

    // child processes
    for (int i = 0; i < n; i++)
    {
        pipe(fd[i]);
        pid_t pid = fork();

        if (pid == 0)
        {
            close(fd[i][0]);

            double a = (1 / (double)n) * i;
            double b = a + (1 / (double)n);
            double proc_result = integral(a, b, rec_width);
            // printf("pIndex: %d - <%f, %f> - result: %f\n", i, a, b, proc_result);
            
            write(fd[i][1], &proc_result, sizeof(proc_result));
            
            close(fd[i][1]);
            
            return 0;
        }
        else if (pid < 0)
        {
            perror("fork error");
            return 1;
        }
    }

    // wait till all child processes end
    for (int i = 0; i < n; i++)
    {
        wait(NULL);
    }


    double total_sum;

    // parent process
    for (int i = 0; i < n; i++)
    {
        close(fd[i][1]); 
        double result;
     
        read(fd[i][0], &result, sizeof(result)); 
        total_sum += result;
     
        close(fd[i][0]); 
    }

    clock_gettime(CLOCK_MONOTONIC, &timespec_end);
    double real_time = (timespec_end.tv_sec - timespec_start.tv_sec) + (timespec_end.tv_nsec - timespec_start.tv_nsec) / 1e9;

    printf("Value: %lf\nRectangle width: %.9f\nProcesses number: %d\nTime: %lfs\n\n", total_sum, rec_width, n, real_time);

    return 0;
}