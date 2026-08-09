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
#include <stdio.h>

#include <arrokoth-0/lexer.h>
#include <arrokoth-0/parser.h>

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

static number_token_t* Number();
static identifier_token_t* Identifier();
static atom_token_t* Atom();
static factor_token_t* Factor();
static term_token_t* Term();
static expression_token_t* Expression();
static function_token_t* FunctionCall();
static assignment_token_t* Assignment();
static statement_token_t* Statement();

static number_token_t* Number()
{
    number_token_t* Output = (number_token_t*)malloc(sizeof(number_token_t));

    if (PeekToken()->Type == NUMBER)
    {
        Output->Val = strtod(PeekToken()->Content, NULL);
        ConsumeToken();
    }

    return Output;
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

static string_token_t* String()
{
    string_token_t* Output = (string_token_t*)malloc(sizeof(string_token_t));

    if (PeekToken()->Type == IDENTIFIER)
    {
        Output->Text = (char*)calloc(strlen(PeekToken()->Content) + 1, 1);
        strcpy(Output->Text, PeekToken()->Content);
        ConsumeToken();
    }

    return Output;
}

static atom_token_t* Atom()
{
    atom_token_t* Output = (atom_token_t*)malloc(sizeof(atom_token_t));

    if (PeekToken()->Type == IDENTIFIER)
    {
        Output->Child.Type = IDENTIFIER_TOKEN;
        Output->Child.Data = Identifier();
    }
    else if (PeekToken()->Type == NUMBER)
    {
        Output->Child.Type = NUMBER_TOKEN;
        Output->Child.Data = Number();
    }
    else if (PeekToken()->Type == STRING)
    {
        Output->Child.Type = STRING_TOKEN;
        Output->Child.Data = String();
    }
    else if (ExpectToken(LEFT_PAREN))
    {
        Output->Child.Type = EXPRESSION;
        Output->Child.Data = Expression();
        ExpectToken(RIGHT_PAREN);
    }

    return Output;
}

static factor_token_t* Factor()
{
    factor_token_t* Output = (factor_token_t*)malloc(sizeof(factor_token_t));
    token_wrapper_t ParentTree;

    ParentTree.Type = ATOM;
    ParentTree.Data = Atom();

    while (AcceptExactToken(OPERATOR, "^"))
    {
        bin_op_token_t* BinOp = (bin_op_token_t*)malloc(sizeof(bin_op_token_t));
        BinOp->R = ParentTree;
        BinOp->L.Type = ATOM;
        BinOp->L.Data = Atom();
        ParentTree.Type = BIN_OP;
        ParentTree.Data = BinOp;
    }

    return Output;
}

static term_token_t* Term()
{
    term_token_t* Output = (term_token_t*)malloc(sizeof(term_token_t));
    token_wrapper_t ParentTree;

    ParentTree.Type = FACTOR;
    ParentTree.Data = Factor();

    while (AcceptExactToken(OPERATOR, "*") || AcceptExactToken(OPERATOR, "/"))
    {
        bin_op_token_t* BinOp = (bin_op_token_t*)malloc(sizeof(bin_op_token_t));
        BinOp->L = ParentTree;
        BinOp->R.Type = FACTOR;
        BinOp->R.Data = Factor();
        ParentTree.Type = BIN_OP;
        ParentTree.Data = BinOp;
    }

    return Output;
}

static expression_token_t* Expression()
{
    expression_token_t* Output = (expression_token_t*)malloc(sizeof(expression_token_t));
    token_wrapper_t ParentTree;

    if (AcceptExactToken(OPERATOR, "-"))
    {
        unary_minus_token_t* UnOp = (unary_minus_token_t*)malloc(sizeof(unary_minus_token_t));
        UnOp->Child.Type = TERM;
        UnOp->Child.Data = Term();
        ParentTree.Type = UNARY_MINUS;
        ParentTree.Data = UnOp;
    }
    else
    {
        ParentTree.Type = TERM;
        ParentTree.Data = Term();
    }

    while (AcceptExactToken(OPERATOR, "+") || AcceptExactToken(OPERATOR, "-"))
    {
        bin_op_token_t* BinOp = (bin_op_token_t*)malloc(sizeof(bin_op_token_t));
        BinOp->L = ParentTree;
        BinOp->R.Type = TERM;
        BinOp->R.Data = Term();
        ParentTree.Type = BIN_OP;
        ParentTree.Data = BinOp;
    }

    return Output;
}

static function_token_t* FunctionCall()
{
    function_token_t* Output = (function_token_t*)malloc(sizeof(function_token_t));
    Output->FuncName.Data = Identifier();
    Output->FuncName.Type = IDENTIFIER_TOKEN;
    return Output;
}

static assignment_token_t* Assignment()
{
    assignment_token_t* Output = (assignment_token_t*)malloc(sizeof(assignment_token_t));

    ExpectExactToken(IDENTIFIER, "var");
    Output->Name.Data = Identifier();
    Output->Name.Type = IDENTIFIER_TOKEN;
    ExpectExactToken(OPERATOR, "=");
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
        while (!AcceptExactToken(IDENTIFIER, "end"));
    }

    TokenList = NULL;
    return Prog;
}