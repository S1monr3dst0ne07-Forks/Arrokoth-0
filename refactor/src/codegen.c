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
#include <stdlib.h>
#include <string.h>

#include <arrokoth-0/parser.h>
#include <arrokoth-0/codegen.h>
#include <arrokoth-0/frontend.h>

static FILE* OutputFile = NULL;
static size_t StatementNum = 0;
static size_t ProcTempVar = 0;

static void GenWhileLoop(while_loop_token_t* WhileLoop);
static void GenIfBranch(if_branch_token_t* IfBranch);
static size_t GenBinOp(bin_op_token_t* BinOp);
static size_t GenIdentifier(identifier_token_t* Id);
static void GenAssignment(assignment_token_t* Assign);
static void GenFunctionCall(function_token_t* Func);
static void GenPrintVar(print_token_t* Print);
static void GenProcCreation(proc_creation_token_t* Proc);
static void GenVarCreation(var_creation_token_t* VarC);
static void GenProgram(program_token_t* Prog);

static size_t DispatchCall(token_wrapper_t TW)
{
    switch (TW.Type)
    {
        case VAR_CREATION:
            GenVarCreation((var_creation_token_t*)TW.Data);
            break;
        case PROC_CREATION:
            GenProcCreation((proc_creation_token_t*)TW.Data);
            break;
        case PRINT_VAR:
            GenPrintVar((print_token_t*)TW.Data);
            break;
        case FUNCTION_TOKEN:
            GenFunctionCall((function_token_t*)TW.Data);
            break;
        case ASSIGNMENT_TOKEN:
            GenAssignment((assignment_token_t*)TW.Data);
            break;
        case IDENTIFIER_TOKEN:
            return GenIdentifier((identifier_token_t*)TW.Data);
        case BIN_OP:
            return GenBinOp((bin_op_token_t*)TW.Data);
        case IF_BRANCH:
            GenIfBranch((if_branch_token_t*)TW.Data);
            break;
        case WHILE_LOOP:
            GenWhileLoop((while_loop_token_t*)TW.Data);
            break;
        case NUMBER_TOKEN:
        case BLOCK_TOKEN:
        case STATEMENT_TOKEN:
        case EXPRESSION:
        case FACTOR:
        case TERM:
        case ATOM:
        case PROGRAM_TOKEN:
            break;
    }
    return SIZE_MAX;
}

static void GenWhileLoop(while_loop_token_t* WhileLoop)
{
    static size_t WhileNum = 0;
    char LoopHeader[30] = {0};
    char LoopBody[30] = {0};
    char LoopExit[30] = {0};
    snprintf(LoopHeader, 30, "_loop_header_%llu", WhileNum);
    snprintf(LoopBody, 30, "_loop_body_%llu", WhileNum);
    snprintf(LoopExit, 30, "_loop_exit_%llu", WhileNum);
    WhileNum++;

    fprintf(OutputFile, "br label %%%s\n%s:\n", LoopHeader, LoopHeader);

    char Op1[20] = {0};
    char Op2[20] = {0};

    if (WhileLoop->L.Type == NUMBER_TOKEN)
    {
        snprintf(Op1, 16, "%lf", ((number_token_t*)WhileLoop->L.Data)->Val);
    }
    else
    {
        snprintf(Op1, 16, "%%.%llu", DispatchCall(WhileLoop->L));
    }

    if (WhileLoop->R.Type == NUMBER_TOKEN)
    {
        snprintf(Op2, 16, "%lf", ((number_token_t*)WhileLoop->R.Data)->Val);
    }
    else
    {
        snprintf(Op2, 16, "%%.%llu", DispatchCall(WhileLoop->R));
    }

    size_t Self = ProcTempVar;
    ProcTempVar++;

    switch (WhileLoop->OpType)
    {
        case EQUAL: fprintf(OutputFile, "%%.%llu = fcmp oeq double %s, %s\n", Self, Op1, Op2); break;
        case NOT_EQUAL: fprintf(OutputFile, "%%.%llu = fcmp one double %s, %s\n", Self, Op1, Op2); break;
        case GREATER: fprintf(OutputFile, "%%.%llu = fcmp ogt double %s, %s\n", Self, Op1, Op2); break;
        case LESSER: fprintf(OutputFile, "%%.%llu = fcmp olt double %s, %s\n", Self, Op1, Op2); break;
        case GREATER_OR_EQ: fprintf(OutputFile, "%%.%llu = fcmp oge double %s, %s\n", Self, Op1, Op2); break;
        case LESSER_OR_EQ: fprintf(OutputFile, "%%.%llu = fcmp ole double %s, %s\n", Self, Op1, Op2); break;
    }

    fprintf(OutputFile, "br i1 %%.%llu, label %%%s, label %%%s\n", Self, LoopBody, LoopExit);
    fprintf(OutputFile, "%s:\n", LoopBody);

    for (size_t N = 0; N < WhileLoop->Statements.CurrSize; N++)
    {
        DispatchCall(WhileLoop->Statements.List[N]);
    }

    fprintf(OutputFile, "br label %%%s\n", LoopHeader);
    fprintf(OutputFile, "%s:\n", LoopExit);
}

static void GenIfBranch(if_branch_token_t* IfBranch)
{
    char Op1[20] = {0};
    char Op2[20] = {0};

    if (IfBranch->L.Type == NUMBER_TOKEN)
    {
        snprintf(Op1, 16, "%lf", ((number_token_t*)IfBranch->L.Data)->Val);
    }
    else
    {
        snprintf(Op1, 16, "%%.%llu", DispatchCall(IfBranch->L));
    }

    if (IfBranch->R.Type == NUMBER_TOKEN)
    {
        snprintf(Op2, 16, "%lf", ((number_token_t*)IfBranch->R.Data)->Val);
    }
    else
    {
        snprintf(Op2, 16, "%%.%llu", DispatchCall(IfBranch->R));
    }

    size_t Self = ProcTempVar;
    ProcTempVar++;

    switch (IfBranch->OpType)
    {
        case EQUAL: fprintf(OutputFile, "%%.%llu = fcmp oeq double %s, %s\n", Self, Op1, Op2); break;
        case NOT_EQUAL: fprintf(OutputFile, "%%.%llu = fcmp one double %s, %s\n", Self, Op1, Op2); break;
        case GREATER: fprintf(OutputFile, "%%.%llu = fcmp ogt double %s, %s\n", Self, Op1, Op2); break;
        case LESSER: fprintf(OutputFile, "%%.%llu = fcmp olt double %s, %s\n", Self, Op1, Op2); break;
        case GREATER_OR_EQ: fprintf(OutputFile, "%%.%llu = fcmp oge double %s, %s\n", Self, Op1, Op2); break;
        case LESSER_OR_EQ: fprintf(OutputFile, "%%.%llu = fcmp ole double %s, %s\n", Self, Op1, Op2); break;
    }

    static size_t IfNum = 0;
    char IfTrueLabel[30] = {0};
    char IfFalseLabel[30] = {0};
    snprintf(IfTrueLabel, 30, "_if_true_%llu", IfNum);
    snprintf(IfFalseLabel, 30, "_if_false_%llu", IfNum);
    IfNum++;

    fprintf(OutputFile, "br i1 %%.%llu, label %%%s, label %%%s\n", Self, IfTrueLabel, IfFalseLabel);
    fprintf(OutputFile, "%s:\n", IfTrueLabel);

    for (size_t N = 0; N < IfBranch->Statements.CurrSize; N++)
    {
        DispatchCall(IfBranch->Statements.List[N]);
    }

    fprintf(OutputFile, "br label %%%s\n", IfFalseLabel);
    fprintf(OutputFile, "%s:\n", IfFalseLabel);
}

static size_t GenBinOp(bin_op_token_t* BinOp)
{
    char Op1[20] = {0};
    char Op2[20] = {0};

    if (BinOp->L.Type == NUMBER_TOKEN)
    {
        snprintf(Op1, 16, "%lf", ((number_token_t*)BinOp->L.Data)->Val);
    }
    else
    {
        snprintf(Op1, 16, "%%.%llu", DispatchCall(BinOp->L));
    }

    if (BinOp->R.Type == NUMBER_TOKEN)
    {
        snprintf(Op2, 16, "%lf", ((number_token_t*)BinOp->R.Data)->Val);
    }
    else
    {
        snprintf(Op2, 16, "%%.%llu", DispatchCall(BinOp->R));
    }

    size_t Self = ProcTempVar;
    ProcTempVar++;

    switch (BinOp->Operator)
    {
        case PLUS:
            fprintf(OutputFile, "%%.%llu = fadd double %s, %s\n", Self, Op1, Op2);
            break;
        case MINUS:
            fprintf(OutputFile, "%%.%llu = fsub double %s, %s\n", Self, Op1, Op2);
            break;
        case MULT:
            fprintf(OutputFile, "%%.%llu = fmul double %s, %s\n", Self, Op1, Op2);
            break;
        case DIV:
            fprintf(OutputFile, "%%.%llu = fdiv double %s, %s\n", Self, Op1, Op2);
            break;
        case EXP:
            fprintf(OutputFile, "%%.%llu = call double (double, double) @pow(double %s, double %s)\n", Self, Op1, Op2);
            break;
    }

    return Self;
}

static size_t GenIdentifier(identifier_token_t* Id)
{
    size_t Self = ProcTempVar;
    ProcTempVar++;
    fprintf(OutputFile, "%%.%llu = load double, ptr @%s\n", Self, Id->Text);
    return Self;
}

static void GenAssignment(assignment_token_t* Assign)
{
    char* Var = ((identifier_token_t*)Assign->Name.Data)->Text;
    if (Assign->Val.Type == NUMBER_TOKEN)
    {
        double Num = ((number_token_t*)Assign->Val.Data)->Val;
        fprintf(OutputFile, "store double %lf, ptr @%s\n", Num, Var);
    }
    else
    {
        fprintf(OutputFile, "store double %%.%llu, ptr @%s\n", DispatchCall(Assign->Val), Var);
    }
}

static void GenFunctionCall(function_token_t* Func)
{
    identifier_token_t* ID = (identifier_token_t*)Func->FuncName.Data;
    fprintf(OutputFile, "call void @%s()\n", ID->Text);
}

static void GenPrintVar(print_token_t* Print)
{
    for (size_t N = 0; N < Print->Ids.CurrSize; N++)
    {
        identifier_token_t* ID = (identifier_token_t*)Print->Ids.List[N].Data;
        fprintf(OutputFile, "%%.%llu = load double, ptr @%s\n", ProcTempVar, ID->Text);
        ProcTempVar++;
        fprintf(OutputFile, "call i32 (ptr,...) @printf(ptr @FORMATSTR, double %%.%llu)\n", ProcTempVar - 1);
    }
}

static void GenProcCreation(proc_creation_token_t* Proc)
{
    identifier_token_t* ProcName = (identifier_token_t*)Proc->Name.Data;

    if (!strcmp(ProcName->Text, "main"))
    {
        fprintf(OutputFile, "define i32 @%s()\n{\n", ProcName->Text);
    }
    else
    {
        fprintf(OutputFile, "define void @%s()\n{\n", ProcName->Text);
    }

    ProcTempVar = 0;
    StatementNum = 0;
    for (size_t N = 0; N < Proc->Statements.CurrSize; N++)
    {
        DispatchCall(Proc->Statements.List[N]);
        StatementNum++;
    }
    ProcTempVar = 0;
    StatementNum = 0;

    if (!strcmp(ProcName->Text, "main"))
    {
        fputs("ret i32 0\n}\n", OutputFile);
    }
    else
    {
        fputs("ret void\n}\n", OutputFile);
    }
}

static void GenVarCreation(var_creation_token_t* VarC)
{
    for (size_t N = 0; N < VarC->Children.CurrSize; N++)
    {
        identifier_token_t* ID = (identifier_token_t*)VarC->Children.List[N].Data;
        fprintf(OutputFile, "@%s = global double 0.0\n", ID->Text);
    }
    fputs("\n", OutputFile);
}

static void GenProgram(program_token_t* Prog)
{
    fputs("@FORMATSTR = private constant [5 x i8] c\"%lf\\0A\\00\"\n", OutputFile);
    fputs("declare i32 @printf(ptr,...)\n", OutputFile);
    fputs("declare double @pow(double,double)\n\n", OutputFile);

    for (size_t N = 0; N < Prog->Blocks.CurrSize; N++)
    {
        DispatchCall(Prog->Blocks.List[N]);
    }
}

void DoCodegen(program_token_t* AST)
{
    if (!strcmp(CompilerFlags.OutputFile, "-"))
    {
        OutputFile = stdout;
    }
    else
    {
        char* OutputName = (char*)malloc(strlen(CompilerFlags.OutputFile) + 5);
        snprintf(OutputName, strlen(CompilerFlags.OutputFile) + 4, "%s.ll", CompilerFlags.OutputFile);
        OutputFile = fopen(OutputName, "wt");
    }

    GenProgram(AST);

    fclose(OutputFile);
}