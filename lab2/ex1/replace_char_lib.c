#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

// zamiana charów
// implementacja z użyciem funkcji biblioteki C
void cLibReplace(char char_to_replace, char replacing_char, char *input_filename, char *output_filename)
{
    char buffer[BUFFER_SIZE];
    size_t read_elements_number;

    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL)
    {
        printf("Nie udało się otworzyć pliku %s\n", input_filename);
        exit(1);
    }

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL)
    {
        printf("Nie udało się otworzyć pliku %s\n", output_filename);
        exit(1);
    }

    while ((read_elements_number = fread(buffer, 1, BUFFER_SIZE, input_file)) > 0)
    {
        for (int i = 0; i < read_elements_number; i++)
        {
            if (buffer[i] == char_to_replace)
            {
                buffer[i] = replacing_char;
            }
        }
        fwrite(buffer, 1, read_elements_number, output_file);
    }

    fclose(input_file);
    fclose(output_file);
}

// zamiana charów
// implementacja z użyciem funkcji systemowych
void sysReplace(char char_to_replace, char replacing_char, char *input_filename, char *output_filename) 
{
    int input_file, output_file;
    char buffer[BUFFER_SIZE];
    size_t read_elements_number;

    input_file = open(input_filename, O_RDONLY);
    if (input_file == -1)
    {
        printf("Nie udało się otworzyć pliku %s\n", input_filename);
        exit(1);
    }
    output_file = open(output_filename, O_WRONLY | O_CREAT);
    if (input_file == -1)
    {
        printf("Nie udało się otworzyć pliku %s\n", output_filename);
        exit(1);
    }

    while ((read_elements_number = read(input_file, buffer, BUFFER_SIZE)) > 0)
    {
        for (int i = 0; i < read_elements_number; i++)
        {
            if (buffer[i] == char_to_replace)
            {
                buffer[i] = replacing_char;
            }
        }
        write(output_file, buffer, read_elements_number);
    }
    
    close(input_file);
    close(output_file);
}

    int main(int argc, char *argv[])
{
    // sprawdź liczbe argumentów
    if (argc != 5)
    {
        int arg_num = argc - 1;
        printf("Wprowadzono złą liczbe argumentów (%d). Należy podać 4 argumenty:\n", arg_num);
        printf("\t(1) znak ASCII, który należy znaleźć w pliku wejściowym\n\t(2) znak ASCII, na który należy zamienić wszystkie wystąpienia pierwszego argumentu\n\t(3) nazwa pliku wejściowego, w którym należy znaleźć pierwszy argument\n\t(4) nazwa pliku wyjściowego, do którego należy zapisać zawartość pliku wejściowego z zamienionym znakami argv[1] na argv[2].\n");
        printf("Np.: ./zamien a b plik_wejściowy.txt plik_wyjściowy.txt\n");
        exit(1);
    }

    // sprawdź poprawność argumentów
    bool valid_args = true;
    
    if (strlen(argv[1]) != 1) {
        fprintf(stderr, "Argument (1) nie poprawny. Powinien być char.\n");
        valid_args = false;
    }
    if (strlen(argv[2]) != 1)
    {
        fprintf(stderr, "Argument (2) nie poprawny. Powinien być char.\n");
        valid_args = false;
    }
    if(!valid_args)
        exit(EXIT_FAILURE);

    // parsuj argumenty
    char char_to_replace = argv[1][0];
    char replacing_char = argv[2][0];
    char *input_filename = argv[3];
    char *output_filename = argv[4];
    // printf("Argumenty:\n(1): %c\n(2): %c\n(3): %s\n(4): %s\n", char_to_replace, replacing_char, input_filename, output_filename);

    struct timespec start_real_1, end_real_1, start_real_2, end_real_2;
    clock_t start_cpu_1, end_cpu_1, start_cpu_2, end_cpu_2;
    double real_time_1, user_time_1, system_time_1, real_time_2, user_time_2, system_time_2;
    long clock_ticks = sysconf(_SC_CLK_TCK);

        // implementacja z użyciem funkcji biblioteki C i pomiar czasu

    clock_gettime(CLOCK_REALTIME, &start_real_1);
    start_cpu_1 = clock();

    // uruchomienie funkcji
    cLibReplace(char_to_replace, replacing_char, input_filename, output_filename);

    clock_gettime(CLOCK_REALTIME, &end_real_1);
    end_cpu_1 = clock();
    
    // czas rzeczywisty
    real_time_1 = end_real_1.tv_nsec - start_real_1.tv_nsec;

    // czas systemowy i użytkownika
    user_time_1 = (double)(end_cpu_1 - start_cpu_1) / clock_ticks;
    system_time_1 = (double)(end_cpu_1 - start_cpu_1 - (long)(user_time_1 * clock_ticks)) / clock_ticks;

        // implementacja z użyciem funkcji systemowych i pomiar czasu

    clock_gettime(CLOCK_REALTIME, &start_real_2);
    start_cpu_2 = clock();

    // uruchomienie funkcji
    sysReplace(char_to_replace, replacing_char, input_filename, output_filename);

    clock_gettime(CLOCK_REALTIME, &end_real_2);
    end_cpu_2 = clock();

    // czas rzeczywisty
    real_time_2 = end_real_2.tv_nsec - start_real_2.tv_nsec;

    // czas systemowy i użytkownika
    user_time_2 = (double)(end_cpu_2 - start_cpu_2) / clock_ticks;
    system_time_2 = (double)(end_cpu_2 - start_cpu_2 - (long)(user_time_2 * clock_ticks)) / clock_ticks;


    // zapis pomiaru czasów do pliku
    FILE *file = fopen("pomiar_zad_1.txt", "w");
    if (file == NULL)
    {
        perror("fopen");
        exit(1);
    }

    fprintf(file, "Implementacja przy użyciu funkcji biblioteki C\n\tCzas rzeczywisty: %lf s\n\tCzas użytkownika: %lf s\n\tCzas systemowy: %lf s\n\nImplementacja przy użyciu funkcji systemowych\n\tCzas rzeczywisty: %lf s\n\tCzas użytkownika: %lf s\n\tCzas systemowy: %lf s\n", real_time_1, user_time_1, system_time_1, real_time_2, user_time_2, system_time_2);

    fclose(file);

        // Porównanie

        // ./zamien [znak1] [znak2] plik_wejściowy plik_wyjściowy ; tr [znak1] [znak2] < plik_wejściowy > tmp ; diff -s tmp plik_wyjściowy

        return 0;
}