#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>z
#include <limits.h>

void find_rec(char *dir_path, char *search_string) 
{
    // printf("CURRENT DIR: %s\n", dir_path);
    DIR *dir;
    struct dirent *entry;
    struct stat buffer_stat;

    // otwarcie bieżącego katalogu
    if ((dir = opendir(dir_path)) == NULL)
    {
        perror("opendir");
        exit(1);
    }

    // iterowanie po plikach katalogu
    while ((entry = readdir(dir)) != NULL)
    {
        char *curr_filename = entry->d_name;
        if (strcmp(curr_filename, ".") == 0 || strcmp(curr_filename, "..") == 0)
        {
            continue;
        }

        // konstruowanie ścieżki pliku
        char curr_path[PATH_MAX];
        snprintf(curr_path, sizeof(curr_path), "%s/%s", dir_path, curr_filename);
        // printf("current file to check: %s\n", curr_path);

        // pobieranie informacji o pliku
        if (stat(curr_path, &buffer_stat) == -1)
        {
            perror("stat");
            continue;
        }

        // sprawdzenie, czy plik jest katalogiem
        if (S_ISDIR(buffer_stat.st_mode))
        {
            pid_t pid = fork();

            if (pid == -1) {
                perror("Nie udało się zforkować procesu");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                find_rec(curr_path, search_string);
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            // jeśli plik nie jest katalogiem sprawdź zawartość
            FILE *file_to_check = fopen(curr_path, "r");
            if (file_to_check == NULL)
            {
                perror("Błąd otwarcia pliku");
                printf(" %s\n", curr_path);
                // exit(1);
                continue;
            }

            // porównaj szukany string z zawartością pliku
            char string_to_check[strlen(search_string)];
            fgets(string_to_check, strlen(search_string) + 1, file_to_check); 
            if (strcmp(string_to_check, search_string) == 0) 
            {
                printf("%s PID:%d\n", curr_path, getpid());
            }

            // char *string_to_check = NULL;
            // size_t len = 0;
            // ssize_t read;
            // read = getline(&string_to_check, &len, file_to_check);
            // if (read != -1 && strncmp(string_to_check, search_string, strlen(search_string)) == 0)
            // {
            //     printf("%s PID:%d\n", curr_path, getpid());
            // }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Uruchamianie: %s <ścieżka_katalogu> <łańcuch_znaków>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    find_rec(argv[1], argv[2]);
    return 0;
}


// dokleić i parsować odpowiednio ścieżki wzgledem odpalonej lokalizacji przy czytaniu Dirów i plików