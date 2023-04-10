#ifndef __WCLIBRARY_H__
#define __WCLIBRARY_H__

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    char **blocks_array;
    size_t total_size;
    size_t current_size;
} BlockArray;

BlockArray *(*BlockArray_initialize)(size_t size);
void (*BlockArray_free)(BlockArray *BlockArray);
void (*BlockArray_destroy)(BlockArray *BlockArray);
char *(*BlockArray_get)(BlockArray *BlockArray, size_t index);
void (*BlockArray_remove)(BlockArray *BlockArray, size_t index);
void (*BlockArray_append)(BlockArray *BlockArray, char *file_name);

#endif