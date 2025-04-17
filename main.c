#include "lexer.h"
#include "tests/lexer_test.h"

int main(void)
{
    if(tokenize("x = 5") == Failed_Tokenization) {
        printf("Tokenization Failed");
        return 1;
    }
    debug_print_stream();
    free_stream();

    return 0;
}