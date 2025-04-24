#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "../lexer/lexer.h"
#define MAX_AST_COUNT   250 // до 250 узлов может быть в программе

//определяю тип возвращаемого значения для основной функции парсинга
typedef enum {
    Successful_Parsing,
    Failed_Parsing
} PARSING_STATUS;

PARSING_STATUS parsing(TOKEN stream[], int tokens_count); //основная функция парсинга (разбивает массив токенов по строкам и анализирует их)


 
typedef struct{
    TOKEN node; //узел, связывающий два лепестка (может быть '=' или любая мат. операция)
    TOKEN right; //правый лепесток
    TOKEN left;  //левый лепесток
} AST;

AST* create_AST(const TOKEN* node, const TOKEN* right, const TOKEN* left);

TOKEN* parse_expression(TOKEN tokens[], int tokens_count);




//структура переменной
typedef struct {
    char* name; //название переменной
    int value; //значение переменной (пока только целочисленные)
} VARIABLE;


typedef enum {
    Successful_add_var,
    Failed_add_var,
} VAR_ADD_STATUS;

//создание переменной
VAR_ADD_STATUS add_variable(const char* name, const int value); 


//проверка наличия переменной
bool check_variable_exists(const char* name);

#endif