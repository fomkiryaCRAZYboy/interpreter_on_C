#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STREAM_SIZE 250 //максимальное количество токенов в строке
#define MAX_VARIABLE_LEN 25 //максимальная длина символов для переменной

TOKEN stream[STREAM_SIZE] = {0};  // массив для потока токенов
static int tokens_count = 0;  //количество токенов в потоке


//добавляю созданный токен в поток (массив stream)
void add_token(TOKEN* token) {
    if(tokens_count >= STREAM_SIZE) {
        printf("ERROR: Token stream overflow\n");
        return;
    }

    stream[tokens_count].type = token -> type;
    stream[tokens_count++].text = token -> text;
}

//создаю новый токен в куче и вношу в него соответствующие данные
TOKEN* create_token(const TOKEN_TYPE type, const char* text){
    TOKEN* new_token = malloc(sizeof(TOKEN));

    if(new_token == NULL) {
        printf("ERROR: Memory error\n");
        return;
    }

    new_token -> type = type;

    new_token -> text = malloc(strlen(text) + 1);

    if(!new_token -> text) {
        free(new_token);  //освобождаю память, занятую токеном, если не удалось выделить память под текст токена
        printf("ERROR: Memory error\n");
        return;
    }

    strcpy(new_token -> text, text);

    return new_token;
}

//после того как токен был успешно создан и добавлен в поток,
//освобождаю память, занятую текстом токена и самим токеном 
void free_token(TOKEN* token){
    free(token -> text);
    free(token);
}

//токенизация, разбиение строки кода на токены для формирование потока 
void tokenize(const char* code_string) {
    char* str = code_string;
    while(*str) {
        //пропуск незначащих пробелов
        //if(*str == ' ')
        if(isspace(*str)) {
            str++;
            continue;
        }

        //если символ - буква
        if(isalpha(*str)){
            char varaible[MAX_VARIABLE_LEN] = {'\0'};

            int i = 0;
            while(isalpha(*str) && i < MAX_VARIABLE_LEN - 1){
                varaible[i++] = *str++;
            }
            varaible[i] = '\0';

            if(isalpha(*str)){
                printf("ERROR: Max variable length = 25\n");
                return;
            }

            //проверяю введенную пользователем лексему на принадлежность к ключевым словам
            if(strcmp(varaible, "end") == 0){
                TOKEN* end_token = create_token(TOKEN_end, "end");
                add_token(end_token);
                free(end_token);
            }

            else if(strcmp(varaible, "print") == 0){
                TOKEN* print_token = create_token(TOKEN_print, "print");
                add_token(print_token);
                free(print_token);
            }

            else{
            TOKEN* var_token = create_token(TOKEN_variable, varaible);
            add_token(var_token);
            free_token(var_token);
            }
        }

        //если символ - цифра
        if(isdigit(*str)){
            char number[32];

            int i = 0;
            for(; isdigit(*str) && i < 31; i++){
                number[i] = *str++;
            }
            number[i] = '\0';
            
            TOKEN* num_token = create_token(TOKEN_number, number);  //создали токен числа
            add_token(num_token);  //добавили токен в поток
            free_token(num_token); //освободили память, которую занимал  токен в куче

            //далее - следующая инетрация цикла, двигаемся далльше по строке кода
        }


        else{
            switch(*str) {
                case '+': {
                    TOKEN* plus_token = create_token(TOKEN_math_operator, "+");
                    add_token(plus_token);
                    free_token(plus_token);
                    str++;
                    break;
                }
                case '-': {
                    TOKEN* minus_token = create_token(TOKEN_math_operator, "-");
                    add_token(minus_token);
                    free_token(minus_token);
                    str++;
                    break;
                }
                case '*': {
                    TOKEN* multiply_token = create_token(TOKEN_math_operator, "*");
                    add_token(multiply_token);
                    free_token(multiply_token);
                    str++;
                    break;
                }
                case '/': {
                    TOKEN* divide_token = create_token(TOKEN_math_operator, "/");
                    add_token(divide_token);
                    free_token(divide_token);
                    str++;
                    break;
                }
                case '(': {
                    TOKEN* open_paren_token = create_token(TOKEN_open_paren, "(");
                    add_token(open_paren_token);
                    free_token(open_paren_token);
                    str++;
                    break;
                }
                case ')': {
                    TOKEN* close_paren_token = create_token(TOKEN_close_paren, ")");
                    add_token(close_paren_token);
                    free_token(close_paren_token);
                    str++;
                    break;
                }
                case '=': {
                    TOKEN* assign_token = create_token(TOKEN_assign, "=");
                    add_token(assign_token);
                    free_token(assign_token);
                    str++;
                    break;
                }
                case ';': {
                    TOKEN* semicolon_token = create_token(TOKEN_semicolon, ";");
                    add_token(semicolon_token);
                    free_token(semicolon_token);
                    str++;
                    break;
                }
                case ',': {
                    TOKEN* comma_token = create_token(TOKEN_comma, ",");
                    add_token(comma_token);
                    free_token(comma_token);
                    str++;
                    break;
                }
                
                default: {
                    printf("ERROR: Syntax error\n");
                    return;
                }
            }
        }
    }

} 