#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "../lexer/lexer.h"
#include "parse_print/parse_print.h"
#include "parse_if/parse_if.h"

#define CONDITION_SIZE  40

#define MAX_VARIABLES_COUNT 100 //можно объявить до ста переменных в коде
#define MAX_AST_COUNT   250 // до 250 узлов может быть в программе

//определяю тип возвращаемого значения для основной функции парсинга
typedef enum {
    Successful_Parsing,
    Failed_Parsing
} PARSING_STATUS;

PARSING_STATUS parsing(TOKEN stream[], int tokens_count); //основная функция парсинга (разбивает массив токенов по строкам и анализирует их)


 
typedef struct{
    TOKEN node; //узел, связывающий два лепестка 
    TOKEN right; //правый лепесток
    TOKEN left;  //левый лепесток
    TOKEN* print_arguments; //список аргументов-токенов для функции принт (если она есть) 
    int count_print_args; 
} AST;

AST* create_AST(const TOKEN* node, const TOKEN* right, const TOKEN* left, const TOKEN* print_arguments, const int args_count);

TOKEN* parse_expression(TOKEN tokens[], int tokens_count);


//в структуре VARIABLE объединение DATA_TYPE для разных типов данных
typedef union {
    int int_value;
    double double_value;
    char string_value[MAX_STRING_LEN];
} DATA_TYPE;

//в структуре VARIABLE будет храниться один из типов данных, к которому принадлежит переменная
typedef enum {
    INT_TYPE, 
    DOUBLE_TYPE,
    STRING_TYPE,
} USING_TYPE;

//структура переменной
typedef struct {
    char* name; //название переменной
    DATA_TYPE value; //значение переменной
    USING_TYPE type; //тип переменной
} VARIABLE;


typedef enum {
    Successful_add_var,
    Failed_add_var,
} VAR_ADD_STATUS;

//создание переменной
VAR_ADD_STATUS create_and_add_variable(const char* name, const DATA_TYPE value, const USING_TYPE type); 


//проверка наличия переменной
bool check_variable_exists(const char* name);

int get_variable_index(const char* name);


//проверка на количество переданных аргументов для print(). возвращает кол-во запятых между аргументами (разделители)
int comma_count(TOKEN arguments[], int tokens_count);

//добавление запаршенного токена-аргумента в результирующий массив аргументов функции 'print'
int add_argument(TOKEN* arguments_array, TOKEN* argument, int index); //index - индекс нового аргумента в массиве

#endif