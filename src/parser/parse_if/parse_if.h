#ifndef PARSE_IF_H
#define PARSE_IF_H

#include "../parser.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


int parse_condition(TOKEN condition[], int tokens_in_cond_count);

//есть ли знаки ">", "<", "==" в условии
bool has_comparison_marks(TOKEN condition[], int tokens_in_cond_count);

//принимает выражение по типу: "x > 3" и возвращает результат сравнения: 1 или 0
int simplify_condition(TOKEN condition[]);


int interpret_condition(TOKEN* condition);

int parse_if(TOKEN line[], int tokens_count_in_line, int* line_number, TOKEN stream[], int* code_idx);

//находим индекс парной закрывающей скобки для текущего условия
int find_matching_brace(TOKEN stream[], int start_idx, int tokens_count);

#endif