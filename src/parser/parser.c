#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include "../lexer/lexer.c" //для MAX_VARIABLE_LEN

#define MAX_VARIABLES_COUNT 100 //можно объявить до ста переменных в коде
#define MAX_AST_COUNT   50 // до 50 узлов может быть в одной строке

VARIABLE variables_array[MAX_VARIABLES_COUNT] = {0}; //массив переменных, будет заполняться во время создания переменных
int variables_count = 0;

bool check_variable_exists(const char* name){
    bool exists = false;
    for(int i = 0; i < variables_count; i++){
        if(strcmp(name, variables_array[i].name) == 0){
            exists = true;
        }
    }

    return exists;
}

AST AST_array[MAX_AST_COUNT] = {0};

AST* create_AST(const TOKEN node, const TOKEN* right, const char* left){
    AST* new_ast = malloc(sizeof(AST));
    if(!new_ast){
        printf("ERROR: Memory error\n");
        return NULL;
    }

    new_ast -> node = node;

    new_ast -> right = malloc(sizeof(TOKEN));
    if(!new_ast -> right) {
        free(new_ast);

        printf("ERROR: Memory error\n");
        return NULL;
    }
    new_ast -> right = right;

    new_ast -> left = malloc(strlen(left) + 1);
    if(!new_ast -> left){
        free(new_ast -> right);
        free(new_ast);

        printf("ERROR: Memory error\n");
        return NULL;
    }
    strcpy(new_ast -> left, left);

    return new_ast;
}

/*
Функция вычисляет сложные выражения, упрощает их, и возвращает токен. 
Например, имеем строку кода: x = 3 * (5 + y);
В функцию передаются токены rvalue (3 * (5 + y))
Фунция вернёт токен числа 21 (при условии, что y = 2),
*/
TOKEN* parse_expression(TOKEN tokens[], int tokens_count) {
    if (tokens_count == 1) {
        // Базовый случай: если токен один, возвращаем его
        TOKEN* token = malloc(sizeof(TOKEN));
        if (!token) {
            printf("ERROR: Memory error\n");
            return NULL;
        }
        
        token->type = tokens[0].type;
        token->text = malloc(strlen(tokens[0].text) + 1);
        if (!token->text) {
            free(token);
            printf("ERROR: Memory error\n");
            return NULL;
        }
        strcpy(token->text, tokens[0].text);
        
        return token;
    }

    // Проверка скобок
    int open_parantases_count = 0;
    int close_parantases_count = 0;

    for (int i = 0; i < tokens_count; i++) {
        if (tokens[i].type == TOKEN_open_paren) open_parantases_count++;
        if (tokens[i].type == TOKEN_close_paren) close_parantases_count++;
    }

    if (open_parantases_count != close_parantases_count) {
        printf("ERROR: All parentheses must be closed\n");
        return NULL;
    }

    // Обработка скобок
    if (open_parantases_count) {
        TOKEN subtokens[100];
        int open_par_idx = 0;
        while (tokens[open_par_idx].type != TOKEN_open_paren) open_par_idx++;

        int close_par_idx = tokens_count - 1;
        while (tokens[close_par_idx].type != TOKEN_close_paren) close_par_idx--;

        open_par_idx++;
        close_par_idx--;

        int count = 0;
        while (count < 100 && open_par_idx <= close_par_idx) {
            subtokens[count++] = tokens[open_par_idx++];
        }

        return parse_expression(subtokens, count);
    }

    // Обработка математических операций с учётом приоритетов
    // Сначала умножение и деление
    for (int i = 0; i < tokens_count; i++) {
        if (tokens[i].type == TOKEN_math_operator && (tokens[i].text == "*" || tokens[i].text == "/")) {
            TOKEN left_part[100], right_part[100];
            int left_count = 0, right_count = 0;

            // Разделяем выражение на левую и правую части от оператора
            for (int j = 0; j < i; j++) {
                left_part[left_count++] = tokens[j];
            }
            for (int j = i + 1; j < tokens_count; j++) {
                right_part[right_count++] = tokens[j];
            }

            // Рекурсивно вычисляем левую и правую части
            TOKEN* left_result = parse_expression(left_part, left_count);
            TOKEN* right_result = parse_expression(right_part, right_count);
            
            if (!left_result || !right_result) {
                if (left_result) free(left_result->text);
                if (right_result) free(right_result->text);
                free(left_result);
                free(right_result);
                return NULL;
            }

            // Проверяем, что оба операнда - числа
            if (left_result->type != TOKEN_number || right_result->type != TOKEN_number) {
                printf("ERROR: Can only perform arithmetic operations on numbers\n");
                free(left_result->text);
                free(right_result->text);
                free(left_result);
                free(right_result);
                return NULL;
            }

            // Выполняем операцию
            int left_num = atoi(left_result->text);
            int right_num = atoi(right_result->text);
            int result;
            
            if (tokens[i].text == "*") {
                result = left_num * right_num;
            } else {
                if (right_num == 0) {
                    printf("ERROR: Division by zero\n");
                    free(left_result->text);
                    free(right_result->text);
                    free(left_result);
                    free(right_result);
                    return NULL;
                }
                result = left_num / right_num;
            }

            // Освобождаем память
            free(left_result->text);
            free(right_result->text);
            free(left_result);
            free(right_result);

            // Создаём результирующий токен
            TOKEN* result_token = malloc(sizeof(TOKEN));
            if (!result_token) {
                printf("ERROR: Memory error\n");
                return NULL;
            }
            
            result_token->type = TOKEN_number;
            char buffer[50];
            snprintf(buffer, sizeof(buffer), "%d", result);
            result_token->text = malloc(strlen(buffer) + 1);
            if (!result_token->text) {
                free(result_token);
                printf("ERROR: Memory error\n");
                return NULL;
            }
            strcpy(result_token->text, buffer);
            
            return result_token;
        }
    }

    // Затем сложение и вычитание
    for (int i = 0; i < tokens_count; i++) {
        if (tokens[i].type == TOKEN_math_operator && (tokens[i].text == "-" || tokens[i].text == "+")) {
            TOKEN left_part[100], right_part[100];
            int left_count = 0, right_count = 0;

            for (int j = 0; j < i; j++) {
                left_part[left_count++] = tokens[j];
            }
            for (int j = i + 1; j < tokens_count; j++) {
                right_part[right_count++] = tokens[j];
            }

            TOKEN* left_result = parse_expression(left_part, left_count);
            TOKEN* right_result = parse_expression(right_part, right_count);
            
            if (!left_result || !right_result) {
                if (left_result) free(left_result->text);
                if (right_result) free(right_result->text);
                free(left_result);
                free(right_result);
                return NULL;
            }

            if (left_result->type != TOKEN_number || right_result->type != TOKEN_number) {
                printf("ERROR: Can only perform arithmetic operations on numbers\n");
                free(left_result->text);
                free(right_result->text);
                free(left_result);
                free(right_result);
                return NULL;
            }

            double left_num = atof(left_result->text);
            double right_num = atof(right_result->text);
            double result;
            
            if (tokens[i].text == "+") {
                result = left_num + right_num;
            } else {
                result = left_num - right_num;
            }

            free(left_result->text);
            free(right_result->text);
            free(left_result);
            free(right_result);

            TOKEN* result_token = malloc(sizeof(TOKEN));
            if (!result_token) {
                printf("ERROR: Memory error\n");
                return NULL;
            }
            
            result_token->type = TOKEN_number;
            char buffer[50];
            snprintf(buffer, sizeof(buffer), "%d", result);
            result_token->text = malloc(strlen(buffer) + 1);
            if (!result_token->text) {
                free(result_token);
                printf("ERROR: Memory error\n");
                return NULL;
            }
            strcpy(result_token->text, buffer);
            
            return result_token;
        }
    }

    // Если дошли сюда, значит выражение не распознано
    printf("ERROR: Invalid expression\n");
    return NULL;
}

PARSING_STATUS parsing(TOKEN stream[], int tokens_count)
{
    int semicolon_count = 0;
    for(int i = 0; i < tokens_count; i++){  //считаем кол-во символов ';' в коде: именно столько будет в нём строк
        if(stream[i].type == TOKEN_semicolon){
            semicolon_count++;
        }
    }
    
    int k = 0; //индекс для перебора токенов всего кода
    int line_number = 1;
    for(int i = 0; i < semicolon_count; i++){
        TOKEN line[100];
        int j = 0;
        while(stream[k].type != TOKEN_semicolon && k < tokens_count){  //формируем массив токенов одной строки для дальнейшего парсинга
            if(stream[k].type == TOKEN_space) { //сразу пропускаем незнаяещие пробелы
                k++;
                continue;
            }
            line[j++] = stream[k++];
        }    
        int tokens_count_in_line = j;  //опредеяем кол-во токенов в строке

        if(line[0].type == TOKEN_print) {  //если строка содержит 'print'
            /*
                code (должна быть отдельная обработка списка аргументов к 'print' и создание выражения)
            */
        }

        else if(line[0].type == TOKEN_end){  //если строка содержит 'end' (то есть последняя строка)
            if(line[1].type != TOKEN_semicolon){  //если после 'end' нет точки с запятой - ошибка
                printf("ERROR: Missed symbol ';' after 'end' keyword in line %d\n", line_number);
                return Failed_Parsing;
            }
            /*
            добавляем в массив выражений 'end' и завершаем цикл (все что находится псле end обработано не будет)
            */
            break;
        }

        //если строка не содержит 'print' или 'end' (то есть не последняя строка)
        else{
            //ищем в строке символ присваивания '='
            int assingment_idx = -1;
            for(int i = 0;i < tokens_count_in_line; i++){
                if(line[i].type == TOKEN_assign){
                    assingment_idx = i;
                }
            }
            if(assingment_idx != -1){

                if(assingment_idx == 0){  //если символ '=' стоит первым в строке - ошибка
                    printf("ERROR in %d line: Missed lvalue before '='\n", line_number);
                    return Failed_Parsing;
                }
                if(assingment_idx == tokens_count_in_line - 1){  //если символ '=' стоит последним в строке - ошибка
                    printf("ERROR in %d line: Missed rvalue after '='\n", line_number);
                    return Failed_Parsing;
                }
                if(assingment_idx != 1){ //знак '=' обязан быть вторым токеном в строке (первый - переменная)
                    printf("ERROR in %d line: The '=' sign must come immediately after the variable\n", line_number);
                    return Failed_Parsing;
                }

                //проверяю, что то, что стоит перед '=' - переменная
                if(line[0].type != TOKEN_variable){
                    printf("ERROR in %d line: '%s' is not lvalue\n", line_number, line[0].text);
                    return Failed_Parsing;
                } 

                char left[MAX_VARIABLE_LEN] = {'\0'};
                strcpy(left, line[0].text);

                TOKEN right[150];
                int j = 0;
                for (int i = assingment_idx + 1; i < tokens_count_in_line; i++) {
                    right[j] = line[i];
                }

                /*необходимо сформировать AST с корнем в виде токена '=' и лепестками в виде lvalue и rvalue*/
                AST* ast = create_AST((TOKEN){.type = TOKEN_assign, .text = "="}, right, left);
                if(ast == NULL){
                    printf("ERROR in %d line: Failed to create AST", line_number);
                    return Failed_Parsing;
                }


                //ПОСЛЕ ТОГО, КАК СФОРМИРОВАЛИ AST ДЛЯ RVALUE:
                //если первый токен - переменная, проверяю её наличие в массиве переменных
                if(!check_variable_exists()) { //если такой переменной не существует - создаём её и добавляем к массиву И УВЕЛИЧИВАЕМ variables_count
                    
                }
            }
        }
    }

    return Successful_Parsing;
}