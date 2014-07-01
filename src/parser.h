#ifndef AWL_PARSER_H
#define AWL_PARSER_H

#include <stdbool.h>

#include "types.h"

void setup_parser(void);
void teardown_parser(void);

bool awlval_parse(const char* input, awlval** v, char** err);
bool awlval_parse_file(const char* file, awlval** v, char** err);

#endif
