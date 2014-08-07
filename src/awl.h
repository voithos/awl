#ifndef AWL_H
#define AWL_H

#define AWL_VERSION "v0.2.1"

#include "types.h"

/* system functions */
void run_scripts(awlenv* e, int argc, char** argv);
void setup_awl(void);
void teardown_awl(void);
char* get_awl_version(void);

#endif
