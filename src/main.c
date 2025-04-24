#include "lexer/lexer.h"
#include "../tests/lexer_tests/lexer_test.h"
#include "../tests/parser_tests/parser_test.h"

extern int tokens_count;
extern TOKEN stream[250];

int main(void)
{
    
    if(tokenize("x = 2;") != Successful_Tokenization) {
        printf("Tokenization Failed\n");
        return 1;
    }
    debug_print_stream();

    if(parsing(stream, tokens_count) != Successful_Parsing) {
        printf("Parsing Failed\n");
        return 1;
    }

    debug_print_AST_array();

    free_stream();

    return 0;
}


/*
----PASSED THE TEST----

"66 - (55)" : [NUM](66), [MATH_OP](-), [OPEN_PAR]((), [NUM](55), [CLOSE_PAR]())

"8 - 6 * 0" : [NUM](8), [MATH_OP](-), [NUM](6), [MATH_OP](*), [NUM](0)

";;;;;"     : [SEMICOL](;), [SEMICOL](;), [SEMICOL](;), [SEMICOL](;), [SEMICOL](;)

"5,0,1,2"   : [NUM](5), [COMMA](,), [NUM](0), [COMMA](,), [NUM](1), [COMMA](,), [NUM](2)


"x = 5; y = x + 2; end;" : [VAR](x), [ASSIGN](=), [NUM](5), [SEMICOL](;), [VAR](y), [ASSIGN](=), [VAR](x), [MATH_OP](+), [NUM](2), [SEMICOL](;), [END](end), [SEMICOL](;)

"var = 123; print(var);" : [VAR](var), [ASSIGN](=), [NUM](123), [SEMICOL](;), [PRINT](print), [OPEN_PAR]((), [VAR](var), [CLOSE_PAR]()), [SEMICOL](;)

............................
nums:
"576"
"0"
............................
math_operations:
"+"
"-"
"*"
"/"
............................
other_symbols:
"()"  две скобки
"("   одна скобка
"="
","
";"
............................
keywords:
"end" - [END](end)
"print" - [PRINT](print)
............................
variables:
"var"
"x"

*/

/*

" " - succes
""  - succes

*/