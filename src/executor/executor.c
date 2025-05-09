#include "../parser/parser.h"
#include "executor.h"
#include <string.h>

#include <stdio.h>
#include <stdlib.h>


extern VARIABLE variables_array[MAX_VARIABLES_COUNT];
extern int variables_count;

void free_variables_array(){
    for(int i = 0; i < variables_count; i++){
        free(variables_array[i].name);
        variables_array[i].name = NULL;
    }    
}


//left - переменная(слева от '='), right - число или переменная (справа от '=')
EXECUTING_STATUS execute_assign(TOKEN* left, TOKEN* right){
    //проверяем, существует ли переменная на данный момент
    bool exist = check_variable_exists(left -> text);

    if(exist){
        //находим индекс переменной в массиве переменных
        int var_idx_in_array = 0;
        while(strcmp(variables_array[var_idx_in_array].name, left -> text) != 0) var_idx_in_array++;
         //если правая часть - число
        if(right -> type == TOKEN_number){
            char* ptr_float = strchr(right -> text, '.'); //ищем плавающую точку в числе
            
            if(ptr_float){
                //меняем тип переменной и присваиваем её новое значение
                variables_array[var_idx_in_array].type = DOUBLE_TYPE;
                variables_array[var_idx_in_array].value.double_value = atof(right -> text);
            } else{
                variables_array[var_idx_in_array].type = INT_TYPE;
                variables_array[var_idx_in_array].value.int_value = atoi(right -> text);
            }
        } else if (right -> type == TOKEN_string) {  //если правая часть строка
            variables_array[var_idx_in_array].type = STRING_TYPE;   
            strcpy(variables_array[var_idx_in_array].value.string_value, right -> text);
        }
        else {  //правая часть - переменная (она существует, так как прошла парсинг)
            int right_var_idx = 0;
            while(strcmp(variables_array[right_var_idx].name, right -> text) != 0) right_var_idx++;
            //меняем тип и значение переменной
            variables_array[var_idx_in_array].type = variables_array[right_var_idx].type;

            if(variables_array[right_var_idx].type == INT_TYPE){
                variables_array[var_idx_in_array].value.int_value = variables_array[right_var_idx].value.int_value;
            } else if(variables_array[right_var_idx].type == DOUBLE_TYPE) {
                variables_array[var_idx_in_array].value.double_value = variables_array[right_var_idx].value.double_value;
            } else {
                strcpy(variables_array[var_idx_in_array].value.string_value, variables_array[right_var_idx].value.string_value);
            }
        }  
      //если lvalue переменная не существует, её сперва необходимо создать  
    } else{
        if(right -> type == TOKEN_number){
            char* ptr_float = strchr(right -> text, '.'); //ищем плавающую точку в числе

            DATA_TYPE value;
            USING_TYPE type;

            if(ptr_float){
                type = DOUBLE_TYPE;
                value.double_value = atof(right -> text);
            } else {
                type = INT_TYPE;
                value.int_value = atoi(right -> text);
            }

            if(create_and_add_variable(left -> text, value, type) != Successful_add_var){
                printf("ERROR: Failed to create or add variable '%s'", left -> text);
                return Failed_Executing;
            }
        } else if(right -> type == TOKEN_string) {
            DATA_TYPE value;
            strcpy(value.string_value, right -> text);
            USING_TYPE type = STRING_TYPE;

            if(create_and_add_variable(left -> text, value, type) != Successful_add_var) {
                printf("ERROR: Failed to create or add variable '%s'", left -> text);
                return Failed_Executing;
            }
        }
        
        //rvalue - переменная
        else {
            int right_var_idx = 0;
            while(strcmp(variables_array[right_var_idx].name, right -> text) != 0) right_var_idx++;

            DATA_TYPE value;
            USING_TYPE type;

            if(variables_array[right_var_idx].type == INT_TYPE){
                type = INT_TYPE;
                value.int_value = variables_array[right_var_idx].value.int_value;
            } else if(variables_array[right_var_idx].type == DOUBLE_TYPE) {
                type = DOUBLE_TYPE;
                value.double_value = variables_array[right_var_idx].value.double_value;
            } else {
                type = STRING_TYPE;
                strcpy(value.string_value, variables_array[right_var_idx].value.string_value);
            }

            if(create_and_add_variable(left -> text, value, type) != Successful_add_var){
                printf("ERROR: Failed to create or add variable '%s'", left -> text);
                return Failed_Executing;
            }
        }    
    }

    return Success_Executing;
}


//принмимает массив аргуиентов к функции 'print', прошедших парсинг
//если аргументом будет переменная, то выводу подлежит её значение. 
//Переменные, находящиеся в этом массиве однозначно существуют, иначе интерпретация программы не дошла бы до этой функции
EXECUTING_STATUS execute_print(TOKEN* arguments, int arguments_count){
    for(int i = 0; i < arguments_count; i++){
      //если агумент - число
        if (arguments[i].type == TOKEN_number){
            char* ptr_float = strchr(arguments[i].text, '.'); //ищем плавающую точку в числе
            if(ptr_float) {
                //выполняем функцию print
                printf("%.2f ", atof(arguments[i].text));
            } else printf("%d ", atoi(arguments[i].text));
        } else {
            printf("%s ", arguments[i].text);
        }
    }
    
    return Success_Executing;
}
   
