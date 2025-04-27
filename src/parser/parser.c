#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

AST AST_array[MAX_AST_COUNT];
int ast_count = 0;

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

    // Копируем только нужное поле объединения
    if (type == INT_TYPE) {
        var.value.int_value = value.int_value;
    } else {
        var.value.double_value = value.double_value;
    }

    //сразу добавляем переменную в массив и увеличиваем variables_count
    variables_array[variables_count++] = var;

    return Successful_add_var;
}



AST* create_AST(const TOKEN* node, const TOKEN* right, const TOKEN* left, const TOKEN* print_arguments, const int args_count){
    if (!node || !right || !left || !print_arguments) {
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

    new_ast ->print_arguments = malloc(args_count * sizeof(TOKEN));
    if(!new_ast->print_arguments){
        free(new_ast -> left.text);
        free(new_ast -> right.text);
        free(new_ast -> node.text);
        free(new_ast);
        printf("ERROR: Memory error\n");
        return NULL;
    }
    for(int i = 0; i < args_count; i++){
        new_ast -> print_arguments[i].text = malloc(strlen(print_arguments[i].text) + 1);
        if(!new_ast -> print_arguments[i].text) {
            for(int j = 0; j < i; j++){
                free(new_ast -> print_arguments[j].text);
            }
            free(new_ast -> left.text);
            free(new_ast -> right.text);
            free(new_ast -> node.text);
            free(new_ast);
            printf("ERROR: Memory error\n");
            return NULL;
        }
        strcpy(new_ast -> print_arguments[i].text, print_arguments[i].text);
    }

    new_ast -> count_print_args = args_count;

    return new_ast;
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
            TOKEN arguments[args_count];
            for(int i = 0; i < args_count; i++){
                arguments[i] = line[i+2];
            }
            
            //итоговый массив токенов-аргументов, прошедших парсинг
            TOKEN* res_args = malloc((comma_count(arguments, args_count) + 1) * sizeof(TOKEN));
            if(!res_args) {
                printf("ERROR: Memory error\n");
                return Failed_Parsing;
            }


            //пропускаем аргументы через функцию парсинга выражений и добавляем к массиву res_args
            int res_args_count = 0;
            for(int i = 0; i < args_count; i++){
                TOKEN argument[100];
                int tokens_in_arg = 0;
                while(argument[i].type != TOKEN_comma){
                    argument[tokens_in_arg++] = arguments[i];
                }

                TOKEN* res_arg = parse_expression(argument, tokens_in_arg);
                if(!res_arg){
                    printf("ERROR in %d line: Failed to parse 'print' func arguments\n", line_number);
                    return Failed_Parsing;
                }

                //добавляем результирующий токен в массив res_args
                if(add_argument(res_args, res_arg, i) != 1){
                    printf("ERROR in %d line: Failed to add argument number %d", line_number, i);
                }

                res_args_count++;
                free_token(res_arg);
            }

            TOKEN* print_token = create_token(TOKEN_print, "print");
            if(!print_token) {
                free(res_args);
                printf("ERROR: Memory error\n");
                return Failed_Parsing;
            } 

            TOKEN* stub = create_token(TOKEN_space, " ");
            if(!stub) {
                free_token(print_token);
                free(res_args);
                printf("ERROR: Memory error\n");
                return Failed_Parsing;
            } 

            TOKEN* stub2 = create_token(TOKEN_space, " ");
            if(!stub2) {
                free_token(stub);
                free_token(print_token);
                free(res_args);
                printf("ERROR: Memory error\n");
                return Failed_Parsing;
            } 

            AST* print_ast = create_AST(print_token, stub, stub2, res_args, res_args_count);
            if(ast_count >= MAX_AST_COUNT){
                printf("ERROR in %d line: Too many AST nodes", line_number);
                free_token(print_token);
                free_token(stub);
                free_token(stub2);
                free(res_args);
                return Failed_Parsing;
            }

            AST_array[ast_count++] = *print_ast;
            //освобождаем память от уже скопированного массива
            free(res_args);
            free_token(print_token);
            free_token(stub);
            free_token(stub2);

            line_number++;
        }

        else if(line[0].type == TOKEN_end){  //если строка содержит 'end' (то есть последняя строка)
            if(line[1].type != TOKEN_semicolon){  //если после 'end' нет точки с запятой - ошибка
                printf("ERROR in %d line: Missed symbol ';' after 'end' keyword\n", line_number);
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

            TOKEN stub3[] = {0};

            AST* end_ast = create_AST(end_token, stub, stub2, stub3, 0);

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

                TOKEN stub[] = {0};

                /*необходимо сформировать AST с корнем в виде токена '=' и лепестками в виде lvalue и rvalue*/
                AST* ast = create_AST(assign_token, right_token, &line[0], stub, 0);
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