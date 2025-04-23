#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "../lexer/lexer.h"


//определяю тип возвращаемого значения для основной функции парсинга
typedef enum {
    Successful_Parsing,
    Failed_Parsing
} PARSING_STATUS;

PARSING_STATUS parsing(TOKEN stream[], int tokens_count); //основна функция парсинга (разбивает массив токенов по строкам и анализирует их)



 
typedef struct {
    TOKEN node; //узел, связывающий два лепестка (может быть '=' или любая мат. операция)
    TOKEN* right; //правый лепесток
    char* left;  //левый лепесток
} AST;

AST* create_AST(const TOKEN node, const TOKEN* right, const char* left);

TOKEN* parse_expession(TOKEN tokens[], int tokens_count);


//структура переменной
typedef struct {
    char* name; //название переменной
    int value; //значение переменной (пока только целочисленные)
} VARIABLE;

//создание переменной
VARIABLE create_variable(const char* name, const int value); 

//проверка наличия переменной
bool check_variable_exists(const char* name);

#endif