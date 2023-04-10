#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <dirent.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024


int main(int argc, char *argv[])
{
    DIR *dir;
    struct dirent *entry;
    struct stat buffer_stat;
    long long total_size = 0;
    char path[BUFFER_SIZE];

    // otwarcie bieżącego katalogu
    if ((dir = opendir(".")) == NULL)
    {
        perror("opendir");
        return 1;
    }

    // iterowanie po plikach katalogu
    while ((entry = readdir(dir)) != NULL)
    {
        // konstruowanie ścieżki pliku
        snprintf(path, sizeof(path), "./%s", entry->d_name);

        // pobieranie informacji o pliku
        if (stat(path, &buffer_stat) == -1) {
            perror("stat");
            continue;
        }

        // sprawdzenie, czy plik nie jest katalogiem
        if (!S_ISDIR(buffer_stat.st_mode))
        {
            printf("%lld %s\n", (long long)buffer_stat.st_size, entry->d_name);

            total_size += buffer_stat.st_size;
        }
    }

    closedir(dir);

    printf("\n%lld total\n", total_size);
    
    // sprawdzenie działania
    // porównaj z wc --bytes *

    return 0;
}