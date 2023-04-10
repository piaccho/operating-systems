
#ifndef __DLLMANAGER_H__
#define __DLLMANAGER_H__

#ifdef USE_DLL
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>


void load_dll_symbols(const char* filepath) {
    void *handle = dlopen(filepath, RTLD_LAZY);

    if (handle == NULL) {
        fprintf(stderr, "DLL not found '%s'\n", filepath);
        return;
    }

    *(void **)(&BlockArray_initialize) = dlsym(handle, "BlockArray_initialize");
    *(void **) (&BlockArray_free) = dlsym(handle,"BlockArray_free");
    *(void **) (&BlockArray_destroy) = dlsym(handle,"BlockArray_destroy");
    *(void **) (&BlockArray_get) = dlsym(handle,"BlockArray_get");
    *(void **) (&BlockArray_remove) = dlsym(handle,"BlockArray_remove");
    *(void **) (&BlockArray_append) = dlsym(handle,"BlockArray_append");
}

#else
void load_dll_symbols(const char* filepath) {}
#endif

#endif