#ifndef TESTS_H
#define TESTS_H

#include "../../src/lexer/lexer.h"
#include <stdio.h>

const char* get_type_of_token(TOKEN_TYPE type);

//функция для вывода сформированного потока токенов
void debug_print_stream();

void free_stream();

#endif // TESTS_H