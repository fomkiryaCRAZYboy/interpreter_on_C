#ifndef LEXER_H
#define LEXER_H

//данное перечисление будет использоваться в качестве типа возвращаемого значения токенизации
typedef enum {
    Successful_Tokenization,
    Failed_Tokenization,
} TOKENIZATION_STATUS;

typedef enum {
    Successful_Add,
    Failed_Add,
} TOKEN_ADD_STATUS;


//перечисление типов токенов 
typedef enum {
    TOKEN_variable,  //переменная
    TOKEN_number,   //число
    TOKEN_math_operator, //математический оператор ( +, -, *, /)
    TOKEN_open_paren, // (
    TOKEN_close_paren, // )
    TOKEN_end,     //маркер явного конца программы
    TOKEN_assign,  // '=' присваивание
    TOKEN_semicolon, // ';' конец строки
    TOKEN_comma,   // ','
    TOKEN_space,  //  незначащий пробел
    TOKEN_print, // ключевое слово print (для вывода)
} TOKEN_TYPE;

//структура токена  
typedef struct {
    TOKEN_TYPE type; //тип токена
    char *text;     //текст токена
} TOKEN;


//токенизация, разбиение строки кода на токены
TOKENIZATION_STATUS tokenize(char* code_string);

//добавление сформированного токена в поток
TOKEN_ADD_STATUS add_token(TOKEN* token);

TOKEN* create_token(const TOKEN_TYPE type, const char* text);

void free_token(TOKEN* token);


#endif // LEXER_H