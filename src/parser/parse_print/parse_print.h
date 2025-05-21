#ifndef PARSE_PRINT_H
#define PARSE_PRINT_H

#include "../parser.h"
#include "../../executor/executor.h"

#include <stdio.h>  
#include <stdlib.h>

int parse_print(TOKEN line[], int tokens_count_in_line, int* line_number);


#endif