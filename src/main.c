#include "lexer/lexer.h"
#include "../tests/lexer_tests/lexer_test.h"
#include "executor/executor.h"

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
            printf("\n");
            break;
        }
        
        if(strstr(input, "end;") != NULL) {
            strcat(program, input);
            break;
        }
        
        strcat(program, input);

        if(strlen(program) >= MAX_PROGRAM_SIZE - MAX_INPUT_SIZE) {
            printf("Program too large!\n");
            return 1;
        }
    }*/

    char program[MAX_PROGRAM_SIZE] = "string = \"hello \";if(1) {print(1);if(1){x = 14.07;print(2);if(1){print(3);print(4);}print(5);}print(6);print(x);}string = \"\";if(string) {print(string);} print(\"after if\");end;";
    // Токенизация и выполнение всей программы
    if(tokenize(program) != Successful_Tokenization) {
        printf("Tokenization error\n");
        return 1;
    }

    //debug_print_stream();
    
    printf("Output:\n");
    puts("---------------------------------------------");
    
    if(parsing(stream, tokens_count) != Successful_Parsing) {
        printf("Parsing Failed\n");
        return 1;
    }
    puts("\n---------------------------------------------");

    free_stream();
    free_variables_array();

    return 0;   
}

