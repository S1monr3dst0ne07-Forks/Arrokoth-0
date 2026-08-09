/*
Orcus-0 - A simple, basic programming language.
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

#include <orcus-0/lexer.h>
#include <orcus-0/parser.h>

void TokenVectorInit(token_vector_t* TV)
{
    TV->CurrSize = 0;
    TV->MaxSize = 2;
    TV->List = (token_wrapper_t*)calloc(TV->MaxSize, sizeof(token_wrapper_t));
}

void TokenVectorAppend(token_vector_t* TV, token_wrapper_t TokenWrapper)
{
    TV->List[TV->CurrSize] = TokenWrapper;
    TV->CurrSize++;
    if (TV->CurrSize == TV->MaxSize)
    {
        TV->MaxSize *= 2;
        TV->List = (token_wrapper_t*)realloc(TV->List, TV->MaxSize);
    }
}

// Global var, fight me
linked_list_node_t* TokenList = NULL;

static void ConsumeToken()
{
    linked_list_node_t* ToDelete = TokenList;
    TokenList = TokenList->Next;
    LinkedListDestroySingular(ToDelete);
}

static lexical_token_t* PeekToken()
{
    return ((lexical_token_t*)TokenList->Data);
}

static char AcceptExactToken(enum LexicalTokenType Type, const char* Content)
{
    if (PeekToken()->Type == Type && (!strcmp(PeekToken()->Content, Content)))
    {
        ConsumeToken();
        return 1;
    }
    return 0;
}

static char AcceptToken(enum LexicalTokenType Type)
{
    if (PeekToken()->Type == Type)
    {
        ConsumeToken();
        return 1;
    }
    return 0;
}

static char ExpectToken(enum LexicalTokenType Type)
{
    if (PeekToken()->Type == Type)
    {
        ConsumeToken();
        return 1;
    }
    exit(-1); // TODO: Make it error out properly
    return 0;
}

static char ExpectExactToken(enum LexicalTokenType Type, const char* Content)
{
    if (PeekToken()->Type == Type && (!strcmp(PeekToken()->Content, Content)))
    {
        ConsumeToken();
        return 1;
    }
    exit(-1); // TODO: Make it error out properly
    return 0;
}

static identifier_token_t* Identifier()
{
    identifier_token_t* Output = (identifier_token_t*)malloc(sizeof(identifier_token_t));

    if (PeekToken()->Type == IDENTIFIER)
    {
        Output->Text = (char*)calloc(strlen(PeekToken()->Content) + 1, 1);
        strcpy(Output->Text, PeekToken()->Content);
        ConsumeToken();
    }

    return Output;
}

static expression_token_t* Expression()
{
    expression_token_t* Output = (expression_token_t*)malloc(sizeof(expression_token_t));

    // TODO

    return Output;
}

static function_token_t* FunctionCall()
{
    function_token_t* Output = (function_token_t*)malloc(sizeof(function_token_t));
    Output->FuncName.Data = Identifier();
    Output->FuncName.Type = IDENTIFIER;
    return Output;
}

static assignment_token_t* Assignment()
{
    assignment_token_t* Output = (assignment_token_t*)malloc(sizeof(assignment_token_t));

    Output->Name.Data = Identifier();
    Output->Name.Type = IDENTIFIER;
    ExpectExactToken(BIN_OP, "=");
    Output->Val.Data = Expression();
    Output->Val.Type = EXPRESSION;

    return Output;
}

static statement_token_t* Statement()
{
    statement_token_t* Output = (statement_token_t*)malloc(sizeof(statement_token_t));

    if (PeekToken()->Type == IDENTIFIER)
    {
        if ( !strcmp(PeekToken()->Content, "run") )
        {
            ConsumeToken();
            Output->Child.Data = FunctionCall();
            Output->Child.Type = FUNCTION_TOKEN;
        }
        else
        {
            Output->Child.Data = Assignment();
            Output->Child.Type = ASSIGNMENT_TOKEN;
        }
    }

    return Output;
}

program_token_t* DoParseAST(linked_list_node_t* LexTokens)
{
    TokenList = LexTokens;
    program_token_t* Prog = (program_token_t*)malloc(sizeof(program_token_t));
    TokenVectorInit(&Prog->Statements);

    if (AcceptExactToken(IDENTIFIER, "program"))
    {
        do
        {
            token_wrapper_t TW;
            TW.Type = STATEMENT_TOKEN;
            TW.Data = Statement();
            TokenVectorAppend(&Prog->Statements, TW);
            ExpectToken(STATEMENT_DELIMITER);
        }
        while (AcceptExactToken(IDENTIFIER, "end"));
    }

    TokenList = NULL;
    return Prog;
}