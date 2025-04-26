#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_VARIABLES_COUNT 100 //можно объявить до ста переменных в коде

AST AST_array[MAX_AST_COUNT];
int ast_count = 0;


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

VAR_ADD_STATUS add_variable(const char* name, const int value){
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
    strcpy(var.name, name);

    var.value = value;

    //сразу добавляем переменную в массив и увеличиваем variables_count
    variables_array[variables_count++] = var;
    return Successful_add_var;
}



AST* create_AST(const TOKEN* node, const TOKEN* right, const TOKEN* left){
    if (!node || !right || !left) {
        printf("ERROR: Null token passed to create_AST\n");
        return NULL;
    }

    AST* new_ast = malloc(sizeof(AST));
    if(!new_ast){
        printf("ERROR: Memory error\n");
        return NULL;
    }

    new_ast -> node = *node;
    new_ast -> right = *right;
    new_ast -> left = *left;

    if(node ->text){
        new_ast -> node.text = malloc(strlen(node ->text) + 1);
        if(!new_ast -> node.text){
            free(new_ast);
            printf("ERROR: Memory error\n");
            return NULL;
        }
        strcpy(new_ast -> node.text, node->text);
    }

    if(right ->text){
        new_ast -> right.text = malloc(strlen(right ->text) + 1);
        if(!new_ast -> right.text){
            free(new_ast -> node.text);
            free(new_ast);
            printf("ERROR: Memory error\n");
            return NULL;
        }
        strcpy(new_ast -> right.text, right->text);
    }

    if(left ->text){
        new_ast -> left.text = malloc(strlen(left ->text) + 1);
        if(!new_ast -> left.text){
            free(new_ast -> right.text);
            free(new_ast -> node.text);
            free(new_ast);
            printf("ERROR: Memory error\n");
            return NULL;
        }
        strcpy(new_ast -> left.text, left->text);
    }

    return new_ast;
}

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
            if(!check_variable_exists(tokens[0].text)){
                printf("ERROR: variable '%s' not declared\n", tokens[0].text);
                return NULL;
            }

            int val;
            bool found = false;
            for(int i = 0; i < variables_count;i++) {
                if(strcmp(variables_array[i].name, tokens[0].text) == 0){
                    val = variables_array[i].value;
                    found = true;
                    break;
                }
            }

            if(!found){
                printf("ERROR: Variable '%s' not found\n", tokens[0].text);
                return NULL;
            }
            
            char text_val[] = {0};
            snprintf(text_val, 31, "%d", val);

            TOKEN* token = create_token(TOKEN_number, text_val);
            return token;
        }

        if(tokens[0].type == TOKEN_number){
            size_t text_len = strlen(tokens[0].text);

            char buff[31];
            strcpy(buff, tokens[0].text);

            TOKEN* token_num = create_token(TOKEN_number, buff); 
            if(!token_num){
                printf("ERROR: Memory error\n");
                return NULL;
            }
            return token_num;
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

        int close_par_idx = tokens_count - 1;
        while (tokens[close_par_idx].type != TOKEN_close_paren) close_par_idx--;

        int count = 0;
        while (count < 100 && open_par_idx+1 <= close_par_idx-1) {
            subtokens[count++] = tokens[open_par_idx++];
        }

        // сперва парсим то, что в скобках
        TOKEN* paren_expression =  parse_expression(subtokens, count);

        TOKEN updated_tokens[100];
        int new_count = tokens_count - (count + 2);

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
            double left_num;  //переменная для хранения численного значения токена с целью дальнейшего вычисления
            double right_num;

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
                    for(int i = 0; i < variables_count;i ++){
                        if(variables_array[i].name == left_result -> text){
                            left_num = variables_array[i].value;
                        }
                    }
                }
            }

            //если токен - число, то берем его значение
            left_num = atof(left_result -> text);

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
                    for(int i = 0; i < variables_count;i ++){
                        if(variables_array[i].name == right_result -> text){
                            right_num = variables_array[i].value;
                        }
                    }
                }
            }

            //если токен - число, то берем его значение
            right_num = atof(right_result -> text);

            // Выполняем операцию
            double result;
            
            if (strcmp(tokens[i].text, "*") == 0) {
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
                result = left_num / (double)right_num;
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
            snprintf(buffer, sizeof(buffer), "%.2f", result);
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

            free(updated_tokens[idx].text);
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

            // Проверяем, что оба операнда - числа или переменные
            double left_num;  //переменная для хранения численного значения токена с целью дальнейшего вычисления
            double right_num;

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
                    for(int i = 0; i < variables_count;i ++){
                        if(variables_array[i].name == left_result -> text){
                            left_num = variables_array[i].value;
                        }
                    }
                }
            } else {
                //если токен - число, то берем его значение
                left_num = atof(left_result -> text);   
            }

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
                    for(int i = 0; i < variables_count;i ++){
                        if(variables_array[i].name == right_result -> text){
                            right_num = variables_array[i].value;
                        }
                    }
                }
            } else {
                //если токен - число, то берем его значение
                right_num = atof(right_result -> text);
            }

            printf("%lf %lf", left_num, right_num);

            double result;
            
            if (strcmp(tokens[i].text, "+") == 0) {
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
            snprintf(buffer, sizeof(buffer), "%.2f", result);
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
        //если цикл while закончился и k < меньше кол-ва токенов, значит он прервался из-за знака ';' 
        if(k < tokens_count){ //добавляем этот токен к строке
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
            добавляем в массив AST 'end' и завершаем цикл (все что находится псле end обработано не будет)
            */

            //в качестве узла выступает токен 'end', правый и левый лепестки - незначащие пробелы (заглушки)
            TOKEN* end_token = create_token(TOKEN_end, "end");
            if(!end_token){
                printf("ERROR: Memory error\n");
                return Failed_Parsing;
            }
            TOKEN* stub = create_token(TOKEN_space, " ");
            if(!stub){
                free_token(end_token);
                printf("ERROR: Memory error\n");
                return Failed_Parsing;
            }
            TOKEN* stub2 = create_token(TOKEN_space, " ");
            if(!stub2){
                free_token(end_token);
                free_token(stub);
                printf("ERROR: Memory error\n");
                return Failed_Parsing;
            }

            AST* end_ast = create_AST(end_token, stub, stub2);

            if(ast_count >= MAX_AST_COUNT){
                printf("ERROR in %d line: Too many AST nodes", line_number);
                free_token(end_token);
                free_token(stub);
                free_token(stub2);
                return Failed_Parsing;
            }

            AST_array[ast_count++] = *end_ast;
            free_token(end_token);
            free_token(stub);
            free_token(stub2);

            line_number++;

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

                char buffer[] = "=";
                TOKEN* assign_token = create_token(TOKEN_assign, buffer);
                if(!assign_token){
                    printf("ERROR: Memory error\n");
                    return Failed_Parsing;
                }

                strcpy(assign_token->text, "=");
                assign_token->type = TOKEN_assign;

                /*необходимо сформировать AST с корнем в виде токена '=' и лепестками в виде lvalue и rvalue*/
                AST* ast = create_AST(assign_token, right_token, &line[0]);
                if(ast == NULL){
                    printf("ERROR in %d line: Failed to create AST", line_number);
                    return Failed_Parsing;
                }


                //ПОСЛЕ ТОГО, КАК СФОРМИРОВАЛИ AST для этой строки:
                //добавляем AST в массив 
                
                if(ast_count >= MAX_AST_COUNT){
                    printf("ERROR in %d line: Too many AST nodes", line_number);
                    free(right_token->text);
                    free(right_token);
                    free(ast);
                    return Failed_Parsing;
                }

                AST_array[ast_count++] = *ast;  //копируем данные

                free(right_token->text);
                free(right_token);
                free(ast);

                line_number++;
            }
        }
    }

    return Successful_Parsing;
}