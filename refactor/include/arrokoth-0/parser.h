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



typedef struct
{
    enum {
        T_ATOM_IDEN,
        T_ATOM_NUMBER,
        T_ATOM_SUBEXPR,
    } type;
    union {
        double number;
        char* iden;
        void* expr; // TODO: fix type
    };
}
node_atom_t;


typedef struct node_expr_s
{
    enum type_expr_e {
        T_EXPR_ADD,
        T_EXPR_MINUS,
        T_EXPR_MULT,
        T_EXPR_DIV,
        T_EXPR_EXP,
        T_EXPR_EQUAL,
        T_EXPR_UNEQUAL,
        T_EXPR_GREATER,
        T_EXPR_LESSER,
        T_EXPR_GE,
        T_EXPR_LE,

        T_EXPR_ATOM,
    } kind;

    node_atom_t* leaf;

    struct node_expr_s* left;
    struct node_expr_s* right;
}
node_expr_t;





typedef struct
{
    void* content;

    enum {
        T_STMT_ASSIGN,
        T_STMT_CALL,
        T_STMT_WHILE,
        T_STMT_IF,
        T_STMT_PRINT,
    } type;
}
node_statement_t;


typedef struct
{
    char* destination;
    node_expr_t* source;
}
node_assign_t;

typedef struct
{
    char* target;
}
node_call_t;



typedef struct
{
    node_statement_t** content;
    uint32_t size;
}
node_block_t;



typedef struct
{
    node_expr_t* condition;
    node_block_t* body;
}
node_while_t;

typedef struct
{
    node_expr_t* condition;
    node_block_t* body;
}
node_if_t;

typedef struct
{
    node_expr_t* target;
}
node_print_t;

typedef struct
{
    char* name;
    node_block_t* body;
}
node_proc_t;

typedef struct 
{
    node_proc_t** procs;
    uint32_t procs_count;

    char** vars;
    uint32_t vars_count;
}
node_program_t;

node_program_t* ParseProg(stream_t stream);

#endif // ARROKOTH_PARSER_H
