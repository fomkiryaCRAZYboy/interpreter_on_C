#include "parse_if.h"

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

int parse_if(TOKEN line[], int tokens_count_in_line, int* line_number, TOKEN stream[], int* code_idx){
    if(line[1].type != TOKEN_open_paren){
        fprintf(stderr, "ERROR in %d line: Missed symbol '(' after 'if' keyword\n", *line_number);
        return 1;
    }
    if(line[2].type == TOKEN_close_paren){
        fprintf(stderr, "ERROR in %d line: Missed a condition to 'if'\n", *line_number);
        return 1;
    }

    TOKEN condition[CONDITION_SIZE];
    int tokens_in_cond_count = 0;
    int idx = 2;
    for(; tokens_in_cond_count < CONDITION_SIZE; idx++, tokens_in_cond_count++){
        if(line[idx].type == TOKEN_open_curly_paren || line[idx].type == TOKEN_semicolon){
            fprintf(stderr, "ERROR in %d line: Missed symbol '(' after 'if' condition\n", *line_number);
            return 1;
        }
        if(line[idx].type == TOKEN_close_paren){
            break;
        }

        condition[tokens_in_cond_count] = line[idx];
    }

    if(line[idx].type != TOKEN_close_paren){
        fprintf(stderr, "ERROR in %d line: Too big condition to 'if'\n", *line_number);
        return 1;
    }

    idx++;
    if(line[idx].type != TOKEN_open_curly_paren){
        fprintf(stderr, "ERROR in %d line: Missed symbol '{' after 'if' condition\n", *line_number);
        return 1;
    }

    int first_open_curl_par_idx = idx; 
    int last_close_curl_par_idx = tokens_count_in_line-1;

    idx++;
    if(line[idx].type == TOKEN_close_curly_paren){
        fprintf(stderr, "ERROR in %d line: Missed a body of condition\n", *line_number);
        return 1;
    }

    TOKEN* parsed_condition = parse_expression(condition, tokens_in_cond_count);
    if (!parsed_condition) {
        fprintf(stderr, "ERROR in %d line: Failed to parse 'if' condition\n", *line_number);
        return 1;
    }

    // Определяем, выполнять ли тело условия
    if (interpret_condition(parsed_condition) == LOGIC_TRUE) {
        
        TOKEN subroutine[STREAM_SIZE];
        int subroutine_tokens_count = 0; 

        int open_curl_count = 0;

        //Собираем токены тела условия
        idx = first_open_curl_par_idx+1;
        while (idx < last_close_curl_par_idx) {

            if (line[idx].type == TOKEN_semicolon) {
                (*line_number)++;
            }

            if(line[idx].type == TOKEN_open_curly_paren) open_curl_count++;

            if (subroutine_tokens_count >= STREAM_SIZE) {
                fprintf(stderr, "ERROR: Subroutine too large\n");
                free_token(parsed_condition);
                return Failed_Parsing;
            }

            subroutine[subroutine_tokens_count] = line[idx];
            idx++;
            subroutine_tokens_count++;
        }

        printf("\n-SUBROUTINE-\n");
        for(int i = 0; i < subroutine_tokens_count; ++i){
            printf("[%s]", subroutine[i].text);
        }
        puts("");

        PARSING_STATUS status = parsing(subroutine, subroutine_tokens_count);
        free_token(parsed_condition);
        if (status != Successful_Parsing) {
            return 1;
        }
        
        return 0;
    }

    free_token(parsed_condition);
    (*line_number)++;
    return 0;
}