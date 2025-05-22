#include "parse_if.h"

//парсинг условия 
//if(x > (15 > 3 + 2))

bool has_comparison_marks(TOKEN condition[], int tokens_in_cond_count)
{
    int i = 0;
    while(i < tokens_in_cond_count)
    {
        if(condition[i].type == TOKEN_equal ||
           condition[i].type == TOKEN_more  ||
           condition[i].type == TOKEN_less) 
           {
            return true;
           }

        ++i;
    }
    
    return false;
}



int simplify_condition(TOKEN condition[])
{
    TOKEN left_token = condition[0];

    double left_value;
    if(left_token.type == TOKEN_number)
    {
        //если токен - число, то берем его значение
        left_value = atoi(left_token.text);
    } 
    else 
    {
        //если токен - строка, берём длину строки
        left_value = strlen(left_token.text);
    }


    TOKEN right_token = condition[2];

    double right_value;
    if(right_token.type == TOKEN_number)
    {
        //если токен - число, то берем его значение
        right_value = atoi(right_token.text);
    } 
    else 
    {
        //если токен - строка, берём длину строки
        right_value = strlen(right_token.text);
    }


    if(condition[1].type == TOKEN_equal)
    {
        return left_value == right_value;
    } 
    else if(condition[1].type == TOKEN_more)
    {
        return left_value > right_value;
    }
    else if(condition[1].type == TOKEN_less)
    {
        return left_value < right_value;
    }
}



int parse_condition(TOKEN condition[], int tokens_in_cond_count)
{
    if (tokens_in_cond_count == 0) {
        fprintf(stderr, "ERROR: Empty tokens array\n");
        return -1;
    }

    bool has_compare_marks = has_comparison_marks(condition, tokens_in_cond_count);

    //если в условии нет знаков сравнения, то его можно запарсить как обычное выражение
    if(!has_compare_marks)
    {
        TOKEN* parsed_condition = parse_expression(condition, tokens_in_cond_count);

        /*
            функция parse_expression() вернёт токен числа или строки, поэтому вызываю 
            для него функцию interpret_condition(), которая в свою очередь
            вернёт 0 или 1
        */
        int interpreted_condition = interpret_condition(parsed_condition);
        free_token(parsed_condition);

        return interpreted_condition;
    }


    for(int cond_idx = 0; cond_idx < tokens_in_cond_count; ++cond_idx)
    {
        //ищем первый знак сравнения и собираем левую и правую части от него
        
    }
}