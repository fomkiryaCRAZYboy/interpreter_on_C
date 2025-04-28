#include "lexer/lexer.h"
#include "../tests/lexer_tests/lexer_test.h"
#include "../tests/parser_tests/parser_test.h"

extern TOKEN stream[];
extern int tokens_count;

extern VARIABLE variables_array[MAX_VARIABLES_COUNT];
extern int variables_count;

int main(void)
{
    
    if(tokenize("x = 5; y = 10 * x; print(x+x); end;") != Successful_Tokenization) {
        printf("Tokenization Failed\n");
        return 1;
    }
    //debug_print_stream();

    printf("Output:\n");
    puts("---------------------------------------------");
    
    if(parsing(stream, tokens_count) != Successful_Parsing) {
        printf("Parsing Failed\n");
        return 1;
    }
    puts("\n---------------------------------------------");

    /*puts("//////////////");
    for(int i = 0; i < variables_count; i++){
        printf("var_name: '%s', value: %.2f", variables_array[i].name, variables_array[i].value.double_value);
    }
    puts("//////////////");*/

    //debug_print_AST_array();

    free_stream();

    return 0;
    
}

