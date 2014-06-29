#ifndef AWL_UTIL_H
#define AWL_UTIL_H

#include <stdbool.h>

bool streq(char* a, char* b);
char* strrev(char* str);
char* strsubstr(char* str, int start, int end);
char* strstep(char* str, int step);
char* get_executable_path();
char* get_base_path();
char* path_join(char* a, char* b);

#endif
