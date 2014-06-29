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

char* strrev(char* str) {
    int len = strlen(str);
    char* newstr = malloc(len + 1);

    char* start = newstr;
    char* end = str + len - 1;

    for (int i = 0; i < len; i++) {
        *start = *end;
        start++;
        end--;
    }
    newstr[len] = '\0';
    return newstr;
}

char* strsubstr(char* str, int start, int end) {
    int len = end - start;
    char* buffer = malloc(len + 1);
    memcpy(buffer, &str[start], len);
    buffer[len] = '\0';
    return buffer;
}

char* strstep(char* str, int step) {
    int len = strlen(str);
    int bufferlen = len / step + (len % step == 0 ? 0 : 1);
    char* buffer = malloc(bufferlen + 1);
    for (int i = 0, j = 0; i < bufferlen; i++, j += step) {
        buffer[i] = str[j];
    }
    buffer[bufferlen] = '\0';
    return buffer;
}

char* get_executable_path() {
    /* TODO: reading /proc is definitely not cross platform */
    char* path = malloc(BUFSIZE);

    int len = readlink("/proc/self/exe", path, BUFSIZE);
    if (len == -1) {
        free(path);
        path = NULL;
    } else {
        path[len] = '\0';
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
