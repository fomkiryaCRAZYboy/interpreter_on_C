#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STREAM_SIZE 2500 //максимальное количество токенов в коде

TOKEN stream[STREAM_SIZE] = {0};  // массив для потока токенов
int tokens_count = 0;  //количество токенов в потоке


//добавляю созданный токен в поток (массив stream)
TOKEN_ADD_STATUS add_token(TOKEN* token) {
    if(tokens_count >= STREAM_SIZE) {
        printf("ERROR: Token stream overflow\n");
        return Failed_Add;
    }

    stream[tokens_count].type = token -> type;
    stream[tokens_count].text = malloc(strlen(token -> text) + 1);
    //здесь выделяется память под текст токена уже в потоке 

    if(stream[tokens_count].text == NULL) {
        printf("ERROR: Memory error\n");
        return Failed_Add;
    }

    strcpy(stream[tokens_count++].text, token -> text);

    return Successful_Add;
}

//создаю новый токен в куче и вношу в него соответствующие данные
TOKEN* create_token(const TOKEN_TYPE type, const char* text){
    TOKEN* new_token = malloc(sizeof(TOKEN));

    if(new_token == NULL) {
        printf("ERROR: Memory error\n");
        return NULL;
    }

    new_token -> type = type;

    new_token -> text = malloc(strlen(text) + 1);

    if(!new_token -> text) {
        free(new_token);  //освобождаю память, занятую токеном, если не удалось выделить память под текст токена
        printf("ERROR: Memory error\n");
        return NULL;
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
TOKENIZATION_STATUS tokenize(char* code_string) {
    char* str = code_string;
    while(*str) {
        //пропуск незначащих пробелов
        if(isspace(*str)) {
            str++;
            continue;
        }

        if(*str == '\"') {
            char string[MAX_STRING_LEN] = {'\0'};
            str++;

            int i = 0;
            for(; i < MAX_STRING_LEN - 1 && *str != '\"'; i++){
                string[i] = *str++;
            }
            if(*str != '\"'){
                printf("ERROR: Max string length = 30\n");
                return Failed_Tokenization;
            }

            string[i] = '\0';
            str++;

            TOKEN* string_token = create_token(TOKEN_string, string);
            if(add_token(string_token) != Successful_Add) {
                free_token(string_token);
                printf("ERROR: Failed to add token(string)\n");
                return Failed_Tokenization;
            }

            free_token(string_token);
            continue;
        }

        //если символ - буква
        if(isalpha(*str)){
            char varaible[MAX_VARIABLE_LEN] = {'\0'};

            int i = 0;
            while(isalpha(*str) && i < MAX_VARIABLE_LEN - 1){
                varaible[i] = *str++;
                i++;
            }
            varaible[i] = '\0';

            if(isalpha(*str)){
                printf("ERROR: Max variable length = 25\n");
                return Failed_Tokenization;
            }

            //проверяю введенную пользователем лексему на принадлежность к ключевым словам
            if(strcmp(varaible, "end") == 0){
                TOKEN* end_token = create_token(TOKEN_end, "end");
                if(add_token(end_token) != Successful_Add) {
                    printf("ERROR: Failed to add token(end)\n");
                    return Failed_Tokenization;
                }
                free(end_token);
            }

            else if(strcmp(varaible, "print") == 0){
                TOKEN* print_token = create_token(TOKEN_print, "print");
                if(add_token(print_token) != Successful_Add) {
                    printf("ERROR: Failed to add token(print)\n");
                    return Failed_Tokenization;
                }
                free(print_token);
            }

            else{
            TOKEN* var_token = create_token(TOKEN_variable, varaible);
            if(add_token(var_token) != Successful_Add) {
                printf("ERROR: Failed to add token(var)\n");
                return Failed_Tokenization;
            }
            free_token(var_token);
            }
        }

        //если символ - цифра
        else if(isdigit(*str)){
            char number[32];

            int i = 0;
            while(isdigit(*str) && i < 31){
                number[i] = *str++;
                i++;
            }
            number[i] = '\0';
            
            TOKEN* num_token = create_token(TOKEN_number, number);  //создали токен числа
            if(add_token(num_token) != Successful_Add) {
                printf("ERROR: Failed to add token(num)\n");
                return Failed_Tokenization;
            }  //добавили токен в поток
            free_token(num_token); //освободили память, которую занимал  токен в куче

            //далее - следующая инетрация цикла, двигаемся далльше по строке кода
        }


        else{
            switch(*str) {
                case '+': {
                    TOKEN* plus_token = create_token(TOKEN_math_operator, "+");
                    if(add_token(plus_token) != Successful_Add) {
                        free_token(plus_token);
                        printf("ERROR: Failed to add token\n");
                        return Failed_Tokenization;
                    }
                    free_token(plus_token);
                    str++;
                    break;
                }
                case '-': {
                    TOKEN* minus_token = create_token(TOKEN_math_operator, "-");
                    if(add_token(minus_token) != Successful_Add) {
                        free_token(minus_token);
                        printf("ERROR: Failed to add token\n");
                        return Failed_Tokenization;
                    }
                    free_token(minus_token);
                    str++;
                    break;
                }
                case '*': {
                    TOKEN* multiply_token = create_token(TOKEN_math_operator, "*");
                    if(add_token(multiply_token) != Successful_Add) {
                        free_token(multiply_token);
                        printf("ERROR: Failed to add token\n");
                        return Failed_Tokenization;
                    }
                    free_token(multiply_token);
                    str++;
                    break;
                }
                case '/': {
                    TOKEN* divide_token = create_token(TOKEN_math_operator, "/");
                    if(add_token(divide_token) != Successful_Add) {
                        free_token(divide_token);
                        printf("ERROR: Failed to add token\n");
                        return Failed_Tokenization;
                    }
                    free_token(divide_token);
                    str++;
                    break;
                }
                case '(': {
                    TOKEN* open_paren_token = create_token(TOKEN_open_paren, "(");
                    if(add_token(open_paren_token) != Successful_Add) {
                        free_token(open_paren_token);
                        printf("ERROR: Failed to add token\n");
                        return Failed_Tokenization;
                    }
                    free_token(open_paren_token);
                    str++;
                    break;
                }
                case ')': {
                    TOKEN* close_paren_token = create_token(TOKEN_close_paren, ")");
                    if(add_token(close_paren_token) != Successful_Add) {
                        free_token(close_paren_token);
                        printf("ERROR: Failed to add token\n");
                        return Failed_Tokenization;
                    }
                    free_token(close_paren_token);
                    str++;
                    break;
                }
                case '=': {
                    TOKEN* assign_token = create_token(TOKEN_assign, "=");
                    if(add_token(assign_token) != Successful_Add) {
                        free_token(assign_token);
                        printf("ERROR: Failed to add token\n");
                        return Failed_Tokenization;
                    }
                    free_token(assign_token);
                    str++;
                    break;
                }
                case ';': {
                    TOKEN* semicolon_token = create_token(TOKEN_semicolon, ";");
                    if(add_token(semicolon_token) != Successful_Add) {
                        free_token(semicolon_token);
                        printf("ERROR: Failed to add token\n");
                        return Failed_Tokenization;
                    }
                    free_token(semicolon_token);
                    str++;
                    break;
                }
                case ',': {
                    TOKEN* comma_token = create_token(TOKEN_comma, ",");
                    if(add_token(comma_token) != Successful_Add) {
                        free_token(comma_token);
                        printf("ERROR: Failed to add token\n");
                        return Failed_Tokenization;
                    }
                    free_token(comma_token);
                    str++;
                    break;
                }
                case '.': {
                    TOKEN* dot_token = create_token(TOKEN_dot, ".");
                    if(add_token(dot_token) != Successful_Add) {
                        free_token(dot_token);
                        printf("ERROR: Failed to add token\n");
                        return Failed_Tokenization;
                    }
                    free_token(dot_token);
                    str++;
                    break;
                }
                
                default: {
                    printf("ERROR: Syntax error\n");
                    return Failed_Tokenization;
                }
            }
        }
    }
    return Successful_Tokenization;
    //после успешной обработки сторки возвращаем Successful_Tokenization
    //токенизация прошла успешно
} 