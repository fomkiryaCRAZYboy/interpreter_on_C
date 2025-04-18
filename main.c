#include "lexer.h"
#include "tests/lexer_test.h"

int main(void)
{
    
    if(tokenize("var = 123; print(var);") != Successful_Tokenization) {
        printf("Tokenization Failed\n");
        return 1;
    }
    debug_print_stream();
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