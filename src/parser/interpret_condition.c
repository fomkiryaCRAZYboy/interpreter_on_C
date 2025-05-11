#include "parser.h"
#include <stdio.h>
#include <string.h>


//так как функция parse_expression() возвращает только токены чисел или строк(даже если была передена переменная) то проверяем только токены чисел или строк
CONDITION_STATUS interpret_condition(TOKEN* condition){
    if(condition->type == TOKEN_number){
        //если значение нулевое - значит условие ложное
        if(strcmp(condition -> text, "0") == 0){
            return LOGIC_FALSE;
        } else {
            return LOGIC_TRUE;
        }
    } //если не число - значит строка
    //если строка пустая - ложь, иначе - правда
    else{
        if(strlen(condition -> text) == 0){
            return LOGIC_FALSE;
        } else {
            return LOGIC_TRUE;
        }
    }
}