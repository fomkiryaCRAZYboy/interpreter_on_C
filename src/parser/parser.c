#include "parser.h"
#include "../lexer/lexer.h"  //для типа TOKEN

#include <stdio.h>
#include <string.h>

#define STACK_SIZE  1000 //максимальное количество выражений(структур EXPRESSION) 
#define VARS_COUNT 100 //максимально количество хранимых в программе переменных

EXPRESSION stack[STACK_SIZE] = {0};
int expressions_count = 0;

VARIABLES variable_table[VARS_COUNT] = {0};  //таблица переменных и их значений
int variables_count = 0;  //увеличиваем на единицу при создании новой переменной


bool variable_existence_check(char* var_name){
    bool if_exists = false;
    for(int i = 0; i < variables_count; i++){
        if(strcmp(var_name, variable_table[i].name) == 0) if_exists = true;
    }
    return if_exists;
}



EXPRESSION* create_expression(const int line_number, const char* expr, const char* kword, const char* args){
    EXPRESSION* new_expr = malloc(sizeof(EXPRESSION));
    if(!new_expr){
        printf("ERROR: Memory error\n");
        return NULL;
    }

    new_expr -> line_number = line_number;

    if(expr){
        new_expr -> expression = malloc(strlen(expr) + 1);
        if(!new_expr -> expression) {
            free(new_expr);    //если не получилось выделить память под текст выражения, то освобождаем память занятую всем выражением (EXPRESSION) 
            printf("ERROR: Memory error\n");
            return NULL;
        }

        strcpy(new_expr -> expression, expr);
    } else new_expr -> expression = NULL;  //если выражение было передано в функцию как NULL, то инициализируем поле структуры как NULL

    if(kword){
        new_expr -> keyword = malloc(strlen(kword) + 1);
        if(!new_expr -> keyword) {
            if(new_expr -> expression) free(new_expr -> expression);  // аналогично освобождаем ВСЮ выделенную память, в случае единственного неудачного выделания
            free(new_expr);
            printf("ERROR: Memory error\n");
            return NULL;
        }    

        strcpy(new_expr -> keyword, kword);
    } else new_expr -> keyword = NULL; //аналогичная инициализация NULL, чтобы не было 'мусорных' значений

    if(args){
        new_expr -> arguments = malloc(strlen(args) + 1);
        if(!new_expr -> arguments) {
            if(new_expr -> expression) free(new_expr -> expression);
            if(new_expr -> keyword) free(new_expr -> keyword);
            free(new_expr);
            
            printf("ERROR: Memory error\n");
            return NULL;
        }

        strcpy(new_expr -> arguments, args);
    } else new_expr -> arguments = NULL;

    return new_expr;
}



ADD_EXPRESSION_STATUS add_expression(EXPRESSION* expr){
    if(expressions_count >= STACK_SIZE){
        printf("ERROR: Too many expressions");
        return Failed_Add_Expression;
    }

    stack[expressions_count].line_number = expr->line_number;

    if(expr -> expression){  //если при создании выражения, текст выражения НЕ был проинициализирован как NULL
        stack[expressions_count].expression = malloc(strlen(expr -> expression) + 1);
        if(!stack[expressions_count].expression) {
            printf("ERROR: Memory error\n");
            return Failed_Add_Expression;
        }

        strcpy(stack[expressions_count].expression, expr -> expression);
    } else stack[expressions_count].expression = NULL;   //иначе инициализируем поле структуры 'expression' как NULL

    if(expr -> keyword){  
        stack[expressions_count].keyword = malloc(strlen(expr -> keyword) + 1);
        if(!stack[expressions_count].keyword) {
            printf("ERROR: Memory error\n");
            return Failed_Add_Expression;
        }

        strcpy(stack[expressions_count].keyword, expr -> keyword);
    } else stack[expressions_count].keyword = NULL;   

    if(expr -> arguments){  
        stack[expressions_count].arguments = malloc(strlen(expr -> arguments) + 1);
        if(!stack[expressions_count].arguments) {
            printf("ERROR: Memory error\n");
            return Failed_Add_Expression;
        }

        strcpy(stack[expressions_count].arguments, expr -> arguments);
    } else stack[expressions_count].arguments = NULL;   
    
    return Succsessful_Add_Expression;
}



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
   
    //обработка ключевого слова print

    /*учесть незначащие пробелы и подвыражения, например: x + 5.
    сначала необходимо вычислить подвыражения, а затем добавить их в поток*/
    if(stream[token_idx++].type == TOKEN_print) {
        if(stream[token_idx++].type != TOKEN_open_paren){ //ключевому слову print необходимо указать аргументы в скобках
            printf("ERROR: The 'print' keyword requires a list of arguments in parentheses\n");
            return Failed_Parsing;
        }

        if(stream[token_idx].type == TOKEN_variable){
            //проверяем, что передання переменная существует
            if(!(variable_existence_check(stream[token_idx].text))) {
                printf("ERROR: Variable '%s' not declared\n", stream[token_idx].text);
                return Failed_Parsing;
            }
        }
        //create_expression(NULL, "print", )
    }

    token_idx = 0;

    while(get_priority(stream[token_idx]) != OPEN_OR_CLOSE_PAREN){
        token_idx++;
    }
    


    return Successful_Parsing;
}