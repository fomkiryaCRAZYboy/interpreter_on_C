#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "../parser/parser.h"

typedef enum {
    Success_Executing,
    Failed_Executing,
} EXECUTING_STATUS;

EXECUTING_STATUS execute_assign(TOKEN* left, TOKEN* right);
EXECUTING_STATUS execute_print(TOKEN* arguments, int arguments_count);

void free_variables_array();

#endif