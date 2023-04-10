#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>

#define BUFFER_SIZE 1024

// implementacja przy użyciu funkcji biblioteki C

void copyCharByChar(char *input_filename, char *output_filename) 
{
    size_t file_size;
    char char_to_copy;

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

    // przechodzimy na koniec pliku i pobieramy jego rozmiar w bajtach
    fseek(input_file, 0, SEEK_END);
    file_size = ftell(input_file);

    for (int i = file_size - 1; i >= 0; i--)
    {
        fseek(input_file, i, SEEK_SET);
        char_to_copy = fgetc(input_file);
        if (char_to_copy == EOF)
        {
            exit(1);
        }
        if (fputc(char_to_copy, output_file) == EOF)
        {
            exit(1);
        }
    }

    fclose(input_file);
    fclose(output_file);
}

void reverse_chars(char *buffer, int size)
{
    for (int i = 0; i < size / 2; i++)
    {
        char temp = buffer[i];
        buffer[i] = buffer[size - i - 1];
        buffer[size - i - 1] = temp;
    }
}

void copyBlockByBlock(char *input_filename, char *output_filename) 
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE); 
    long file_size;

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

    // przechodzimy na koniec pliku i pobieramy jego rozmiar w bajtach
    fseek(input_file, 0, SEEK_END);
    file_size = ftell(input_file);

    for (long i = file_size - BUFFER_SIZE; i >= 0; i -= BUFFER_SIZE)
    {
        fseek(input_file, i, SEEK_SET);
        fread(buffer, 1, BUFFER_SIZE, input_file);
        reverse_chars(buffer, BUFFER_SIZE);
        fwrite(buffer, 1, BUFFER_SIZE, output_file);
    }

    if (file_size % BUFFER_SIZE != 0)
    {
        fseek(input_file, 0, SEEK_SET);
        fread(buffer, 1, file_size % BUFFER_SIZE, input_file);
        reverse_chars(buffer, file_size % BUFFER_SIZE);
        fwrite(buffer, 1, file_size % BUFFER_SIZE, output_file);
    }

    fclose(input_file);
    fclose(output_file);
}

int main(int argc, char *argv[])
{
    // sprawdź liczbe argumentów
    if (argc != 3)
    {
        int arg_num = argc - 1;
        printf("Wprowadzono złą liczbe argumentów (%d). Należy podać 2 argumenty:\n", arg_num);
        printf("\t(1) nazwa pliku wejściowego, z którego należy kopiować dane\n\t(2) nazwa pliku wyjściowego, do którego należy przekopiować dane z pliku wejściowego odwrócone bajt po bajcie.\n");
        printf("Np.: ./reverse plik_wejściowy.txt plik_wyjściowy.txt\n");
        exit(1);
    }

    // parsuj argumenty
    char *input_filename = argv[1];
    char *output_filename = argv[2];
    // printf("Argumenty:\n(1): %s\n(2): %s\n", input_filename, output_filename);


    struct timespec start_real_1, end_real_1, start_real_2, end_real_2;
    clock_t start_cpu_1, end_cpu_1, start_cpu_2, end_cpu_2;
    double real_time_1, user_time_1, system_time_1, real_time_2, user_time_2, system_time_2;
    long clock_ticks = sysconf(_SC_CLK_TCK);

        // czytanie znak po znaku i pomiar czasu

    clock_gettime(CLOCK_REALTIME, &start_real_1);
    start_cpu_1 = clock();

    // uruchomienie funkcji
    copyCharByChar(input_filename, output_filename);

    clock_gettime(CLOCK_REALTIME, &end_real_1);
    end_cpu_1 = clock();

    // czas rzeczywisty
    real_time_1 = end_real_1.tv_nsec - start_real_1.tv_nsec;

    // czas systemowy i użytkownika
    user_time_1 = (double)(end_cpu_1 - start_cpu_1) / clock_ticks;
    system_time_1 = (double)(end_cpu_1 - start_cpu_1 - (long)(user_time_1 * clock_ticks)) / clock_ticks;


        // czytanie bloków po 1024 znaki i pomiar czasu

    clock_gettime(CLOCK_REALTIME, &start_real_2);
    start_cpu_2 = clock();

    // uruchomienie funkcji
    copyBlockByBlock(input_filename, output_filename);

    clock_gettime(CLOCK_REALTIME, &end_real_2);
    end_cpu_2 = clock();

    // czas rzeczywisty
    real_time_2 = end_real_2.tv_nsec - start_real_2.tv_nsec;

    // czas systemowy i użytkownika
    user_time_2 = (double)(end_cpu_2 - start_cpu_2) / clock_ticks;
    system_time_2 = (double)(end_cpu_2 - start_cpu_2 - (long)(user_time_2 * clock_ticks)) / clock_ticks;

    // zapis pomiaru czasów do pliku
    FILE *file = fopen("pomiar_zad_2.txt", "w");
    if (file == NULL)
    {
        perror("fopen");
        exit(1);
    }

    fprintf(file, "Implementacja przy użyciu funkcji biblioteki C\n\tCzas rzeczywisty: %lf s\n\tCzas użytkownika: %lf s\n\tCzas systemowy: %lf s\n\nImplementacja przy użyciu funkcji systemowych\n\tCzas rzeczywisty: %lf s\n\tCzas użytkownika: %lf s\n\tCzas systemowy: %lf s\n", real_time_1, user_time_1, system_time_1, real_time_2, user_time_2, system_time_2);

    fclose(file);

    // sprawdzenie działania
    // ./reverse input.txt tmp1 ; ./reverse tmp1 tmp2 ; diff -s tmp2 input.txt

    return 0;
}