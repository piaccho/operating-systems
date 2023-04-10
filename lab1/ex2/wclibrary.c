#include "wclibrary.h"

// initialize a BlockArray structure
BlockArray *BlockArray_initialize(size_t size)
{
    BlockArray *NewBlockArray = (BlockArray *)calloc(1, sizeof(BlockArray));
    if (NewBlockArray == NULL)
    {
        fprintf(stderr, "Błąd: Nie udało się zaalokować pamięci.\n");
        exit(EXIT_FAILURE);
    }

    NewBlockArray->blocks_array = (char **)calloc(size, sizeof(char *));
    if (NewBlockArray->blocks_array == NULL)
    {
        fprintf(stderr, "Błąd: Nie udało się zaalokować pamięci.\n");
        exit(EXIT_FAILURE);
    }

    NewBlockArray->total_size = size;
    NewBlockArray->current_size = 0;

    return NewBlockArray;
}

// free memory of BlockArray
void BlockArray_free(BlockArray *BlockArray)
{
    for (size_t i = 0; i < BlockArray->current_size; i++)
    {
        free(BlockArray->blocks_array[i]);
    }
    BlockArray->current_size = 0;
}

// destroy BlockArray
void BlockArray_destroy(BlockArray *BlockArray)
{
    BlockArray_free(BlockArray);
    free(BlockArray->blocks_array);
}

// get data from BlockArray of specific index
char *BlockArray_get(BlockArray *BlockArray, size_t index)
{
    if (index >= BlockArray->total_size)
        return "";
    if (BlockArray->blocks_array[index] != NULL)
        return BlockArray->blocks_array[index];
    return "";
}

// free memory of pointer from BlockArray of specific index
void BlockArray_remove(BlockArray *BlockArray, size_t index)
{
    if (index < BlockArray->total_size)
    {
        if (BlockArray->blocks_array[index] != NULL)
        {
            free(BlockArray->blocks_array[index]);

            // move next elements
            for (size_t i = index; i < BlockArray->total_size - 1; i++)
            {
                BlockArray->blocks_array[i] = BlockArray->blocks_array[i + 1];
            }

            // decrement current size of BlockArray
            BlockArray->current_size--;
        }
    }
}

void BlockArray_append(BlockArray *BlockArray, char *file_name)
{
    // run 'wc' command
    char command[256];
    sprintf(command, "wc '%s' > /tmp/wc.tmp", file_name);
    if (system(command) == -1)
    {
        fprintf(stderr, "Błąd: Nie udało się uruchomić programu wc.\n");
        exit(EXIT_FAILURE);
    }

    // save output to temporary file and get block size
    FILE *tmp_file = fopen("/tmp/wc.tmp", "r");
    if (tmp_file == NULL)
    {
        fprintf(stderr, "Błąd: Nie udało się otworzyć pliku tymczasowego.\n");
        exit(EXIT_FAILURE);
    }

    // check block size for allocating by checking position of cursor
    fseek(tmp_file, 0, SEEK_END);
    size_t size_of_block = ftell(tmp_file);
    fclose(tmp_file);

    // allocate block and put data from tmp into it
    FILE *input_file = fopen("/tmp/wc.tmp", "r");
    if (input_file == NULL)
    {
        fprintf(stderr, "Błąd: Nie udało się otworzyć pliku tymczasowego.\n");
        exit(EXIT_FAILURE);
    }
    size_t size = size_of_block;
    char *buffer = calloc(size, sizeof(char));
    fread(buffer, sizeof(char), size, input_file);
    fclose(input_file);

    if (BlockArray->current_size < BlockArray->total_size)
    {
        int current_index = BlockArray->current_size;
        BlockArray->blocks_array[current_index] = buffer;
        // increment current size of BlockArray
        BlockArray->current_size++;
    }
    else
    {
        fprintf(stderr, "Błąd: Za mało pamięci\n");
    }

    // delete temporary file
    remove("/tmp/wc.tmp");
}
