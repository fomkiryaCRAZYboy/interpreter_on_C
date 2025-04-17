#include "lexer.h"

const char* get_type_of_token(TOKEN_TYPE type) {
    switch (type) {
        case TOKEN_value:           return "VAL";
        case TOKEN_number:          return "NUM";
        case TOKEN_math_operator:   return "MATH_OP";
        case TOKEN_open_paren:      return "OPEN_PAR";
        case TOKEN_close_paren:     return "CLOSE_OP";
        case TOKEN_eof:             return "EOF";
        case TOKEN_assign:          return "ASSIGN";
        case TOKEN_semicolon:       return "SEMICOL";
        case TOKEN_comma:           return "COMMA";
        case TOKEN_space:           return "SPACE";
        case TOKEN_print:           return "PRINT";

        default:                    return "UNKNOWN_TYPE";
    }
}

//функция для вывода сформированного потока токенов
void debug_print_stream(TOKEN stream[]){
    for(int i = 0; i < sizeof(stream) / sizeof(*stream); i++){
        //будет выведен тип токена и само значение. Пример: [NUM](5)
        printf("[%s], (%s)", get_type_of_token(stream[i].type), stream[i].text);
        printf(", ");
    }
}

