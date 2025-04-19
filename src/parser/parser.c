#include "parser.h"
#include "../lexer/lexer.h"  //для типа TOKEN

#define STACK_SIZE  100 //максимальное количество выражений(структур EXPRESSION) в строке

EXPRESSION* stack[STACK_SIZE];
int expressions_count = 0;


OPERATOR_PRIORITY get_priority(TOKEN token) {
    if(token.type == TOKEN_open_paren || token.type == TOKEN_close_paren) return OPEN_OR_CLOSE_PAREN;

    if(token.type == TOKEN_comma) return COMMA;

    if(token.type == TOKEN_math_operator) {
        if(token.text = "*" || token.text == "/") return MATH_MULT_DIV;
        return MATH_ADD_SUB;
    }

    if(token.type == TOKEN_assign) return ASSIGN;
    if(token.type == TOKEN_semicolon) return SEMICOLON;

    return ANOTHER;
}

PARSING_STATUS parsing(const TOKEN stream[], const int tokens_count){
    int token_idx = 0;
    int priority = -1;

    int perantases_count = 0;
    int comma_count = 0;
    int mult_div_count = 0;
    int add_sub_count = 0;
    int assign_count = 0;
    int semicolon_count = 0;

    for(int i = 0; i < tokens_count - 1; i++) {
        if(stream[token_idx].type == TOKEN_open_paren || stream[token_idx].type == TOKEN_close_paren){
            perantases_count++;
        }
        if(stream[token_idx].type == TOKEN_comma) comma_count++;
        if(stream[token_idx].text == "*" || stream[token_idx].text == "/") mult_div_count++;
        if(stream[token_idx].text == "+" || stream[token_idx].text == "-") add_sub_count++;
        if(stream[token_idx].type == TOKEN_assign) assign_count++;
        if(stream[token_idx].type == TOKEN_semicolon) semicolon_count++;
    }
    
    if(assign_count > 1) {
        printf("ERROR: Too many assignment statements\n");
        return Failed_Parsing;
    }

    if(perantases_count % 2 != 0){
        printf("ERROR: The parentases must be closed\n");
        return Failed_Parsing;
    }

    if(semicolon_count != 1 || stream[tokens_count-1].type != TOKEN_semicolon){
        printf("ERROR: There should be ONE semicolon at the END of the line\n");
        return Failed_Parsing;
    }

    token_idx = 0;
    

    return Successful_Parsing;
}