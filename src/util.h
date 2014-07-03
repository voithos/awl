#ifndef AWL_UTIL_H
#define AWL_UTIL_H

#include <stdbool.h>

bool streq(const char* a, const char* b);
char* strrev(const char* str);
char* strsubstr(const char* str, int start, int end);
char* strstep(const char* str, int step);
char* get_executable_path(void);
char* get_base_path(void);
char* path_join(const char* a, const char* b);

#endif
