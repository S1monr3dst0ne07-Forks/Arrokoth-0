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
        token_wrapper_t* NewList = (token_wrapper_t*)calloc(TV->MaxSize, sizeof(token_wrapper_t));
        memset(NewList, 0, TV->MaxSize);
        memcpy(NewList, TV->List, TV->CurrSize * sizeof(token_wrapper_t));
        free(TV->List);
        TV->List = NewList;
    }
}

static void BinOpSetOperator(bin_op_token_t* BinOp, char* Content)
{
    BinOp->Operator = (!strcmp(Content, "+")) ? PLUS : BinOp->Operator;
    BinOp->Operator = (!strcmp(Content, "-")) ? MINUS : BinOp->Operator;
    BinOp->Operator = (!strcmp(Content, "*")) ? MULT : BinOp->Operator;
    BinOp->Operator = (!strcmp(Content, "/")) ? DIV : BinOp->Operator;
    BinOp->Operator = (!strcmp(Content, "^")) ? EXP : BinOp->Operator;
}

static enum CompBinOpType GetCondOpType(char* Content)
{
    enum CompBinOpType Res = EQUAL;
    Res = (!strcmp(Content, "==")) ? EQUAL : Res;
    Res = (!strcmp(Content, "!=")) ? NOT_EQUAL : Res;
    Res = (!strcmp(Content, ">")) ? GREATER : Res;
    Res = (!strcmp(Content, "<")) ? LESSER : Res;
    Res = (!strcmp(Content, ">=")) ? GREATER_OR_EQ : Res;
    Res = (!strcmp(Content, "<=")) ? LESSER_OR_EQ : Res;
    return Res;
}

// Global var, fight me
linked_list_node_t* TokenList = NULL;

static char* LexTypeToStr(token_type_t T)
{
    switch (T)
    {
        case TT_IDENTIFIER:
            return "IDENTIFIER";
        case TT_STATEMENT_DELIMITER:
            return "STATEMENT_DELIMITER";
        case TT_LEFT_PAREN:
            return "LEFT_PAREN";
        case TT_RIGHT_PAREN:
            return "RIGHT_PAREN";
        case TT_OPERATOR:
            return "OPERATOR";
        case TT_NUMBER:
            return "NUMBER";
        case TT_COMMA:
            return "COMMA";
        case TT_LEFT_BRACKET:
            return "LEFT_BRACKET";
        case TT_RIGHT_BRACKET:
            return "RIGHT_BRACKET";
        case TT_PRINT:
            return "PRINT";
    }
    return "?";
}

static void ConsumeToken()
{
    linked_list_node_t* ToDelete = TokenList;
    TokenList = TokenList->Next;
    LinkedListDestroySingular(ToDelete);
}

static token_t PeekToken()
{
    return ((token_t)TokenList->Data);
}

static char AcceptToken(token_type_t Type)
{
    if (PeekToken()->type == Type)
    {
        ConsumeToken();
        return 1;
    }
    return 0;
}

static char AcceptExactToken(token_type_t Type, const char* Content)
{
    if (PeekToken()->type == Type && (!strcmp(PeekToken()->content, Content)))
    {
        ConsumeToken();
        return 1;
    }
    return 0;
}

static void ExpectError(token_type_t T1, token_type_t T2, size_t Line)
{
    fprintf(stderr, "[PARSER @ Line %lld] error: expected '%s', got '%s' instead\n", Line, LexTypeToStr(T1), LexTypeToStr(T2));
    exit(1);
}

static char ExpectToken(token_type_t Type)
{
    token_t Tok = PeekToken();
    if (Tok->type == Type)
    {
        ConsumeToken();
        return 1;
    }
    ExpectError(Type, Tok->type, Tok->line);
    return 0;
}

static char ExpectTokenNoConsume(token_type_t Type)
{
    token_t Tok = PeekToken();
    if (Tok->type == Type)
    {
        return 1;
    }
    ExpectError(Type, Tok->type, Tok->line);
    return 0;
}

static char ExpectExactToken(token_type_t Type, const char* Content)
{
    token_t Tok = PeekToken();
    if (Tok->type == Type && (!strcmp(Tok->content, Content)))
    {
        ConsumeToken();
        return 1;
    }
    ExpectError(Type, Tok->type, Tok->line);
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
static while_loop_token_t* WhileLoop();
static if_branch_token_t* IfBranch();
static print_token_t* PrintVar();
static statement_token_t* Statement();
static proc_creation_token_t* ProcCreation();
static var_creation_token_t* VarCreation();
static block_token_t* Block();

static number_token_t* Number()
{
    number_token_t* Output = (number_token_t*)malloc(sizeof(number_token_t));

    ExpectTokenNoConsume(TT_NUMBER);

    Output->Val = strtod(PeekToken()->content, NULL);
    ConsumeToken();

    return Output;
}

static identifier_token_t* Identifier()
{
    identifier_token_t* Output = (identifier_token_t*)malloc(sizeof(identifier_token_t));

    ExpectTokenNoConsume(TT_IDENTIFIER);

    Output->Text = (char*)calloc(strlen(PeekToken()->content) + 1, 1);
    strcpy(Output->Text, PeekToken()->content);
    ConsumeToken();

    return Output;
}

static atom_token_t* Atom()
{
    atom_token_t* Output = (atom_token_t*)malloc(sizeof(atom_token_t));

    if (PeekToken()->type == TT_IDENTIFIER)
    {
        Output->Child.Type = IDENTIFIER_TOKEN;
        Output->Child.Data = Identifier();
    }
    else if (PeekToken()->type == TT_NUMBER)
    {
        Output->Child.Type = NUMBER_TOKEN;
        Output->Child.Data = Number();
    }
    else if (ExpectToken(TT_LEFT_PAREN))
    {
        Output->Child.Type = EXPRESSION;
        Output->Child.Data = Expression();
        ExpectToken(TT_RIGHT_PAREN);
    }

    return Output;
}

static factor_token_t* Factor()
{
    factor_token_t* Output = (factor_token_t*)malloc(sizeof(factor_token_t));
    token_wrapper_t ParentTree;

    ParentTree.Type = ATOM;
    ParentTree.Data = Atom();

    while (AcceptExactToken(TT_OPERATOR, "^"))
    {
        if (ParentTree.Type == ATOM)
        {
            bin_op_token_t* BinOp = (bin_op_token_t*)malloc(sizeof(bin_op_token_t));
            BinOp->L = ParentTree;
            BinOp->R.Type = ATOM;
            BinOp->R.Data = Atom();
            BinOp->Operator = EXP;
            ParentTree.Type = BIN_OP;
            ParentTree.Data = BinOp;
        }
        else
        {
            bin_op_token_t* BinOp = (bin_op_token_t*)malloc(sizeof(bin_op_token_t));
            BinOp->R = ParentTree;
            BinOp->L.Type = ATOM;
            BinOp->L.Data = Atom();
            BinOp->Operator = EXP;
            ParentTree.Type = BIN_OP;
            ParentTree.Data = BinOp;
        }
    }

    Output->Child = ParentTree;
    return Output;
}

static term_token_t* Term()
{
    term_token_t* Output = (term_token_t*)malloc(sizeof(term_token_t));
    token_wrapper_t ParentTree;

    ParentTree.Type = FACTOR;
    ParentTree.Data = Factor();

    char* Op;
    Op = PeekToken()->content;
    while (AcceptExactToken(TT_OPERATOR, "*") || AcceptExactToken(TT_OPERATOR, "/"))
    {
        bin_op_token_t* BinOp = (bin_op_token_t*)malloc(sizeof(bin_op_token_t));
        BinOp->L = ParentTree;
        BinOp->R.Type = FACTOR;
        BinOp->R.Data = Factor();
        BinOpSetOperator(BinOp, Op);
        ParentTree.Type = BIN_OP;
        ParentTree.Data = BinOp;
        Op = PeekToken()->content;
    }

    Output->Child = ParentTree;
    return Output;
}

static expression_token_t* Expression()
{
    expression_token_t* Output = (expression_token_t*)malloc(sizeof(expression_token_t));
    token_wrapper_t ParentTree;

    ParentTree.Type = TERM;
    ParentTree.Data = Term();

    char* Op;
    Op = PeekToken()->content;
    while (AcceptExactToken(TT_OPERATOR, "+") || AcceptExactToken(TT_OPERATOR, "-"))
    {
        bin_op_token_t* BinOp = (bin_op_token_t*)malloc(sizeof(bin_op_token_t));
        BinOp->L = ParentTree;
        BinOp->R.Type = TERM;
        BinOp->R.Data = Term();
        BinOpSetOperator(BinOp, Op);
        ParentTree.Type = BIN_OP;
        ParentTree.Data = BinOp;
        Op = PeekToken()->content;
    }

    Output->Child = ParentTree;
    return Output;
}

static function_token_t* FunctionCall()
{
    function_token_t* Output = (function_token_t*)malloc(sizeof(function_token_t));

    ExpectTokenNoConsume(TT_IDENTIFIER);
    Output->FuncName.Data = Identifier();
    Output->FuncName.Type = IDENTIFIER_TOKEN;

    return Output;
}

static assignment_token_t* Assignment()
{
    assignment_token_t* Output = (assignment_token_t*)malloc(sizeof(assignment_token_t));

    ExpectTokenNoConsume(TT_IDENTIFIER);
    Output->Name.Type = IDENTIFIER_TOKEN;
    Output->Name.Data = Identifier();
    ExpectExactToken(TT_OPERATOR, "=");
    Output->Val.Data = Expression();
    Output->Val.Type = EXPRESSION;

    return Output;
}

static while_loop_token_t* WhileLoop()
{
    while_loop_token_t* Output = (while_loop_token_t*)malloc(sizeof(while_loop_token_t));
    TokenVectorInit(&Output->Statements);

    Output->L.Type = EXPRESSION;
    Output->L.Data = Expression();

    Output->OpType = GetCondOpType(PeekToken()->content);
    ExpectToken(TT_OPERATOR);

    Output->R.Type = EXPRESSION;
    Output->R.Data = Expression();

    ExpectToken(TT_LEFT_BRACKET);
    do
    {
        token_wrapper_t TW;
        TW.Type = STATEMENT_TOKEN;
        TW.Data = Statement();
        TokenVectorAppend(&Output->Statements, TW);
        ExpectToken(TT_STATEMENT_DELIMITER);
    }
    while (!AcceptToken(TT_RIGHT_BRACKET));

    return Output;
}

static if_branch_token_t* IfBranch()
{
    if_branch_token_t* Output = (if_branch_token_t*)malloc(sizeof(if_branch_token_t));
    TokenVectorInit(&Output->Statements);

    Output->L.Type = EXPRESSION;
    Output->L.Data = Expression();

    Output->OpType = GetCondOpType(PeekToken()->content);
    ExpectToken(TT_OPERATOR);

    Output->R.Type = EXPRESSION;
    Output->R.Data = Expression();

    ExpectToken(TT_LEFT_BRACKET);
    do
    {
        token_wrapper_t TW;
        TW.Type = STATEMENT_TOKEN;
        TW.Data = Statement();
        TokenVectorAppend(&Output->Statements, TW);
        ExpectToken(TT_STATEMENT_DELIMITER);
    }
    while (!AcceptToken(TT_RIGHT_BRACKET));

    return Output;
}

static print_token_t* PrintVar()
{
    print_token_t* Output = (print_token_t*)malloc(sizeof(print_token_t));
    TokenVectorInit(&Output->Ids);

    do
    {
        token_wrapper_t TW;
        ExpectTokenNoConsume(TT_IDENTIFIER);
        TW.Type = IDENTIFIER_TOKEN;
        TW.Data = Identifier();
        TokenVectorAppend(&Output->Ids, TW);
    }
    while (AcceptToken(TT_COMMA));

    return Output;
}

static statement_token_t* Statement()
{
    statement_token_t* Output = (statement_token_t*)malloc(sizeof(statement_token_t));

    if (AcceptExactToken(TT_IDENTIFIER, "run"))
    {
        Output->Child.Data = FunctionCall();
        Output->Child.Type = FUNCTION_TOKEN;
    }
    else if (AcceptExactToken(TT_IDENTIFIER, "while"))
    {
        Output->Child.Data = WhileLoop();
        Output->Child.Type = WHILE_LOOP;
    }
    else if (AcceptExactToken(TT_IDENTIFIER, "if"))
    {
        Output->Child.Data = IfBranch();
        Output->Child.Type = IF_BRANCH;
    }
    else if (AcceptToken(TT_PRINT))
    {
        Output->Child.Data = PrintVar();
        Output->Child.Type = PRINT_VAR;
    }
    else if (PeekToken()->type == TT_IDENTIFIER)
    {
        Output->Child.Data = Assignment();
        Output->Child.Type = ASSIGNMENT_TOKEN;
    }

    return Output;
}

static proc_creation_token_t* ProcCreation()
{
    proc_creation_token_t* Output = (proc_creation_token_t*)malloc(sizeof(proc_creation_token_t));
    TokenVectorInit(&Output->Statements);

    ExpectTokenNoConsume(TT_IDENTIFIER);
    Output->Name.Type = IDENTIFIER_TOKEN;
    Output->Name.Data = Identifier();

    ExpectToken(TT_LEFT_BRACKET);
    do
    {
        token_wrapper_t TW;
        TW.Type = STATEMENT_TOKEN;
        TW.Data = Statement();
        TokenVectorAppend(&Output->Statements, TW);
        ExpectToken(TT_STATEMENT_DELIMITER);
    }
    while (!AcceptToken(TT_RIGHT_BRACKET));

    return Output;
}

static var_creation_token_t* VarCreation()
{
    var_creation_token_t* Output = (var_creation_token_t*)malloc(sizeof(var_creation_token_t));
    TokenVectorInit(&Output->Children);

    do
    {
        token_wrapper_t TW;
        ExpectTokenNoConsume(TT_IDENTIFIER);
        TW.Type = IDENTIFIER_TOKEN;
        TW.Data = Identifier();
        TokenVectorAppend(&Output->Children, TW);
    }
    while (AcceptToken(TT_COMMA));
    ExpectToken(TT_STATEMENT_DELIMITER);

    return Output;
}

static block_token_t* Block()
{
    block_token_t* Output = (block_token_t*)malloc(sizeof(block_token_t));

    if (AcceptExactToken(TT_IDENTIFIER, "var"))
    {
        Output->Child.Type = VAR_CREATION;
        Output->Child.Data = VarCreation();
    }
    else if (ExpectExactToken(TT_IDENTIFIER, "proc"))
    {
        Output->Child.Type = PROC_CREATION;
        Output->Child.Data = ProcCreation();
    }

    return Output;
}

program_token_t* DoParseAST(linked_list_node_t* LexTokens)
{
    TokenList = LexTokens;
    program_token_t* Prog = (program_token_t*)malloc(sizeof(program_token_t));
    TokenVectorInit(&Prog->Blocks);

    if (ExpectExactToken(TT_IDENTIFIER, "program"))
    {
        do
        {
            token_wrapper_t TW;
            TW.Type = BLOCK_TOKEN;
            TW.Data = Block();
            TokenVectorAppend(&Prog->Blocks, TW);
        }
        while (!AcceptExactToken(TT_IDENTIFIER, "end"));
    }

    TokenList = NULL;
    return Prog;
}
