#ifdef USE_DLL
    #include "wclibrary_so.h"
#else
    #include "wclibrary.h"
#endif

#include "dllmanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

// KODY KOMEND
    // 0 - wrong command
    // 1 - init size — stworzenie tablicy o rozmiarze size (int)
    // 2 - count file — zliczenie słów i linii w pliku file zapis wyniku w tablicy
    // 3 - show index – wyświetlenie zawartości tablicy o indeksie index 
    // 4 - delete index — usunięcie z tablicy bloków o indeksie index
    // 5 - destroy – usunięcie z pamięci tablicy z pkt. 1
    // 6 - quit - wyjscie
void parse_command(char *input, int *command_code, size_t *command_num_param, char* command_str_param)
{
    // check which command prompted
    // init 'size'
    if (sscanf(input, "init %zu", command_num_param) == 1)
    {
        *command_code = 1;
    }
    // count 'file_name'
    else if (sscanf(input, "count %s", command_str_param) == 1)
    {
        *command_code = 2;
    }
    // show 'index'
    else if (sscanf(input, "show %zu", command_num_param) == 1)
    {
        *command_code = 3;
    }
    // delete 'index'
    else if (sscanf(input, "delete %zu", command_num_param) == 1)
    {
        *command_code = 4;
    }
    // destroy
    else if (strcmp(input, "destroy\n") == 0)
    {
        *command_code = 5;
    }
    // quit
    else if (strcmp(input, "quit\n") == 0)
    {
        *command_code = 6;
    }
    // wrong command
    else
    {
        *command_code = 0;
    }
}

int main(int argc, char **argv)
{
    load_dll_symbols("wclibrary.so");
    BlockArray *BlockArray = NULL;
    bool is_block_array_init = false;

    int command_code = 0;
    size_t command_num_param = 0;
    char command_str_param[] = "";

    struct timespec start_real, end_real;
    clock_t start_cpu, end_cpu;
    double real_time, user_time, system_time;

    while (1) {
        printf("> ");

        char input[256];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "Błąd odczytu danych\n");
            exit(EXIT_FAILURE);
        }

        parse_command(input, &command_code, &command_num_param, command_str_param);

        clock_gettime(CLOCK_REALTIME, &start_real);
        start_cpu = clock();

        switch (command_code)
        {
        case 1:
            if (is_block_array_init) {
                fprintf(stderr, "Tablica już została zainicjowana. Prosze użyć komendy 'destroy' i zainicjować nową tablice\n");
            } else {
                BlockArray = BlockArray_initialize(command_num_param);
                is_block_array_init = true;
            }
            break;
        case 2:
            if (is_block_array_init)
            {
                BlockArray_append(BlockArray, command_str_param);
            }
            else
            {
                fprintf(stderr, "Tablica nie została zainicjowana. Prosze użyć komendy 'init' i zainicjować nową tablice\n");
            }
            break;
        case 3:
            if (is_block_array_init)
            {
                printf("%s", BlockArray_get(BlockArray, command_num_param));
            }
            else
            {
                fprintf(stderr, "Tablica nie została zainicjowana. Prosze użyć komendy 'init' i zainicjować nową tablice\n");
            }
            break;
        case 4:
            if (is_block_array_init)
            {
                BlockArray_remove(BlockArray, command_num_param);
            }
            else
            {
                fprintf(stderr, "Tablica nie została zainicjowana. Prosze użyć komendy 'init' i zainicjować nową tablice\n");
            }
            break;
        case 5:
            if (is_block_array_init)
            {
                BlockArray_destroy(BlockArray);
                is_block_array_init = false;
            }
            else
            {
                fprintf(stderr, "Tablica nie została zainicjowana. Prosze użyć komendy 'init' i zainicjować nową tablice\n");
            }
            break;
        case 6:
            return 0;
            break;
        default:
            fprintf(stderr, "Niepoprawna komenda\n");
            break;
        }
        
        clock_gettime(CLOCK_REALTIME, &end_real);
        end_cpu = clock();

        // Compute real time
        real_time = (double)(end_real.tv_sec - start_real.tv_sec) +
                    (double)(end_real.tv_nsec - start_real.tv_nsec) / 1000000000.0;
        real_time = end_real.tv_nsec - start_real.tv_nsec;

        // Compute user time and CPU time
        long clock_ticks = sysconf(_SC_CLK_TCK);
        user_time = (double)(end_cpu - start_cpu) / clock_ticks;
        system_time = (double)(end_cpu - start_cpu - (long)(user_time * clock_ticks)) / clock_ticks;

        // Print process times
        printf("Czas rzeczywisty: %lf s\n", real_time);
        printf("Czas użytkownika: %lf s\n", user_time);
        printf("Czas systemowy: %lf s\n", system_time);
    }

    return 0;
}
