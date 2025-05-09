#include "../../src/lexer/lexer.h"
#include "lexer_test.h"
#include <stdlib.h>

const char* get_type_of_token(TOKEN_TYPE type) {
    switch (type) {
        case TOKEN_variable:        return "VAR";
        case TOKEN_number:          return "NUM";
        case TOKEN_math_operator:   return "MATH_OP";
        case TOKEN_open_paren:      return "OPEN_PAR";
        case TOKEN_close_paren:     return "CLOSE_PAR";
        case TOKEN_end:             return "END";
        case TOKEN_assign:          return "ASSIGN";
        case TOKEN_semicolon:       return "SEMICOL";
        case TOKEN_comma:           return "COMMA";
        case TOKEN_space:           return "SPACE";
        case TOKEN_print:           return "PRINT";
        case TOKEN_dot:             return "DOT";
        case TOKEN_string:          return "STR";

        default:                    return "UNKNOWN_TYPE";
    }
}

extern int tokens_count;
extern TOKEN stream[250];

//функция для вывода сформированного потока токенов
void debug_print_stream(){
    printf("---------------------\n");
    printf("tokens_count = %d\n", tokens_count);
    printf("---------------------\n");
    for(int i = 0; i < tokens_count; i++){
        //будет выведен тип токена и само значение. Пример: [NUM](5)
        printf("[%s](%s), ", get_type_of_token(stream[i].type), stream[i].text);
    }
}


//очищаю поток ото всех токенов
//освобождаю память занятю текстом токена
void free_stream(){
    for(int i = 0; i < tokens_count; i++) {
        free(stream[i].text);
        stream[i].text = NULL;
        stream[i].type = 0;
    } 
}

