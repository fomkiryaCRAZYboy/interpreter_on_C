#include "lexer/lexer.h"
#include "../tests/lexer_tests/lexer_test.h"
#include "../tests/parser_tests/parser_test.h"

extern TOKEN stream[];
extern int tokens_count;

int main(void)
{
    
    if(tokenize("print(5);") != Successful_Tokenization) {
        printf("Tokenization Failed\n");
        return 1;
    }
    debug_print_stream();

    printf("\n");
    
    if(parsing(stream, tokens_count) != Successful_Parsing) {
        printf("Parsing Failed\n");
        return 1;
    }

    debug_print_AST_array();

    free_stream();

    return 0;
    
}

