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

#include <arrokoth-0/ast_graph.h>
#include <arrokoth-0/parser.h>
#include <arrokoth-0/frontend.h>

static FILE* OutputFile;
static char* CrawlNumber(number_token_t* Num);
static char* CrawlIdentifier(identifier_token_t* Id);
static char* CrawlAtom(atom_token_t* Atom);
static char* CrawlFactor(factor_token_t* Factor);
static char* CrawlTerm(term_token_t* Term);
static char* CrawlExpression(expression_token_t* Expr);
static char* CrawlBinOp(bin_op_token_t* BinOp);
static char* CrawlPrintVar(print_token_t* PrintV);
static char* CrawlFunction(function_token_t* Func);
static char* CrawlAssignment(assignment_token_t* Assign);
static char* CrawlWhileLoop(while_loop_token_t* While);
static char* CrawlIfBranch(if_branch_token_t* IfBranch);
static char* CrawlStatement(statement_token_t* Statement);
static char* CrawlVarCreation(var_creation_token_t* VarC);
static char* CrawlProcCreation(proc_creation_token_t* Proc);
static char* CrawlBlock(block_token_t* Block);

static char* DispatchCall(token_wrapper_t TV)
{
    switch (TV.Type)
    {
        case PROGRAM_TOKEN:
            break;
        case BLOCK_TOKEN:
            return CrawlBlock((block_token_t*)TV.Data);
        case PROC_CREATION:
            return CrawlProcCreation((proc_creation_token_t*)TV.Data);
        case VAR_CREATION:
            return CrawlVarCreation((var_creation_token_t*)TV.Data);
        case STATEMENT_TOKEN:
            return CrawlStatement((statement_token_t*)TV.Data);
        case IF_BRANCH:
            return CrawlIfBranch((if_branch_token_t*)TV.Data);
        case WHILE_LOOP:
            return CrawlWhileLoop((while_loop_token_t*)TV.Data);
        case ASSIGNMENT_TOKEN:
            return CrawlAssignment((assignment_token_t*)TV.Data);
        case FUNCTION_TOKEN:
            return CrawlFunction((function_token_t*)TV.Data);
        case PRINT_VAR:
            return CrawlPrintVar((print_token_t*)TV.Data);
        case BIN_OP:
            return CrawlBinOp((bin_op_token_t*)TV.Data);
        case EXPRESSION:
            return CrawlExpression((expression_token_t*)TV.Data);
        case TERM:
            return CrawlTerm((term_token_t*)TV.Data);
        case FACTOR:
            return CrawlFactor((factor_token_t*)TV.Data);
        case ATOM:
            return CrawlAtom((atom_token_t*)TV.Data);
        case IDENTIFIER_TOKEN:
            return CrawlIdentifier((identifier_token_t*)TV.Data);
        case NUMBER_TOKEN:
            return CrawlNumber((number_token_t*)TV.Data);
    }
    return "IMPOSSIBLE";
}

static char* CrawlNumber(number_token_t* Num)
{
    static size_t NumNum = 0;
    char* NumName = (char*)malloc(64);
    snprintf(NumName, 64, "Num%lld", NumNum);
    fprintf(OutputFile, "%s [label=\"%lf\",fillcolor=white,style=filled,fontcolor=black]\n", NumName, Num->Val);
    NumNum++;
    return NumName;
}

static char* CrawlIdentifier(identifier_token_t* Id)
{
    static size_t IDNum = 0;
    char* IDName = (char*)malloc(64);
    snprintf(IDName, 64, "ID%lld", IDNum);
    fprintf(OutputFile, "%s [label=\"%s\",fillcolor=white,style=filled,fontcolor=black]\n", IDName, Id->Text);
    IDNum++;
    return IDName;
}

static char* CrawlAtom(atom_token_t* Atom)
{
    static size_t AtomNum = 0;
    char* AtomName = (char*)malloc(64);
    snprintf(AtomName, 64, "Atom%lld", AtomNum);
    fprintf(OutputFile, "%s [label=Atom,fillcolor=gray50,style=filled,fontcolor=black]\n", AtomName);
    AtomNum++;

    char* ConnectTo = DispatchCall(Atom->Child);
    fprintf(OutputFile, "%s -> %s\n", AtomName, ConnectTo);

    return AtomName;
}

static char* CrawlFactor(factor_token_t* Factor)
{
    static size_t FactorNum = 0;
    char* FactorName = (char*)malloc(64);
    snprintf(FactorName, 64, "Factor%lld", FactorNum);
    fprintf(OutputFile, "%s [label=Factor,fillcolor=gray40,style=filled,fontcolor=white]\n", FactorName);
    FactorNum++;

    char* ConnectTo = DispatchCall(Factor->Child);
    fprintf(OutputFile, "%s -> %s\n", FactorName, ConnectTo);

    return FactorName;
}

static char* CrawlTerm(term_token_t* Term)
{
    static size_t TermNum = 0;
    char* TermName = (char*)malloc(64);
    snprintf(TermName, 64, "Term%lld", TermNum);
    fprintf(OutputFile, "%s [label=Term,fillcolor=gray39,style=filled,fontcolor=white]\n", TermName);
    TermNum++;

    char* ConnectTo = DispatchCall(Term->Child);
    fprintf(OutputFile, "%s -> %s\n", TermName, ConnectTo);

    return TermName;
}

static char* CrawlExpression(expression_token_t* Expr)
{
    static size_t ExprNum = 0;
    char* ExprName = (char*)malloc(64);
    snprintf(ExprName, 64, "Expr%lld", ExprNum);
    fprintf(OutputFile, "%s [label=Expression,fillcolor=gray30,style=filled,fontcolor=white]\n", ExprName);
    ExprNum++;

    char* ConnectTo = DispatchCall(Expr->Child);
    fprintf(OutputFile, "%s -> %s\n", ExprName, ConnectTo);

    return ExprName;
}

static char* CrawlBinOp(bin_op_token_t* BinOp)
{
    static size_t BinOpNum = 0;
    char* BinOpName = (char*)malloc(64);
    snprintf(BinOpName, 64, "BinOp%lld", BinOpNum);
    char* OpName = "+";
    OpName = (BinOp->Operator == MINUS) ? "-" : OpName;
    OpName = (BinOp->Operator == MULT) ? "*" : OpName;
    OpName = (BinOp->Operator == DIV) ? "/" : OpName;
    OpName = (BinOp->Operator == EXP) ? "^" : OpName;
    fprintf(OutputFile, "%s [label=\"Binary Operation: %s\",fillcolor=yellow,style=filled,fontcolor=black]\n", BinOpName, OpName);
    BinOpNum++;

    char* ConnectTo1 = DispatchCall(BinOp->L);
    char* ConnectTo2 = DispatchCall(BinOp->R);
    fprintf(OutputFile, "%s -> %s\n", BinOpName, ConnectTo1);
    fprintf(OutputFile, "%s -> %s\n", BinOpName, ConnectTo2);

    return BinOpName;
}

static char* CrawlPrintVar(print_token_t* PrintV)
{
    static size_t PrintNum = 0;
    char* PrintName = (char*)malloc(64);
    snprintf(PrintName, 64, "Print%lld", PrintNum);
    fprintf(OutputFile, "%s [label=\"Print variables\",fillcolor=darkorange1,style=filled,fontcolor=black]\n", PrintName);
    PrintNum++;

    for (size_t N = 0; N < PrintV->Ids.CurrSize; N++)
    {
        char* ConnectTo = DispatchCall(PrintV->Ids.List[N]);
        fprintf(OutputFile, "%s -> %s\n", PrintName, ConnectTo);
    }

    return PrintName;
}

static char* CrawlFunction(function_token_t* Func)
{
    static size_t FuncNum = 0;
    char* FuncName = (char*)malloc(64);
    snprintf(FuncName, 64, "Func%lld", FuncNum);
    fprintf(OutputFile, "%s [label=\"Function Call\",fillcolor=green,style=filled,fontcolor=black]\n", FuncName);
    FuncNum++;

    char* ConnectTo = DispatchCall(Func->FuncName);
    fprintf(OutputFile, "%s -> %s\n", FuncName, ConnectTo);

    return FuncName;
}

static char* CrawlAssignment(assignment_token_t* Assign)
{
    static size_t AssignNum = 0;
    char* AssignName = (char*)malloc(64);
    snprintf(AssignName, 64, "Assign%lld", AssignNum);
    fprintf(OutputFile, "%s [label=Assignment,fillcolor=blue,style=filled,fontcolor=white]\n", AssignName);
    AssignNum++;

    char* ConnectTo1 = DispatchCall(Assign->Name);
    char* ConnectTo2 = DispatchCall(Assign->Val);
    fprintf(OutputFile, "%s -> %s\n", AssignName, ConnectTo1);
    fprintf(OutputFile, "%s -> %s\n", AssignName, ConnectTo2);

    return AssignName;
}

static char* CrawlWhileLoop(while_loop_token_t* While)
{
    static size_t WhileNum = 0;
    char* WhileName = (char*)malloc(64);
    snprintf(WhileName, 64, "While%lld", WhileNum);
    char* OpName = "==";
    OpName = (While->OpType == NOT_EQUAL) ? "!=" : OpName;
    OpName = (While->OpType == GREATER) ? ">" : OpName;
    OpName = (While->OpType == LESSER) ? "<" : OpName;
    OpName = (While->OpType == GREATER_OR_EQ) ? ">=" : OpName;
    OpName = (While->OpType == LESSER_OR_EQ) ? "<=" : OpName;
    fprintf(OutputFile, "%s [label=\"While (L %s R)\",fillcolor=brown,style=filled,fontcolor=black]\n", WhileName, OpName);
    WhileNum++;

    char* ConnectTo1 = DispatchCall(While->L);
    char* ConnectTo2 = DispatchCall(While->R);
    fprintf(OutputFile, "%s -> %s\n", WhileName, ConnectTo1);
    fprintf(OutputFile, "%s -> %s\n", WhileName, ConnectTo2);

    for (size_t N = 0; N < While->Statements.CurrSize; N++)
    {
        char* ConnectTo = DispatchCall(While->Statements.List[N]);
        fprintf(OutputFile, "%s -> %s\n", WhileName, ConnectTo);
    }

    return WhileName;
}

static char* CrawlIfBranch(if_branch_token_t* IfBranch)
{
    static size_t IfNum = 0;
    char* IfName = (char*)malloc(64);
    snprintf(IfName, 64, "If%lld", IfNum);
    char* OpName = "==";
    OpName = (IfBranch->OpType == NOT_EQUAL) ? "!=" : OpName;
    OpName = (IfBranch->OpType == GREATER) ? ">" : OpName;
    OpName = (IfBranch->OpType == LESSER) ? "<" : OpName;
    OpName = (IfBranch->OpType == GREATER_OR_EQ) ? ">=" : OpName;
    OpName = (IfBranch->OpType == LESSER_OR_EQ) ? "<=" : OpName;
    fprintf(OutputFile, "%s [label=\"If (L %s R)\",fillcolor=cadetblue1,style=filled,fontcolor=black]\n", IfName, OpName);
    IfNum++;

    char* ConnectTo1 = DispatchCall(IfBranch->L);
    char* ConnectTo2 = DispatchCall(IfBranch->R);
    fprintf(OutputFile, "%s -> %s\n", IfName, ConnectTo1);
    fprintf(OutputFile, "%s -> %s\n", IfName, ConnectTo2);

    for (size_t N = 0; N < IfBranch->Statements.CurrSize; N++)
    {
        char* ConnectTo = DispatchCall(IfBranch->Statements.List[N]);
        fprintf(OutputFile, "%s -> %s\n", IfName, ConnectTo);
    }

    return IfName;
}

static char* CrawlStatement(statement_token_t* Statement)
{
    static size_t StatementNum = 0;
    char* StatementName = (char*)malloc(64);
    snprintf(StatementName, 64, "Stmt%lld", StatementNum);
    fprintf(OutputFile, "%s [label=Statement,fillcolor=gray28,style=filled,fontcolor=white]\n", StatementName);
    StatementNum++;

    char* ConnectTo = DispatchCall(Statement->Child);
    fprintf(OutputFile, "%s -> %s\n", StatementName, ConnectTo);

    return StatementName;
}

static char* CrawlVarCreation(var_creation_token_t* VarC)
{
    static size_t VarCNum = 0;
    char* VarCName = (char*)malloc(64);
    snprintf(VarCName, 64, "VarC%lld", VarCNum);
    fprintf(OutputFile, "%s [label=\"Variable Creation\",fillcolor=pink,style=filled,fontcolor=black]\n", VarCName);
    VarCNum++;

    for (size_t N = 0; N < VarC->Children.CurrSize; N++)
    {
        char* ConnectTo = DispatchCall(VarC->Children.List[N]);
        fprintf(OutputFile, "%s -> %s\n", VarCName, ConnectTo);
    }

    return VarCName;
}

static char* CrawlProcCreation(proc_creation_token_t* Proc)
{
    static size_t ProcNum = 0;
    char* ProcName = (char*)malloc(64);
    snprintf(ProcName, 64, "Proc%lld", ProcNum);
    fprintf(OutputFile, "%s [label=\"Procedure Creation\",fillcolor=purple,style=filled,fontcolor=white]\n", ProcName);
    ProcNum++;

    char* ConnectTo1 = DispatchCall(Proc->Name);
    fprintf(OutputFile, "%s -> %s\n", ProcName, ConnectTo1);
    for (size_t N = 0; N < Proc->Statements.CurrSize; N++)
    {
        char* ConnectTo = DispatchCall(Proc->Statements.List[N]);
        fprintf(OutputFile, "%s -> %s\n", ProcName, ConnectTo);
    }

    return ProcName;
}

static char* CrawlBlock(block_token_t* Block)
{
    static size_t BlockNum = 0;
    char* BlockName = (char*)malloc(64);
    snprintf(BlockName, 64, "Blck%lld", BlockNum);
    fprintf(OutputFile, "%s [label=Block,fillcolor=gray10,style=filled,fontcolor=white]\n", BlockName);
    BlockNum++;

    char* ConnectTo = DispatchCall(Block->Child);
    fprintf(OutputFile, "%s -> %s\n", BlockName, ConnectTo);

    return BlockName;
}

static void CrawlProgram(program_token_t* Prog)
{
    fputs("PROG [label=Program,fillcolor=red,style=filled]\n", OutputFile);
    for (size_t N = 0; N < Prog->Blocks.CurrSize; N++)
    {
        char* ConnectTo = DispatchCall(Prog->Blocks.List[N]);
        fprintf(OutputFile, "PROG -> %s\n", ConnectTo);
    }
}

void GenerateAstGraph(program_token_t* AST)
{
    fputs(CompilerFlags.OutputFile, OutputFile);
    if (!strcmp(CompilerFlags.OutputFile, "-"))
    {
        OutputFile = stdout;
    }
    else
    {
        char* OutputName = (char*)malloc(strlen(CompilerFlags.OutputFile) + 5);
        snprintf(OutputName, strlen(CompilerFlags.OutputFile) + 5, "%s.dot", CompilerFlags.OutputFile);
        OutputFile = fopen(OutputName, "wt");
    }

    fputs("digraph\n{\n", OutputFile);
    CrawlProgram(AST);
    fputs("}", OutputFile);

    fclose(OutputFile);
}