#ifndef PARSER_H
#define PARSER_H
#include "../lexer/lexer.h"  //для типа TOKEN

//возвращаемое значение функции parsing
typedef enum {
    Successful_Parsing,
    Failed_Parsing
} PARSING_STATUS;


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
    EXPRESSION* next_expression;  //указатель на выражение, которое следует вычислять следующим
    char* expression;  //текстовое представление выражения
} EXPRESSION;

//функция для анализа потока токенов и формирования выражений
PARSING_STATUS parsing(const TOKEN stream[], const int tokens_count);

//функция получения проиритета токена
OPERATOR_PRIORITY get_priority(TOKEN token);


#endif