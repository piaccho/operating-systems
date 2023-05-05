#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

char *FIFO_PATH = "/tmp/myfifo";

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
    double a = atof(argv[1]);   
    double b = atof(argv[2]);
    double rec_width = atof(argv[3]);

    double result = integral(a, b, rec_width);

    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("open error");
        return 1;
    }

    char buffer[256];
    size_t size = snprintf(buffer, 256, "%lf\n", result);

    if (write(fd, buffer, size) == -1)
    {
        perror("write error");
        return 1;
    }
    close(fd);

    return 0;
}