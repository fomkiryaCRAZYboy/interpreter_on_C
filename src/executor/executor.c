#include "../parser/parser.h"
#include "executor.h"
#include <string.h>

#include <stdio.h>
#include <stdlib.h>


extern VARIABLE variables_array[MAX_VARIABLES_COUNT];
extern int variables_count;

/*
EXECUTING_STATUS executing(AST ast_array[], int ast_count){
    for(int i = 0 ; i < ast_count; i++){
        //если узел AST - это знак присваивания, значит слева от него - переменная, а справа - значение, которое необходимо переменной присвоить
        if(ast_array[i].node.type == TOKEN_assign){
            //проверяем тип rvalue токена (чтобы понять какой именно задать тип для переменной)
            char* ptr_float = strchr(ast_array[i].right.text, '.'); //ищем плавающую точку в числе
            //проверяем, существует ли на данный момент переменная
            if(check_variable_exists(ast_array[i].left.text)){
                //если существует
                //ищем индекс переменной в массиве переменных
                int var_idx_in_array = 0;
                while(strcmp(variables_array[var_idx_in_array].name, ast_array[i].left.text) != 0) var_idx_in_array++;

                if(ptr_float){
                    //меняем тип переменной и присваиваем её новое значение
                    variables_array[var_idx_in_array].type = DOUBLE_TYPE;
                    variables_array[var_idx_in_array].value.double_value = aotf(ast_array[i].right.text);
                } else{
                    variables_array[var_idx_in_array].type = INT_TYPE;
                    variables_array[var_idx_in_array].value.int_value = aoti(ast_array[i].right.text);
                }
            } else {
                //если переменной нет в массиве - её необходимо создать
                DATA_TYPE value;
                USING_TYPE type;
                if(ptr_float){
                    value.double_value = aotf(ast_array[i].right.text);
                    type = DOUBLE_TYPE;
                } else{
                    value.int_value = aoti(ast_array[i].right.text);
                    type = INT_TYPE;
                }
                if(create_and_add_variable(ast_array[i].left.text, value, type) != Successful_add_var) {
                    printf("ERROR in %d line: Failed to create or add variable\n", i+1);
                    return Failed_Executing;
                }
            }
        }
    }

    return Success_Executing;
}
*/

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
        } else {  //правая часть - переменная (она существует, так как прошла парсинг)
            int right_var_idx = 0;
            while(strcmp(variables_array[right_var_idx].name, right -> text) != 0) right_var_idx++;
            //меняем тип и значение переменной
            variables_array[var_idx_in_array].type = variables_array[right_var_idx].type;

            if(variables_array[right_var_idx].type == INT_TYPE){
                variables_array[var_idx_in_array].value.int_value = variables_array[right_var_idx].value.int_value;
            } else variables_array[var_idx_in_array].value.double_value = variables_array[right_var_idx].value.double_value;
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
        } //rvalue - переменная
        else {
            int right_var_idx = 0;
            while(strcmp(variables_array[right_var_idx].name, right -> text) != 0) right_var_idx++;

            DATA_TYPE value;
            USING_TYPE type;

            if(variables_array[right_var_idx].type == INT_TYPE){
                type = INT_TYPE;
                value.int_value = variables_array[right_var_idx].value.int_value;
            } else {
                type = DOUBLE_TYPE;
                value.double_value = variables_array[right_var_idx].value.double_value;
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
        if(arguments[i].type == TOKEN_variable){
            //ищем переменную в массиве переменных
            int var_idx = 0;
            while(strcmp(arguments[i].text, variables_array[var_idx].name) != 0) var_idx++;

            if(variables_array[var_idx].type == INT_TYPE){
                //выполняем функцию print
                printf("%d ", variables_array[var_idx].value.int_value);
            } else //выполняем функцию print
                printf("%.2f ", variables_array[var_idx].value.double_value);
        }  //если агумент - число
        else {
            char* ptr_float = strchr(arguments[i].text, '.'); //ищем плавающую точку в числе
            if(ptr_float) {
                //выполняем функцию print
                printf("%.2f ", atof(arguments[i].text));
            } else printf("%d ", atoi(arguments[i].text));
        }
    }
    
    return Success_Executing;
}
   
