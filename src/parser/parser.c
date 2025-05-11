#include "parser.h"
#include "../executor/executor.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int add_argument(TOKEN* arguments_array, TOKEN* argument, int index){
    if (!arguments_array || !argument || !argument->text) {
        printf("ERROR: Invalid arguments\n");
        return 0;
    }

    arguments_array[index].type = argument->type;
    arguments_array[index].text = malloc(strlen(argument -> text) + 1);
    if(!arguments_array[index].text){
        printf("ERROR: Memory error");
        return 0;
    }
    strcpy(arguments_array[index].text, argument -> text);

    return 1;
}


int comma_count(TOKEN arguments[], int tokens_count){
    int comma_count = 0;
    for(int i = 0; i < tokens_count; i++){
        if(arguments[i].type == TOKEN_comma)comma_count++;
    }

    return comma_count;
}


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

int get_variable_index(const char* name){
    for(int i = 0; i < variables_count; i++){
        if(strcmp(name, variables_array[i].name) == 0){
            return i;
        }
    }

    return -1;
}

VAR_ADD_STATUS create_and_add_variable(const char* name, const DATA_TYPE value, const USING_TYPE type){
    if(variables_count >= MAX_VARIABLES_COUNT){
        printf("ERROR: Too many variables declared\n");
        return Failed_add_var;
    }

    VARIABLE var;

    var.name = malloc(strlen(name) + 1);
    if(!var.name){
        printf("ERROR: Memory error");
        return Failed_add_var;
    }
    strncpy(var.name, name, strlen(name) + 1);

    var.type = type;
    bool str = false;

    // Копируем только нужное поле объединения
    if (type == INT_TYPE) {
        var.value.int_value = value.int_value;
    } else if (type == DOUBLE_TYPE) {
        var.value.double_value = value.double_value;
    } else {
        //если используемый тип не int и не double, значит это - строка
        str = true;
        strncpy(var.value.string_value, value.string_value, strlen(value.string_value) + 1);   
    }

    //сразу добавляем переменную в массив и увеличиваем variables_count
    variables_array[variables_count++] = var;

    return Successful_add_var;
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
        TOKEN line[200];
        int j = 0;
        while(stream[k].type != TOKEN_semicolon && k < tokens_count){  //формируем массив токенов одной строки для дальнейшего парсинга
            if(stream[k].type == TOKEN_space) { //сразу пропускаем незнаяещие пробелы
                k++;
                continue;
            }
            line[j++] = stream[k++];
        }
        //если цикл while закончился и k < меньше кол-ва токенов, значит он прервался из-за знака ';' 
        if(k < tokens_count){ //добавляем этот токен к строке
            line[j++] = stream[k++];
        }    
        int tokens_count_in_line = j;  //опредеяем кол-во токенов в строке

        if(line[0].type == TOKEN_print) {  //если строка содержит 'print'
            /*
                code (должна быть отдельная обработка списка аргументов к 'print' и создание выражения)
            */

            if(line[1].type != TOKEN_open_paren){
                printf("ERROR in %d line: The 'print' function requires a list of arguments in parentheses\n", line_number);
                return Failed_Parsing;
            }

            if(line[2].type == TOKEN_close_paren){
                printf("ERROR in %d line: Missed a list of arguments in 'print' function\n", line_number);
                return Failed_Parsing;
            }

            //аргументы к функции print могут передаваться через запятую и их необходимо парсить
            int args_count = 0;
            int last_paren_idx = tokens_count_in_line - 2;
            for(int i = 2; i < last_paren_idx; i++){
                args_count++;
            }

            //массив токенов всего выражения в скобках (включая запятые)
            TOKEN arguments[args_count];
            for(int i = 0; i < args_count; i++){
                arguments[i] = line[i+2];
            }

            //итоговый массив токенов-аргументов, прошедших парсинг
            int count_of_comma = comma_count(arguments, args_count);
            TOKEN* res_args = malloc((count_of_comma + 1) * sizeof(TOKEN));
            if(!res_args) {
                printf("ERROR: Memory error\n");
                return Failed_Parsing;
            }


            //пропускаем аргументы через функцию парсинга выражений и добавляем к массиву res_args
            int res_args_count = 0;
            int tokens_in_arg = 0;

            for(int i = 0; i <= count_of_comma; i++){
                TOKEN argument[100];
                int count = 0;
                while(arguments[tokens_in_arg].type != TOKEN_comma && tokens_in_arg < args_count){
                    argument[count] = arguments[tokens_in_arg];
                    tokens_in_arg++;
                    count++;
                }

                TOKEN* res_arg = parse_expression(argument, count);
                if(!res_arg){
                    printf("ERROR in %d line: Failed to parse 'print' func arguments\n", line_number);
                    return Failed_Parsing;
                }

                //добавляем результирующий токен в массив res_args
                if(add_argument(res_args, res_arg, i) != 1){
                    printf("ERROR in %d line: Failed to add argument number %d", line_number, i);
                    return Failed_Parsing;
                }

                res_args_count++;
                tokens_in_arg++;
                free_token(res_arg);
            }

            if(execute_print(res_args, res_args_count) != Success_Executing){
                printf("ERROR in %d line: Execution error\n", line_number);
                free(res_args);
                return Failed_Parsing;
            }

            for(int i = 0; i < res_args_count; i++){
                free(res_args[i].text);
                res_args[i].text = NULL;
            }
            free(res_args);

            line_number++;
        }

        //обработка условных выражений
        else if(line[0].type == TOKEN_if){
            if(line[1].type != TOKEN_open_paren){
                fprintf(stderr, "ERROR in %d line: Missed symbol '(' after 'if' keyword\n", line_number);
                return Failed_Parsing;
            }
            if(line[2].type == TOKEN_close_paren){
                fprintf(stderr, "ERROR in %d line: Missed a condition to 'if'\n", line_number);
                return Failed_Parsing;
            }

            TOKEN condition[CONDITION_SIZE];
            int tokens_in_cond_count = 0;
            int idx = 2;
            for(; tokens_in_cond_count < CONDITION_SIZE; idx++, tokens_in_cond_count++){
                if(line[idx].type == TOKEN_close_paren){
                    break;
                }

                condition[tokens_in_cond_count] = line[idx];
            }

            if(line[idx].type != TOKEN_close_paren){
                fprintf(stderr, "ERROR in %d line: Too big condition to 'if'\n", line_number);
                return Failed_Parsing;
            }

            idx++;
            if(line[idx].type != TOKEN_open_curly_paren){
                fprintf(stderr, "ERROR in %d line: Missed symbol '{' after 'if' condition\n", line_number);
                return Failed_Parsing;
            }
            idx++;
            if(line[idx].type == TOKEN_close_curly_paren){
                fprintf(stderr, "ERROR in %d line: Missed a body of condition\n", line_number);
                return Failed_Parsing;
            }

            TOKEN* parsed_condition = parse_expression(condition, tokens_in_cond_count);
            if (!parsed_condition) {
                fprintf(stderr, "ERROR in %d line: Failed to parse 'if' condition\n", line_number);
                return Failed_Parsing;
            }

            //для формирования подпрограммы (тела цикла) находим с помощью индекса k первую открывающую фигурную скобку
            //так как в теле цикла могут быть другие подпрограммы, необходимо учитывать уровни скобок
            
            int stream_idx = k;
            int close_curl_par_count = 0;
            int last_close_curl_par_idx = k;
            while(stream[stream_idx].type != TOKEN_open_curly_paren && stream[stream_idx].type != TOKEN_end){
                if(stream[stream_idx].type == TOKEN_close_curly_paren) {
                    close_curl_par_count++;
                    last_close_curl_par_idx = stream_idx;
                }
                stream_idx++;
            }  

            // Определяем, выполнять ли тело условия
            if (interpret_condition(parsed_condition) == LOGIC_TRUE) {
                
                TOKEN subroutine[STREAM_SIZE];
                int subroutine_tokens_count = 0; 

                stream_idx = k-1;
                
                int first_open_curl_par_idx = stream_idx;
                while(close_curl_par_count && stream[stream_idx].type != TOKEN_close_curly_paren){
                    if(stream[stream_idx].type == TOKEN_open_curly_paren) {
                        close_curl_par_count--;
                        first_open_curl_par_idx = stream_idx;
                    }
                    stream_idx--;
                }

                //Собираем токены тела условия
                idx = first_open_curl_par_idx+1;
                while (idx < last_close_curl_par_idx) {

                    if (stream[idx].type == TOKEN_semicolon) {
                        line_number++;
                    }

                    if (subroutine_tokens_count >= STREAM_SIZE) {
                        fprintf(stderr, "ERROR: Subroutine too large\n");
                        free_token(parsed_condition);
                        return Failed_Parsing;
                    }

                    subroutine[subroutine_tokens_count] = stream[idx];
                    idx++;
                    subroutine_tokens_count++;
                }


                PARSING_STATUS status = parsing(subroutine, subroutine_tokens_count);
                if (status != Successful_Parsing) {
                    free_token(parsed_condition);
                    return status;
                }

                k = last_close_curl_par_idx+1;
                continue;
            }

            free_token(parsed_condition);
            line_number++;

            k = last_close_curl_par_idx+1;
            continue;
        }

        else if(line[0].type == TOKEN_end){  //если строка содержит 'end' (то есть последняя строка)
            if(line[1].type != TOKEN_semicolon){  //если после 'end' нет точки с запятой - ошибка
                printf("ERROR in %d line: Missed symbol ';' after 'end' keyword\n", line_number);
                return Failed_Parsing;
            }

            //после 'end' останавливаем цикл, все что находится после не подлежит парсингу. Конец программы
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

                TOKEN right[150];
                int j = 0;  //кол-во токенов после '='
                for (int i = assingment_idx + 1; line[i].type != TOKEN_semicolon ; i++) {
                    right[j] = line[i];
                    j++;
                }

                //парсим rvlaue (массив токенов после знака '=')
                TOKEN* right_token = parse_expression(right, j);
                if(!right_token){
                    printf("ERROR in %d line: Failed to parse expression\n", line_number);
                    return Failed_Parsing;
                }

                if(execute_assign(&line[0], right_token) != Success_Executing){
                    printf("ERROR in %d line: Execution error\n", line_number);
                    free_token(right_token);
                    return Failed_Parsing;
                } 

                free_token(right_token);

                line_number++;
            }
        }
    }

    return Successful_Parsing;
}