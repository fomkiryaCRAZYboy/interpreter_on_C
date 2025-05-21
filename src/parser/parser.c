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
    for(int i = 0; i < tokens_count; i++){  //считаем кол-во символов ';' в коде
        if(stream[i].type == TOKEN_semicolon){
            semicolon_count++;
        }
    }
    
    int code_idx = 0; //индекс для перебора токенов всего кода
    int line_number = 1;
    
    for(int i = 0; i < semicolon_count; i++){
        TOKEN line[200];
        int j = 0;

        int subroutine_exist = 0;
        while(stream[code_idx].type != TOKEN_semicolon && code_idx< tokens_count){  //формируем массив токенов одной строки для дальнейшего парсинга
            if(stream[code_idx].type == TOKEN_space) { //сразу пропускаем незнаяещие пробелы
                code_idx++;
                continue;
            }

            //если встетился токен '{', то формирую 'строку' до момента, пока не встречу '}'
            if(stream[code_idx].type == TOKEN_open_curly_paren){
                subroutine_exist = 1;
                int paren_blance = 0; //баланс фигурных скобок
                
                //добавляю саму фигурную скобку
                line[j++] = stream[code_idx++];

                while(code_idx < tokens_count){
                    if(stream[code_idx].type == TOKEN_open_curly_paren){
                        paren_blance++;
                    }
                    if(stream[code_idx].type == TOKEN_close_curly_paren){
                        if(paren_blance){
                            paren_blance--;
                        } 
                        else {
                            break;
                        }
                    }
                    line[j++] = stream[code_idx++];                    
                }

                if(code_idx < tokens_count){
                    line[j++] = stream[code_idx++];
                }
                break;
            }

            line[j++] = stream[code_idx++];
        }
        //если цикл while закончился и code_idx< меньше кол-ва токенов, значит он прервался из-за знака ';' 
        if(!subroutine_exist){
            if(code_idx< tokens_count){ //добавляем этот токен к строке
                line[j++] = stream[code_idx++];
            }  
        }
          
        int tokens_count_in_line = j;  //опредеяем кол-во токенов в строке

        /*printf("\n--LINE--\n");
        for(int i = 0; i < tokens_count_in_line; ++i){
            printf("%s", line[i].text);
        }
        puts("");*/

        if(line[0].type == TOKEN_print) {  //если строка содержит 'print'
            if(parse_print(line, tokens_count_in_line, &line_number) != 0){
                return Failed_Parsing;
            }
        }

        //обработка условных выражений
        else if(line[0].type == TOKEN_if){

            if(parse_if(line, tokens_count_in_line, &line_number, stream, &code_idx) != 0){
                return Failed_Parsing;
            }
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