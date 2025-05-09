#include "parser.h" 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern int variables_count;
extern VARIABLE variables_array[MAX_VARIABLES_COUNT];

/*
Функция вычисляет сложные выражения, упрощает их, и возвращает токен. 
Например, имеем строку кода: x = 3 * (5 + y);
В функцию передаются токены rvalue (3 * (5 + y))
Фунция вернёт токен числа 21 (при условии, что y = 2),
*/
TOKEN* parse_expression(TOKEN tokens[], int tokens_count) {
    if (tokens == NULL || tokens_count == 0) {
        printf("ERROR: Empty tokens array\n");
        return NULL;
    }

    if (tokens_count == 1) {
        // Базовый случай: если токен один, возвращаем его 
        if(tokens[0].type == TOKEN_variable){
            char text_val[MAX_STRING_LEN] = {0}; //массив символов для текстового представления значания переменной

            DATA_TYPE val = {0};
            bool str = 0;

            int var_idx = get_variable_index(tokens[0].text);
            if(var_idx == -1) {
                fprintf(stderr, "ERROR: variable '%s' not declared\n", tokens[0].text);
                return NULL;
            } 
            //копируем её значание (int или double) в соответствующие поля объединения val
            if(variables_array[var_idx].type == INT_TYPE) {
                val.int_value = variables_array[var_idx].value.int_value;
                //записыаем в text_val значение переменной
                snprintf(text_val, 31, "%d", val.int_value);
            }
            else if (variables_array[var_idx].type == DOUBLE_TYPE){
                val.double_value = variables_array[var_idx].value.double_value;
                snprintf(text_val, 31, "%f", val.double_value);
            } else {
                str = 1;
                strcpy(val.string_value, variables_array[var_idx].value.string_value);
                snprintf(text_val, MAX_STRING_LEN, "%s", val.string_value);
            }

            if(str){
                TOKEN* token = create_token(TOKEN_string, text_val);
                return token;
            } else {
                TOKEN* token = create_token(TOKEN_number, text_val);
                return token;
            }
        }

        if(tokens[0].type == TOKEN_number){
            char buff[31];
            strcpy(buff, tokens[0].text);

            TOKEN* token_num = create_token(TOKEN_number, buff); 
            if(!token_num){
                fprintf(stderr, "ERROR: Memory error\n");
                return NULL;
            }
            return token_num;
        }

        if(tokens[0].type == TOKEN_string){
            char buff[MAX_STRING_LEN] = {'\0'};
            strcpy(buff, tokens[0].text);

            TOKEN* token_str = create_token(TOKEN_string, buff);
            if(!token_str){
                fprintf(stderr, "ERROR: Memory error\n");
                return NULL;
            }
            return token_str;
        }
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

        int close_par_idx = open_par_idx+1;
        int paren_lvl = 0;
        while (tokens[close_par_idx].type != TOKEN_close_paren) {
            if(tokens[close_par_idx].type == TOKEN_open_paren){
                paren_lvl++;
            }
            close_par_idx++;
        }
        while(paren_lvl){
            close_par_idx++;
            if(tokens[close_par_idx].type == TOKEN_close_paren)paren_lvl--;
        }
        int from = open_par_idx+1;
        int to = close_par_idx-1;

        int count = 0;
        while (count < 100 && from <= to) {
            subtokens[count++] = tokens[from++];
        }

        // сперва парсим то, что в скобках
        TOKEN* paren_expression =  parse_expression(subtokens, count);

        TOKEN updated_tokens[100];
        int new_count = tokens_count - (count+2) + 1;

        int pos = 0;
        for(int i = 0; i < open_par_idx; i++){
            updated_tokens[pos++] = tokens[i];
        }

        updated_tokens[pos].type = paren_expression->type;

        updated_tokens[pos].text = malloc(strlen(paren_expression->text) + 1);
        if(!updated_tokens[pos].text) {
            printf("ERROR: Memory error\n");
            return NULL;
        }
        strcpy(updated_tokens[pos].text, paren_expression -> text);
        int idx = pos;
        pos++;

        for(int i = close_par_idx + 1; i < tokens_count; i++){
            updated_tokens[pos++] = tokens[i];
        }

        return parse_expression(updated_tokens, new_count);

        free(updated_tokens[idx].text);
    }

    // Обработка математических операций с учётом приоритетов
    // Сначала умножение и деление
    for (int i = 0; i < tokens_count; i++) {
        if (tokens[i].type == TOKEN_math_operator && (strcmp(tokens[i].text, "*") == 0 || strcmp(tokens[i].text, "/") == 0)) {
            TOKEN left_part[100], right_part[100];
            int left_count = 0, right_count = 0;

            // Разделяем выражение на левую и правую части от оператора
            for (int j = i-1; j >= 0 && tokens[j].type != TOKEN_math_operator; j--) {
                left_part[left_count++] = tokens[j];
            }
            for (int j = i + 1; j < tokens_count && tokens[j].type != TOKEN_math_operator; j++) {
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

            // Проверяем, что оба операнда - числа или переменные
            DATA_TYPE left_num = {0};  //переменная для хранения численного значения токена с целью дальнейшего вычисления
            DATA_TYPE right_num = {0};

            int int_left = 1; //флаг, показывающий в какое поле (инт/дабл) было записано значение

            if(left_result -> type != TOKEN_number){
                if(left_result -> type != TOKEN_variable){
                    printf("ERROR: Can only perform arithmetic operations on numbers\n");
                    free(left_result->text);
                    free(right_result->text);
                    free(left_result);
                    free(right_result);
                    return NULL;
                }
                else {
                    //если токен - переменная, проверяем её существование (так как в этой функции обрабатываются rvalue, то если переменной не существует - это ошибка)
                    if(!(check_variable_exists(left_result -> text))){
                        printf("ERROR: Variable '%s' not declared", left_result -> text);
                        return NULL;
                    }

                    //записываем численное значение переменной в left_num
                    for(int i = 0; i < variables_count;i++){
                        if(strcmp(variables_array[i].name, left_result -> text) == 0){
                            if(variables_array[i].type == INT_TYPE) {
                                left_num.int_value = variables_array[i].value.int_value;
                            } else {
                                left_num.double_value = variables_array[i].value.double_value;
                                int_left = 0;
                            }
                        }
                    }
                }
            } else {
                 //если токен - число, то берем его значение
                char* ptr = strchr(left_result -> text, '.'); //ищем плавающую точку в числе
                if(ptr){
                    left_num.double_value = atof(left_result -> text);
                    int_left = 0;
                } else {
                    left_num.int_value = atoi(left_result -> text);
                }
            }

            int int_right = 1;

            if(right_result -> type != TOKEN_number){
                if(right_result -> type != TOKEN_variable){
                    printf("ERROR: Can only perform arithmetic operations on numbers\n");
                    free(left_result->text);
                    free(right_result->text);
                    free(left_result);
                    free(right_result);
                    return NULL;
                }
                else {
                    //если токен - переменная, проверяем её существование (так как в этой функции обрабатываются rvalue, то если переменной не существует - это ошибка)
                    if(!(check_variable_exists(right_result -> text))){
                        printf("ERROR: Variable '%s' not declared", right_result -> text);
                        return NULL;
                    }

                    //записываем численное значение переменной в left_num
                    for(int i = 0; i < variables_count;i++){
                        if(strcmp(variables_array[i].name, right_result -> text) == 0){
                            if(variables_array[i].type == INT_TYPE) {
                                right_num.int_value = variables_array[i].value.int_value;
                            } else {
                                right_num.double_value = variables_array[i].value.double_value;
                                int_right = 0;
                            }
                        }
                    }
                }
            } else {
                //если токен - число, то берем его значение
               char* ptr = strchr(right_result -> text, '.'); //ищем плавающую точку в числе
               if(ptr){
                    right_num.double_value = atof(right_result -> text);
                    int_right = 0;
               } else {
                    right_num.int_value = atoi(right_result -> text);
               }            
           }

            // Выполняем операцию
            DATA_TYPE result = {0};
            
            if (strcmp(tokens[i].text, "*") == 0) {
                if (int_left && int_right) {
                    // Оба числа целые
                    result.double_value = left_num.int_value * right_num.int_value;
                } 
                else if (int_left) {
                    // Левое целое, правое дробное
                    result.double_value = left_num.int_value * right_num.double_value;
                }
                else if (int_right) {
                    // Правое целое, левое дробное
                    result.double_value = left_num.double_value * right_num.int_value;
                }
                else {
                    // Оба дробные
                    result.double_value = left_num.double_value * right_num.double_value;
                }
            } else {
                if (int_right) {
                    if(right_num.int_value == 0) {
                        printf("ERROR: Division by zero\n");
                        free(left_result->text);
                        free(right_result->text);
                        free(left_result);
                        free(right_result);
                        return NULL;
                    }
                } else {
                    if(right_num.double_value == 0) {
                        printf("ERROR: Division by zero\n");
                        free(left_result->text);
                        free(right_result->text);
                        free(left_result);
                        free(right_result);
                        return NULL;
                    }
                }

                if (int_left && int_right) {
                    // Оба числа целые
                    result.double_value = left_num.int_value / (double)right_num.int_value;
                } 
                else if (int_left) {
                    // Левое целое, правое дробное
                    result.double_value = left_num.int_value / right_num.double_value;
                }
                else if (int_right) {
                    // Правое целое, левое дробное
                    result.double_value = left_num.double_value / (double)right_num.int_value;
                }
                else {
                    // Оба дробные
                    result.double_value = left_num.double_value / right_num.double_value;
                }
            }

            // Освобождаем память
            free_token(left_result);
            free_token(right_result);

            // Создаём результирующий токен
            TOKEN* result_token = malloc(sizeof(TOKEN));
            if (!result_token) {
                printf("ERROR: Memory error\n");
                return NULL;
            }
            
            result_token->type = TOKEN_number;
            char buffer[50];
            snprintf(buffer, sizeof(buffer), "%.2f", result.double_value);
            result_token->text = malloc(strlen(buffer) + 1);
            if (!result_token->text) {
                free(result_token);
                printf("ERROR: Memory error\n");
                return NULL;
            }
            strcpy(result_token->text, buffer);

            //обновленный массив токенов, содержащий только результат умножения/деления
            TOKEN updated_tokens[100];
            int new_count = tokens_count - (left_count + right_count + 1) + 1;

            // Копируем часть до левого операнда
            int pos = 0;
            for (int j = 0; j < i - left_count; j++) {
                updated_tokens[pos++] = tokens[j];
            }

            updated_tokens[pos].type = TOKEN_number;
            updated_tokens[pos].text = malloc(strlen(result_token->text) + 1);
            if(!updated_tokens[pos].text) {
                printf("ERROR: Memory error\n");
                return NULL;
            }

            int idx = pos;
            strcpy(updated_tokens[pos++].text, result_token->text);

            // Копируем часть после правого операнда
            for (int j = i + right_count + 1; j < tokens_count; j++) {
                updated_tokens[pos++] = tokens[j];
            }

            free_token(result_token);
            
            return parse_expression(updated_tokens, new_count);
        }
    }

    // Затем сложение и вычитание
    for (int i = tokens_count - 1; i >= 0; i--) {
        if (tokens[i].type == TOKEN_math_operator && (strcmp(tokens[i].text, "+") == 0 || strcmp(tokens[i].text, "-") == 0)) {
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
        
            // Проверяем, что оба операнда - числа или переменные или строки
            DATA_TYPE left_num = {0};  //переменная для хранения численного значения токена с целью дальнейшего вычисления
            DATA_TYPE right_num = {0};

            int int_left = 0;
            int double_left = 0;

            if(left_result -> type != TOKEN_number && left_result -> type != TOKEN_string){
                if(left_result -> type != TOKEN_variable){
                    printf("ERROR: Can only perform arithmetic operations on numbers\n");
                    free(left_result->text);
                    free(right_result->text);
                    free(left_result);
                    free(right_result);
                    return NULL;
                }
                else {
                    //если токен - переменная, проверяем её существование (так как в этой функции обрабатываются rvalue, то если переменной не существует - это ошибка)
                    if(!(check_variable_exists(left_result -> text))){
                        printf("ERROR: Variable '%s' not declared", left_result -> text);
                        free(left_result->text);
                        free(right_result->text);
                        free(left_result);
                        free(right_result);
                        return NULL;
                    }

                    //записываем численное значение переменной в left_num
                    for(int i = 0; i < variables_count;i ++){
                        if(strcmp(variables_array[i].name, left_result -> text) == 0){
                            if(variables_array[i].type == INT_TYPE){
                                left_num.int_value = variables_array[i].value.int_value;
                                int_left = 1;
                                double_left = 0;
                            } else if(variables_array[i].type == DOUBLE_TYPE){
                                left_num.double_value = variables_array[i].value.double_value;
                                int_left = 0;
                                double_left = 1;
                            } else {
                                int_left = 0; 
                                double_left = 0;
                                strcpy(left_num.string_value, variables_array[i].value.string_value);
                            }
                        }
                    }
                }
            } else {

                if(left_result -> type == TOKEN_string){
                    int_left = 0; 
                    double_left = 0;
                    strcpy(left_num.string_value, left_result -> text);
                } else {
                    //если токен - число, то берем его значение
                    char* ptr = strchr(left_result -> text, '.'); //ищем плавающую точку в числе
                    if(ptr){
                        left_num.double_value = atof(left_result -> text);
                        int_left = 0;
                        double_left = 1;
                    } else {
                        int_left = 1;
                        double_left = 0;
                        left_num.int_value = atoi(left_result -> text);
                    }  
                }     
            }

            int int_right = 0;
            int double_right = 0;


            if(right_result -> type != TOKEN_number && right_result -> type != TOKEN_string){
                if(right_result -> type != TOKEN_variable){
                    printf("ERROR: Can only perform arithmetic operations on numbers\n");
                    free(left_result->text);
                    free(right_result->text);
                    free(left_result);
                    free(right_result);
                    return NULL;
                }
                else {
                    //если токен - переменная, проверяем её существование (так как в этой функции обрабатываются rvalue, то если переменной не существует - это ошибка)
                    if(!(check_variable_exists(right_result -> text))){
                        printf("ERROR: Variable '%s' not declared", right_result -> text);
                        free(left_result->text);
                        free(right_result->text);
                        free(left_result);
                        free(right_result);
                        return NULL;
                    }

                    //записываем численное значение переменной в left_num
                    for(int i = 0; i < variables_count;i ++){
                        if(variables_array[i].name == right_result -> text){
                            if(variables_array[i].type == INT_TYPE) {
                                int_right = 1;
                                double_right = 0;
                                right_num.int_value = variables_array[i].value.int_value;
                            } else if(variables_array[i].type == DOUBLE_TYPE){
                                right_num.double_value = variables_array[i].value.double_value;
                                int_right = 0;
                                double_right = 1;
                            } else {
                                int_right = 0; 
                                double_right = 0;
                                strcpy(right_num.string_value, variables_array[i].value.string_value);
                            }
                        }
                    }
                }
            } else {
                if(right_result -> type == TOKEN_string){
                    int_right = 0; 
                    double_right = 0;
                    strcpy(right_num.string_value, right_result -> text);
                } else {
                    //если токен - число, то берем его значение
                    char* ptr = strchr(right_result -> text, '.'); //ищем плавающую точку в числе
                    if(ptr){
                        right_num.double_value = atof(right_result -> text);
                        int_right = 0;
                        double_right = 1;
                    } else {
                        int_right = 1;
                        double_right = 0;
                        right_num.int_value = atoi(right_result -> text);
                    }   
                }  
            }

            DATA_TYPE result = {0};

            //если переданы две строки - производим конкатенацию
            if(!int_left && !double_left && !int_right && !double_right){

                if (strcmp(tokens[i].text, "+") == 0) {
                    char concatenation_string[MAX_STRING_LEN] = {'\0'};

                    if(strlen(left_num.string_value) + strlen(right_num.string_value) >= MAX_STRING_LEN){
                        fprintf(stderr, "ERROR: String too long\n");
                        free(left_result->text);
                        free(right_result->text);
                        free(left_result);
                        free(right_result);
                        return NULL;
                    }  

                    strcpy(concatenation_string, left_num.string_value);
                    strcat(concatenation_string, right_num.string_value);

                    free(left_result->text);
                    free(right_result->text);
                    free(left_result);
                    free(right_result);

                    TOKEN* result_token = create_token(TOKEN_string, concatenation_string);
                    if(!result_token){
                        fprintf(stderr, "ERROR: Memory error\n");
                        return NULL;
                    }

                    return result_token;

                } else { 
                    fprintf(stderr, "ERROR: Cannot subtract string\n");
                    free(left_result->text);
                    free(right_result->text);
                    free(left_result);
                    free(right_result);
                    return NULL;
                }
            }

            if(!int_left && !double_left && (int_right || double_right) ||
               !int_right &&!double_right && (int_left || double_left)) {
                fprintf(stderr, "ERROR: Сannot perform arithmetic operations on numbers and strings together\n");
                    free(left_result->text);
                    free(right_result->text);
                    free(left_result);
                    free(right_result);
                    return NULL;
               }

            double left = int_left ? (double)left_num.int_value : left_num.double_value;
            double right = int_right ? (double)right_num.int_value : right_num.double_value;
            
            if (strcmp(tokens[i].text, "+") == 0) {
                result.double_value = left + right;
            } else { 
                result.double_value = left - right;
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
            snprintf(buffer, sizeof(buffer), "%.2f", result.double_value);
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