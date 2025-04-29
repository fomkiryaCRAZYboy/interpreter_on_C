#include "lexer/lexer.h"
#include "../tests/lexer_tests/lexer_test.h"
#include "../tests/parser_tests/parser_test.h"

extern TOKEN stream[];
extern int tokens_count;

extern VARIABLE variables_array[MAX_VARIABLES_COUNT];
extern int variables_count;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INPUT_SIZE 1024
#define MAX_PROGRAM_SIZE 65536

int main(void)
{
    /*char input[MAX_INPUT_SIZE];
    char program[MAX_PROGRAM_SIZE] = {0};
    printf("Welcome to interpreter! Type 'end;' to finish input.\n");

    while(1) {
        printf("> ");
        
        if(!fgets(input, MAX_INPUT_SIZE, stdin)) {
            printf("\n"); // Для красоты при Ctrl+D
            break;
        }
        
        // Проверка на команду завершения ввода
        if(strstr(input, "end;") != NULL) {
            strcat(program, input);
            break;
        }
        
        // Добавляем введенную строку к программе
        strcat(program, input);
        
        // Проверка на переполнение буфера программы
        if(strlen(program) >= MAX_PROGRAM_SIZE - MAX_INPUT_SIZE) {
            printf("Program too large!\n");
            return 1;
        }
    }*/
    
    // Токенизация и выполнение всей программы
    if(tokenize("\"hello344@#@$)_+9/.y\"") != Successful_Tokenization) {
        printf("Tokenization error\n");
        return 1;
    }

    debug_print_stream();
    
    /*printf("Output:\n");
    puts("---------------------------------------------");
    
    if(parsing(stream, tokens_count) != Successful_Parsing) {
        printf("Parsing Failed\n");
        return 1;
    }
    puts("\n---------------------------------------------");*/

    free_stream();

    return 0;
    
}

