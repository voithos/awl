#include "util.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>

#define BUFSIZE 4096

bool streq(char* a, char* b) {
    return strcmp(a, b) == 0;
}

char* get_executable_path() {
    char* path = malloc(BUFSIZE);
    if (readlink("/proc/self/exe", path, BUFSIZE) == -1) {
        free(path);
        path = NULL;
    }
    return path;
}

char* get_base_path() {
    // Executable is in 'bin' directory, so we need to go up twice
    char* exe_path = get_executable_path();
    char* base_path = path_join(dirname(exe_path), "../");
    free(exe_path);
    return base_path;
}

char* path_join(char* a, char* b) {
    char* buffer = malloc(BUFSIZE);
    snprintf(buffer, BUFSIZE, "%s/%s", a, b);
    return buffer;
}
