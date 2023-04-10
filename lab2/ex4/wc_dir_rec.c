#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <dirent.h>
#include <sys/stat.h>
#include <ftw.h>
#include <unistd.h>

long long total_size = 0;

// ścieżka do katalogu, struktura statystyk pliku, flaga, która informuje, w jakim kontekście jest wywoływana funkcja (np. do czytania czy do zapisywania)
static int process_file(const char *path, const struct stat *buffer_stat, int flag)
{
    // sprawdzenie, czy plik nie jest katalogiem
    if (!S_ISDIR(buffer_stat->st_mode))
    {
        printf("%lld %s\n", (long long)buffer_stat->st_size, path);

        total_size += buffer_stat->st_size;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    // sprawdzanie liczby argumentów
    if (argc != 2)
    {
        int arg_num = argc - 1;
        printf("Wprowadzono złą liczbe argumentów (%d). Należy podać 1 argument:\n", arg_num);
        printf("\t(1) scieżka do katalogu\n");
        exit(1);
    }

    char *dir_path = argv[1];

    // wywołanie funkcji ftw() z przeszukiwaniem katalogów w głąb
    // scieżka katalogu, callback, głębokość drzewa
    if (ftw(dir_path, process_file, 0) == -1)
    {
        perror("ftw");
        exit(1);
    }

    // wypisanie sumarycznego rozmiaru plików
    printf("Total size: %lld\n", total_size);

    
    // sprawdzenie działania
    // porównaj z:
    //  find nazwa_katalogu | xargs wc --bytes

    return 0;
}