#ifndef PARSER_H
#define PARSER_H
#include "../lexer/lexer.h"  //для типа TOKEN

#include <stdbool.h>


//возвращаемое значение функции parsing
typedef enum {
    Successful_Parsing,
    Failed_Parsing,
} PARSING_STATUS;

typedef enum {
    Failed_Add_Expression,
    Succsessful_Add_Expression,
} ADD_EXPRESSION_STATUS;


typedef enum {
    OPEN_OR_CLOSE_PAREN = 0, // ( или )
    COMMA = 2, // ,
    MATH_MULT_DIV = 4,  // * или /
    MATH_ADD_SUB = 6,   // + или  -      
    ASSIGN = 8,     // =
    ANOTHER = 10,   // другие типы токенов (переменные, ключевые слова, и т.д.)
    SEMICOLON = 12,  // ;
} OPERATOR_PRIORITY;  //0 - самый высокий приоритет, 4 - самый низкий


/*------------------------------------------------------------------------------------------------------------------------
из потока токенов строки сформируется поток выражений, например:

 "x = 2 * (5 + y)" ->
 (формирование потока токенов)
 -> [VAR](x), [ASSIGN](=), [NUM](2), [MATH_OP](*), [OPEN_PAR]((), [NUM](5), [MATH_OP](+), [VAR](y), [CLOSE_PAR]()) ->
 -> поток выражений: 
    1) res_1 = (5 + y)
    2) res_2 = 2 * res_1
    3) x = res_2
---------------------------------------------------------------------------------------------------------------------------
*/

//будет формироваться свой стек выражений
typedef struct {
    int line_number;
    char* expression;  //текстовое представление выражения
    char* keyword; //ключевые слова print, end (если их нет: NULL)
    char* arguments; //список аргументов (если есть print, иначе - NULL)
} EXPRESSION;

//структура, хранящая название переменной и ее значение (пока что только целочисленные значения)
typedef struct{
    char* name;
    int value;
} VARIABLES;


//основная функция для анализа потока токенов
PARSING_STATUS parsing(const TOKEN stream[], const int tokens_count);


//создание выражения
EXPRESSION* create_expression(const int line_number, const char* expr, const char* kword, const char* args);


//добавление выражения в поток
ADD_EXPRESSION_STATUS add_expression(EXPRESSION* expr);

//проверка существования переменной в таблице
bool variable_existence_check(char* var_name);

//функция получения проиритета токена
OPERATOR_PRIORITY get_priority(TOKEN token);

#endif