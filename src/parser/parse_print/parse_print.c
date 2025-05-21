#include "parse_print.h"

int parse_print(TOKEN line[], int tokens_count_in_line, int* line_number){
    if(line[0].type != TOKEN_print){
        printf("ERROR in %d line: Incorrect 'print' function position\n", *line_number);
        return 1;
    }

    if(line[1].type != TOKEN_open_paren){
        printf("ERROR in %d line: The 'print' function requires a list of arguments in parentheses\n", *line_number);
        return 1;
    }

    if(line[2].type == TOKEN_close_paren){
        printf("ERROR in %d line: Missed a list of arguments in 'print' function\n", *line_number);
        return 1;
    }    

    printf("\n[%s]\n", line[tokens_count_in_line-2].text);

    if(line[tokens_count_in_line-2].type != TOKEN_close_paren){
        printf("ERROR in %d line: Incorrect position of close paren\n", *line_number);
        return 1;
    }    

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
        return 1;
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
            printf("ERROR in %d line: Failed to parse 'print' func arguments\n", *line_number);
            return 1;
        }

        //добавляем результирующий токен в массив res_args
        if(add_argument(res_args, res_arg, i) != 1){
            printf("ERROR in %d line: Failed to add argument number %d", *line_number, i);
            return 1;
        }

        res_args_count++;
        tokens_in_arg++;
        free_token(res_arg);
    }

    if(execute_print(res_args, res_args_count) != Success_Executing){
        printf("ERROR in %d line: Execution error\n", *line_number);
        free(res_args);
        return 1;
    }

    for(int i = 0; i < res_args_count; i++){
        free(res_args[i].text);
        res_args[i].text = NULL;
    }
    free(res_args);

    (*line_number)++;

    return 0;
}