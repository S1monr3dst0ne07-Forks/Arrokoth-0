/*
Arrokoth-0 - A simple, basic programming language.
Copyright (C) 2026  NorbCodes

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ARROKOTH_PARSER_H
#define ARROKOTH_PARSER_H

#include <stdint.h>

#include <arrokoth-0/linked_list.h>

enum ParserTokenType
{
    PROGRAM_TOKEN = 0,
    STATEMENT_TOKEN,
    ASSIGNMENT_TOKEN,
    FUNCTION_TOKEN,
    EXPRESSION,
    UNARY_MINUS,
    BIN_OP,
    TERM,
    FACTOR,
    ATOM,
    IDENTIFIER_TOKEN,
    NUMBER_TOKEN,
    STRING_TOKEN,
    BLOCK_TOKEN,
    VAR_CREATION,
    PROC_CREATION,
    WHILE_LOOP,
    IF_BRANCH
};

enum BinOpType
{
    PLUS = 0,
    MINUS,
    MULT,
    DIV,
    EXP
};

enum CompBinOpType
{
    EQUAL = 0,
    NOT_EQUAL,
    GREATER,
    LESSER,
    GREATER_OR_EQ,
    LESSER_OR_EQ
};

typedef struct token_wrapper_s
{
    enum ParserTokenType Type;
    void* Data;
}
token_wrapper_t;

typedef struct token_vector_s
{
    size_t MaxSize;
    size_t CurrSize;
    token_wrapper_t* List;
}
token_vector_t;

void TokenVectorInit(token_vector_t* TV);
void TokenVectorAppend(token_vector_t* TV, token_wrapper_t TokenWrapper);

// Forward declare
struct program_token_s;
struct statement_token_s;
struct assignment_token_s;
struct function_token_s;
struct identifier_token_s;
struct expression_token_s;
struct unary_minus_token_s;
struct bin_op_token_s;
struct term_token_s;
struct factor_token_s;
struct atom_token_s;
struct number_token_s;
struct string_token_s;
struct block_token_s;
struct var_creation_token_s;
struct proc_creation_token_s;
struct while_loop_token_s;
struct if_branch_token_s;

typedef struct program_token_s
{
    token_vector_t Blocks;
}
program_token_t;

typedef struct statement_token_s
{
    token_wrapper_t Child;
}
statement_token_t;

typedef struct assignment_token_s
{
    token_wrapper_t Name;
    token_wrapper_t Val;
}
assignment_token_t;

typedef struct function_token_s
{
    token_wrapper_t FuncName;
}
function_token_t;

typedef struct identifier_token_s
{
    char* Text;
}
identifier_token_t;

typedef struct expression_token_s
{
    token_wrapper_t Child;
}
expression_token_t;

typedef struct unary_minus_token_s
{
    token_wrapper_t Child;
}
unary_minus_token_t;

typedef struct bin_op_token_s
{
    enum BinOpType Operator;
    token_wrapper_t L;
    token_wrapper_t R;
}
bin_op_token_t;

typedef struct term_token_s
{
    token_wrapper_t Child;
}
term_token_t;

typedef struct factor_token_s
{
    token_wrapper_t Child;
}
factor_token_t;

typedef struct atom_token_s
{
    token_wrapper_t Child;
}
atom_token_t;

typedef struct number_token_s
{
    double Val;
}
number_token_t;

typedef struct string_token_s
{
    char* Text;
}
string_token_t;

typedef struct block_token_s
{
    token_wrapper_t Child;
}
block_token_t;

typedef struct var_creation_token_s
{
    token_vector_t Children;
}
var_creation_token_t;

typedef struct proc_creation_token_s
{
    token_wrapper_t Name;
    token_vector_t Statements;
}
proc_creation_token_t;

typedef struct while_loop_token_s
{
    enum CompBinOpType OpType;
    token_wrapper_t L;
    token_wrapper_t R;
    token_vector_t Statements;
}
while_loop_token_t;

typedef struct if_branch_token_s
{
    enum CompBinOpType OpType;
    token_wrapper_t L;
    token_wrapper_t R;
    token_vector_t Statements;
}
if_branch_token_t;

program_token_t* DoParseAST(linked_list_node_t* LexTokens);

#endif // ARROKOTH_PARSER_H