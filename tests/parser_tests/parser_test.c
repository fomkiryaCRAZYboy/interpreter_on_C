#include "parser_test.h"

extern AST AST_array[MAX_AST_COUNT];
extern int ast_count;

void debug_print_AST_array(){
    printf("--------------------\n");
    printf("ast_count = %d\n", ast_count);
    printf("--------------------\n");
    
    for(int i = 0; i < ast_count; i++){
        printf("********************************\n");
        printf("LINE NUMBER %d\n", i+1);
        printf("Node type: %s (text of token: '%s')\n", get_type_of_token(AST_array[i].node.type), AST_array[i].node.text);
        printf("Left branch type: %s (text of branch: '%s')\n", get_type_of_token(AST_array[i].left.type), AST_array[i].left.text);
        printf("Right branch type: %s (text of branch: '%s')\n", get_type_of_token(AST_array[i].right.type), AST_array[i].right.text);  
        puts("Print arguments (if exist):");
        for(int k = 0; k < AST_array[i].count_print_args; k++){
            printf("(%s) , ", AST_array[i].print_arguments[k].text);
        }
        puts("");
        printf("********************************\n"); 
    }
}