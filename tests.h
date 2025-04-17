#ifndef __TESTS_H__
#define __TESTS_H__

#include "lexer.h"
#include <stdio.h>

const char* get_type_of_token(TOKEN_TYPE type);

//функция для вывода сформированного потока токенов
void debug_print_stream(TOKEN stream[]);

#endif //__TESTS_H__