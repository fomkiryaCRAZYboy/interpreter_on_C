#ifndef __LEXER_H__
#define __LEXER_H__


//перечисление типов токенов 
typedef enum {
    TOKEN_value,  //переменная
    TOKEN_number,   //число
    TOKEN_math_operator, //математический оператор ( +, -, *, /)
    TOKEN_open_paren, // (
    TOKEN_close_paren, // )
    TOKEN_eof,     //конец файла
    TOKEN_assign,  // '=' присваивание
    TOKEN_semicolon, // ';' конец строки
    TOKEN_comma,   // ,
    TOKEN_space,  //  незначащий пробел
    TOKEN_print, // ключевое слово print (для вывода)
} TOKEN_TYPE;

//структура токена  
typedef struct {
    TOKEN_TYPE type; //тип токена
    char *text;     //текст токена
} TOKEN;


//токенизация, разбиение строки кода на токены
void tokenize(const char* code_string);

//добавление сформированного токена в поток
void add_token(TOKEN* token);

TOKEN* create_token(const TOKEN_TYPE type, const char* text);

void free_token(TOKEN* token);


#endif // __LEXER_H__