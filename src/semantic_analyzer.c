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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <arrokoth-0/semantic_analyzer.h>

void StringVectorInit(string_vector_t* V)
{
    V->CurrSize = 0;
    V->MaxSize = 2;
    V->List = (char**)calloc(V->MaxSize, sizeof(char*));
}

void StringVectorAppend(string_vector_t* V, char* Str)
{
    V->List[V->CurrSize] = Str;
    V->CurrSize++;
    if (V->CurrSize == V->MaxSize)
    {
        V->MaxSize *= 2;
        char** NewList = (char**)calloc(V->MaxSize, sizeof(char*));
        memset(NewList, 0, V->MaxSize);
        memcpy(NewList, V->List, V->CurrSize * sizeof(char*));
        free(V->List);
        V->List = NewList;
    }
}

uint8_t StringVectorContains(string_vector_t* V, char* Str)
{
    for (size_t N = 0; N < V->CurrSize; N++)
    {
        if (!strcmp(V->List[N], Str))
        {
            return 1;
        }
    }
    return 0;
}

static string_vector_t VariableList;
static string_vector_t ProcedureList;

static void AnalyzeWhileLoop(while_loop_token_t* While);
static void AnalyzeIfBranch(if_branch_token_t* IfBranch);
static void AnalyzeBinOp(bin_op_token_t* BinOp);
static void AnalyzeUnaryMinus(unary_minus_token_t* UnaryMinus);
static void AnalyzePrintVar(print_token_t* Print);
static void AnalyzeFunctionCall(function_token_t* Func);
static void AnalyzeAssignment(assignment_token_t* Assign);
static void AnalyzeProcCreation(proc_creation_token_t* Proc);
static void AnalyzeVarCreation(var_creation_token_t* VarC);
static void AnalyzeProgram(program_token_t* AST);

static void DispatchCall(token_wrapper_t TW)
{
    switch (TW.Type)
    {
        case VAR_CREATION:
            AnalyzeVarCreation((var_creation_token_t*)TW.Data);
            break;
        case PROC_CREATION:
            AnalyzeProcCreation((proc_creation_token_t*)TW.Data);
            break;
        case ASSIGNMENT_TOKEN:
            AnalyzeAssignment((assignment_token_t*)TW.Data);
            break;
        case FUNCTION_TOKEN:
            AnalyzeFunctionCall((function_token_t*)TW.Data);
            break;
        case PRINT_VAR:
            AnalyzePrintVar((print_token_t*)TW.Data);
            break;
        case UNARY_MINUS:
            AnalyzeUnaryMinus((unary_minus_token_t*)TW.Data);
            break;
        case BIN_OP:
            AnalyzeBinOp((bin_op_token_t*)TW.Data);
            break;
        case IF_BRANCH:
            AnalyzeIfBranch((if_branch_token_t*)TW.Data);
            break;
        case WHILE_LOOP:
            AnalyzeWhileLoop((while_loop_token_t*)TW.Data);
            break;
        case PROGRAM_TOKEN:
        case EXPRESSION:
        case STATEMENT_TOKEN:
        case BLOCK_TOKEN:
        case TERM:
        case FACTOR:
        case ATOM:
            fputs("[SEMANTIC ANALYZER] error: encountered illegal token, AST is corrupt", stderr);
            exit(-1);
        case IDENTIFIER_TOKEN:
        case NUMBER_TOKEN:
            break;
    }
}

static void AnalyzeWhileLoop(while_loop_token_t* While)
{
    if (While->L.Type == IDENTIFIER_TOKEN)
    {
        identifier_token_t* ID = (identifier_token_t*)While->L.Data;
        if (!StringVectorContains(&VariableList, ID->Text))
        {
            fprintf(stderr, "[SEMANTIC ANALYZER] error: variable '%s' doesn't exist", ID->Text);
            exit(-1);
        }
    }
    else
    {
        DispatchCall(While->L);
    }

    if (While->R.Type == IDENTIFIER_TOKEN)
    {
        identifier_token_t* ID = (identifier_token_t*)While->R.Data;
        if (!StringVectorContains(&VariableList, ID->Text))
        {
            fprintf(stderr, "[SEMANTIC ANALYZER] error: variable '%s' doesn't exist", ID->Text);
            exit(-1);
        }
    }
    else
    {
        DispatchCall(While->R);
    }

    for (size_t N = 0; N < While->Statements.CurrSize; N++)
    {
        DispatchCall(While->Statements.List[N]);
    }
}

static void AnalyzeIfBranch(if_branch_token_t* IfBranch)
{
    if (IfBranch->L.Type == IDENTIFIER_TOKEN)
    {
        identifier_token_t* ID = (identifier_token_t*)IfBranch->L.Data;
        if (!StringVectorContains(&VariableList, ID->Text))
        {
            fprintf(stderr, "[SEMANTIC ANALYZER] error: variable '%s' doesn't exist", ID->Text);
            exit(-1);
        }
    }
    else
    {
        DispatchCall(IfBranch->L);
    }

    if (IfBranch->R.Type == IDENTIFIER_TOKEN)
    {
        identifier_token_t* ID = (identifier_token_t*)IfBranch->R.Data;
        if (!StringVectorContains(&VariableList, ID->Text))
        {
            fprintf(stderr, "[SEMANTIC ANALYZER] error: variable '%s' doesn't exist", ID->Text);
            exit(-1);
        }
    }
    else
    {
        DispatchCall(IfBranch->R);
    }

    for (size_t N = 0; N < IfBranch->Statements.CurrSize; N++)
    {
        DispatchCall(IfBranch->Statements.List[N]);
    }
}

static void AnalyzeBinOp(bin_op_token_t* BinOp)
{
    if (BinOp->L.Type == IDENTIFIER_TOKEN)
    {
        identifier_token_t* ID = (identifier_token_t*)BinOp->L.Data;
        if (!StringVectorContains(&VariableList, ID->Text))
        {
            fprintf(stderr, "[SEMANTIC ANALYZER] error: variable '%s' doesn't exist", ID->Text);
            exit(-1);
        }
    }
    else
    {
        DispatchCall(BinOp->L);
    }

    if (BinOp->R.Type == IDENTIFIER_TOKEN)
    {
        identifier_token_t* ID = (identifier_token_t*)BinOp->R.Data;
        if (!StringVectorContains(&VariableList, ID->Text))
        {
            fprintf(stderr, "[SEMANTIC ANALYZER] error: variable '%s' doesn't exist", ID->Text);
            exit(-1);
        }
    }
    else
    {
        DispatchCall(BinOp->R);
    }

    if (BinOp->Operator == DIV && BinOp->R.Type == NUMBER_TOKEN)
    {
        number_token_t* R = (number_token_t*)BinOp->R.Data;
        uint64_t ReinterpretR = *((uint64_t*)&R->Val);
        if (ReinterpretR == 0)
        {
            fputs("[SEMANTIC ANALYZER] error: cannot divide by 0", stderr);
            exit(-1);
        }
    }
}

static void AnalyzeUnaryMinus(unary_minus_token_t* UnaryMinus)
{
    if (UnaryMinus->Child.Type == IDENTIFIER_TOKEN)
    {
        identifier_token_t* ID = (identifier_token_t*)UnaryMinus->Child.Data;
        if (!StringVectorContains(&VariableList, ID->Text))
        {
            fprintf(stderr, "[SEMANTIC ANALYZER] error: variable '%s' doesn't exist", ID->Text);
            exit(-1);
        }
    }
    else if (UnaryMinus->Child.Type != NUMBER_TOKEN)
    {
        DispatchCall(UnaryMinus->Child);
    }
}

static void AnalyzePrintVar(print_token_t* Print)
{
    for (size_t N = 0; N < Print->Ids.CurrSize; N++)
    {
        identifier_token_t* ID = (identifier_token_t*)Print->Ids.List[N].Data;
        if (!StringVectorContains(&VariableList, ID->Text))
        {
            fprintf(stderr, "[SEMANTIC ANALYZER] error: variable '%s' doesn't exist", ID->Text);
            exit(-1);
        }
    }
}

static void AnalyzeFunctionCall(function_token_t* Func)
{
    identifier_token_t* ID = (identifier_token_t*)Func->FuncName.Data;

    if (!StringVectorContains(&ProcedureList, ID->Text))
    {
        fprintf(stderr, "[SEMANTIC ANALYZER] error: procedure '%s' doesn't exist", ID->Text);
        exit(-1);
    }
}

static void AnalyzeAssignment(assignment_token_t* Assign)
{
    identifier_token_t* ID = (identifier_token_t*)Assign->Name.Data;
    if (!StringVectorContains(&VariableList, ID->Text))
    {
        fprintf(stderr, "[SEMANTIC ANALYZER] error: variable '%s' doesn't exist", ID->Text);
        exit(-1);
    }

    if (Assign->Val.Type == IDENTIFIER_TOKEN)
    {
        identifier_token_t* ID2 = (identifier_token_t*)Assign->Val.Data;
        if (!StringVectorContains(&VariableList, ID2->Text))
        {
            fprintf(stderr, "[SEMANTIC ANALYZER] error: variable '%s' doesn't exist", ID2->Text);
            exit(-1);
        }
    }
    else
    {
        DispatchCall(Assign->Val);
    }
}

static void AnalyzeProcCreation(proc_creation_token_t* Proc)
{
    identifier_token_t* ID = (identifier_token_t*)Proc->Name.Data;
    StringVectorAppend(&ProcedureList, ID->Text);

    for (size_t N = 0; N < Proc->Statements.CurrSize; N++)
    {
        DispatchCall(Proc->Statements.List[N]);
    }
}

static void AnalyzeVarCreation(var_creation_token_t* VarC)
{
    for (size_t N = 0; N < VarC->Children.CurrSize; N++)
    {
        identifier_token_t* ID = (identifier_token_t*)VarC->Children.List[N].Data;
        StringVectorAppend(&VariableList, ID->Text);
    }
}

static void AnalyzeProgram(program_token_t* AST)
{
    for (size_t N = 0; N < AST->Blocks.CurrSize; N++)
    {
        DispatchCall(AST->Blocks.List[N]);
    }
}

void DoSemanticAnalyzation(program_token_t* AST)
{
    StringVectorInit(&VariableList);
    StringVectorInit(&ProcedureList);
    AnalyzeProgram(AST);
}