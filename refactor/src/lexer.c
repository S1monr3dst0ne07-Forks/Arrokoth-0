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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <arrokoth-0/lexer.h>
#include <arrokoth-0/linked_list.h>

static char IsWhitespaceChar(char CH)
{
    return (CH >= 9 && CH <= 13) || (CH == 32);
}

static char IsStartingIdentifierChar(char CH)
{
    return (CH >= 'a' && CH <= 'z') || (CH >= 'A' && CH <= 'Z') || (CH == '_');
}

static char IsIdentifierChar(char CH)
{
    return (CH >= 'a' && CH <= 'z') || (CH >= 'A' && CH <= 'Z') || (CH >= '0' && CH <= '9') || (CH == '_') || (CH == '\'');
}

static char IsEMDASOperator(char CH)
{
    return (CH == '*') || (CH == '/') || (CH == '+') || (CH == '-') || (CH == '^');
}

static char IsGreaterOrSmallerOperator(char CH)
{
    return (CH == '>') || (CH == '<');
}

static char IsNumericalDigit(char CH)
{
    return (CH >= '0' && CH <= '9');
}

static char IsCommentStart(char CH)
{
    return (CH == '[');
}

lexical_token_t* NewToken(enum LexicalTokenType Type, char* Content, size_t Start, size_t End, size_t Line)
{
    lexical_token_t* Token = (lexical_token_t*)malloc(sizeof(lexical_token_t));
    memset(Token, 0, sizeof(lexical_token_t));
    Token->Type = Type;
    Token->Content = Content;
    Token->StartPos = Start;
    Token->EndPos = End;
    Token->Line = Line;
    return Token;
}

linked_list_node_t* DoLexicalAnalysis(const char* InputText, size_t Line)
{
    size_t CurrPos = 0;
    size_t SavePos = 0;
    linked_list_node_t* TokenList = NULL;

    while (CurrPos < strlen(InputText))
    {
        if (IsWhitespaceChar(InputText[CurrPos]))
        {
            do
            {
                CurrPos++;
            }
            while (IsWhitespaceChar(InputText[CurrPos]));
        }

        if (IsCommentStart(InputText[CurrPos]))
        {
            return TokenList;
        }

        if (IsStartingIdentifierChar(InputText[CurrPos]))
        {
            SavePos = CurrPos;
            do
            {
                CurrPos++;
            }
            while (IsIdentifierChar(InputText[CurrPos]));
            char* Content = (char*)calloc(CurrPos - SavePos + 1, 1);
            strncpy(Content, InputText + SavePos, CurrPos - SavePos);
            lexical_token_t* IdToken = NewToken(IDENTIFIER, Content, SavePos, CurrPos, Line);
            TokenList = LinkedListAppend(TokenList, IdToken);
        }

        if (IsNumericalDigit(InputText[CurrPos]))
        {
            SavePos = CurrPos;
            do
            {
                CurrPos++;
            }
            while (IsNumericalDigit(InputText[CurrPos]));
            if (InputText[CurrPos] == '.')
            {
                do
                {
                    CurrPos++;
                }
                while (IsNumericalDigit(InputText[CurrPos]));
            }
            char* Content = (char*)calloc(CurrPos - SavePos + 1, 1);
            strncpy(Content, InputText + SavePos, CurrPos - SavePos);
            lexical_token_t* NumToken = NewToken(NUMBER, Content, SavePos, CurrPos, Line);
            TokenList = LinkedListAppend(TokenList, NumToken);
        }

        if (InputText[CurrPos] == ';')
        {
            CurrPos++;
            lexical_token_t* StmtEnderToken = NewToken(STATEMENT_DELIMITER, ";", CurrPos-1, CurrPos-1, Line);
            TokenList = LinkedListAppend(TokenList, StmtEnderToken);
        }

        if (InputText[CurrPos] == '(')
        {
            CurrPos++;
            lexical_token_t* LeftParen = NewToken(LEFT_PAREN, "(", CurrPos-1, CurrPos-1, Line);
            TokenList = LinkedListAppend(TokenList, LeftParen);
        }

        if (InputText[CurrPos] == ')')
        {
            CurrPos++;
            lexical_token_t* RightParen = NewToken(RIGHT_PAREN, ")", CurrPos-1, CurrPos-1, Line);
            TokenList = LinkedListAppend(TokenList, RightParen);
        }

        if (InputText[CurrPos] == '#')
        {
            CurrPos++;
            lexical_token_t* Hash = NewToken(PRINT, "#", CurrPos-1, CurrPos-1, Line);
            TokenList = LinkedListAppend(TokenList, Hash);
        }

        if (IsEMDASOperator(InputText[CurrPos]))
        {
            char* Content = (char*)calloc(2, 1);
            strncpy(Content, InputText + CurrPos, 1);
            CurrPos++;
            lexical_token_t* OpToken = NewToken(OPERATOR, Content, CurrPos-1, CurrPos-1, Line);
            TokenList = LinkedListAppend(TokenList, OpToken);
        }

        if (InputText[CurrPos] == '=')
        {
            SavePos = CurrPos;
            CurrPos++;
            if (InputText[CurrPos] == '=')
            {
                CurrPos++;
            }
            char* Content = (char*)calloc(CurrPos - SavePos + 1, 1);
            strncpy(Content, InputText + SavePos, CurrPos - SavePos);
            lexical_token_t* OpToken = NewToken(OPERATOR, Content, SavePos, CurrPos, Line);
            TokenList = LinkedListAppend(TokenList, OpToken);
        }

        if (IsGreaterOrSmallerOperator(InputText[CurrPos]))
        {
            SavePos = CurrPos;
            CurrPos++;
            if (InputText[CurrPos] == '=')
            {
                CurrPos++;
            }
            char* Content = (char*)calloc(CurrPos - SavePos + 1, 1);
            strncpy(Content, InputText + SavePos, CurrPos - SavePos);
            lexical_token_t* OpToken = NewToken(OPERATOR, Content, SavePos, CurrPos, Line);
            TokenList = LinkedListAppend(TokenList, OpToken);
        }

        if (InputText[CurrPos] == '!')
        {
            SavePos = CurrPos;
            CurrPos++;
            if (InputText[CurrPos] == '=')
            {
                CurrPos++;
                char* Content = (char*)calloc(CurrPos - SavePos + 1, 1);
                strncpy(Content, InputText + SavePos, CurrPos - SavePos);
                lexical_token_t* OpToken = NewToken(OPERATOR, Content, SavePos, CurrPos, Line);
                TokenList = LinkedListAppend(TokenList, OpToken);
            }
            else
            {
                fprintf(stderr, "[LEXER @ Line %lld Column %lld] error: expected '=' after '!' to form '!=' (NOT EQUAL) operator, got %c instead\n", Line, CurrPos, InputText[CurrPos]);
                exit(-1);
            }
        }

        if (InputText[CurrPos] == ',')
        {
            CurrPos++;
            lexical_token_t* CommaToken = NewToken(COMMA, ",", CurrPos-1, CurrPos-1, Line);
            TokenList = LinkedListAppend(TokenList, CommaToken);
        }

        if (InputText[CurrPos] == '{')
        {
            CurrPos++;
            lexical_token_t* LeftBracket = NewToken(LEFT_BRACKET, "{", CurrPos-1, CurrPos-1, Line);
            TokenList = LinkedListAppend(TokenList, LeftBracket);
        }

        if (InputText[CurrPos] == '}')
        {
            CurrPos++;
            lexical_token_t* RightBracket = NewToken(RIGHT_BRACKET, "}", CurrPos-1, CurrPos-1, Line);
            TokenList = LinkedListAppend(TokenList, RightBracket);
        }
    }
    return TokenList;
}