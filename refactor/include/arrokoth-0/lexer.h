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

#ifndef ARROKOTH_LEXER_H
#define ARROKOTH_LEXER_H

#include <stdint.h>

#include <arrokoth-0/linked_list.h>

typedef enum token_type_e
{
    TT_IDENTIFIER,
    TT_STATEMENT_DELIMITER,
    TT_LEFT_PAREN,
    TT_RIGHT_PAREN,
    TT_OPERATOR,
    TT_NUMBER,
    TT_COMMA,
    TT_LEFT_BRACKET,
    TT_RIGHT_BRACKET,
    TT_PRINT,
    TT_FORMATTING,
} token_type_t;

typedef struct token_s
{
    size_t line;
    token_type_t type;
    char* content;

    // NULL for end of token stream.
    struct token_s* next;
}
*token_t;

token_t DoLexicalAnalysis(FILE* fd);

#endif // ARROKOTH_LEXER_H
