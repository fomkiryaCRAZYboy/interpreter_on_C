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
    TOKEN_assign,  //=  
    TOKEN_semicolon, // ;
    TOKEN_comma,   // ,
    TOKEN_space,  // пробел
    TOKEN_print, // ключевое слово print (для вывода)
} TOKEN_TYPE;

//структура токена  
typedef struct {
    TOKEN_TYPE type; //тип токена
    char *text;     //текст токена
    int line;       //номер строки
} TOKEN;

#endif // __LEXER_H__