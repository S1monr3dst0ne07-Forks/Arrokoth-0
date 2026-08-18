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
#include <stdint.h>

#include <arrokoth-0/parser.h>
#include <arrokoth-0/tree_transform.h>

typedef struct crawl_result_s
{
    void* NewData;
    enum ParserTokenType NewType;
}
crawl_result_t;

static crawl_result_t CrawlAtom(atom_token_t* Atom);
static crawl_result_t CrawlFactor(factor_token_t* Factor);
static crawl_result_t CrawlTerm(term_token_t* Term);
static crawl_result_t CrawlBinOp(bin_op_token_t* BinOp);
static crawl_result_t CrawlUnaryOp(unary_minus_token_t* UnOp);
static crawl_result_t CrawlExpression(expression_token_t* Expr);
static crawl_result_t CrawlAssignment(assignment_token_t* Assign);
static crawl_result_t CrawlIfBranch(if_branch_token_t* IfBranch);
static crawl_result_t CrawlWhileLoop(while_loop_token_t* WhileLoop);
static crawl_result_t CrawlStatement(statement_token_t* Stmt);
static crawl_result_t CrawlProcCreation(proc_creation_token_t* ProcCreation);
static crawl_result_t CrawlBlock(block_token_t* Block);

static crawl_result_t DispatchCall(void* Data, enum ParserTokenType Type)
{
    switch (Type)
    {
        case EXPRESSION:
            return CrawlExpression((expression_token_t*)Data);
        case UNARY_MINUS:
            return CrawlUnaryOp((unary_minus_token_t*)Data);
        case BIN_OP:
            return CrawlBinOp((bin_op_token_t*)Data);
        case TERM:
            return CrawlTerm((term_token_t*)Data);
        case FACTOR:
            return CrawlFactor((factor_token_t*)Data);
        case ATOM:
            return CrawlAtom((atom_token_t*)Data);
        default:
            break;
    }
    return (crawl_result_t){Data, Type};
}

static crawl_result_t CrawlAtom(atom_token_t* Atom)
{
    crawl_result_t Res = DispatchCall(Atom->Child.Data, Atom->Child.Type);
    free(Atom);
    return Res;
}

static crawl_result_t CrawlFactor(factor_token_t* Factor)
{
    crawl_result_t Res = DispatchCall(Factor->Child.Data, Factor->Child.Type);
    free(Factor);
    return Res;
}

static crawl_result_t CrawlTerm(term_token_t* Term)
{
    crawl_result_t Res = DispatchCall(Term->Child.Data, Term->Child.Type);
    free(Term);
    return Res;
}

static crawl_result_t CrawlBinOp(bin_op_token_t* BinOp)
{
    crawl_result_t Res;
    Res.NewData = BinOp;
    Res.NewType = BIN_OP;

    crawl_result_t LValueRes = DispatchCall(BinOp->L.Data, BinOp->L.Type);
    BinOp->L.Data = LValueRes.NewData;
    BinOp->L.Type = LValueRes.NewType;

    crawl_result_t RValueRes = DispatchCall(BinOp->R.Data, BinOp->R.Type);
    BinOp->R.Data = RValueRes.NewData;
    BinOp->R.Type = RValueRes.NewType;

    return Res;
}

static crawl_result_t CrawlUnaryOp(unary_minus_token_t* UnOp)
{
    crawl_result_t Res;
    Res.NewData = UnOp;
    Res.NewType = UNARY_MINUS;

    crawl_result_t ValueRes = DispatchCall(UnOp->Child.Data, UnOp->Child.Type);
    UnOp->Child.Data = ValueRes.NewData;
    UnOp->Child.Type = ValueRes.NewType;

    return Res;
}

static crawl_result_t CrawlExpression(expression_token_t* Expr)
{
    crawl_result_t Res = DispatchCall(Expr->Child.Data, Expr->Child.Type);
    free(Expr);
    return Res;
}

static crawl_result_t CrawlAssignment(assignment_token_t* Assign)
{
    crawl_result_t Res;
    Res.NewData = Assign;
    Res.NewType = ASSIGNMENT_TOKEN;

    crawl_result_t ValueRes = DispatchCall(Assign->Val.Data, Assign->Val.Type);
    Assign->Val.Data = ValueRes.NewData;
    Assign->Val.Type = ValueRes.NewType;

    return Res;
}

static crawl_result_t CrawlIfBranch(if_branch_token_t* IfBranch)
{
    crawl_result_t Res;
    Res.NewData = IfBranch;
    Res.NewType = IF_BRANCH;

    crawl_result_t LRes = DispatchCall(IfBranch->L.Data, IfBranch->L.Type);
    IfBranch->L.Data = LRes.NewData;
    IfBranch->L.Type = LRes.NewType;

    crawl_result_t RRes = DispatchCall(IfBranch->R.Data, IfBranch->R.Type);
    IfBranch->R.Data = RRes.NewData;
    IfBranch->R.Type = RRes.NewType;

    for (size_t N = 0; N < IfBranch->Statements.CurrSize; N++)
    {
        crawl_result_t Res = CrawlStatement((statement_token_t*)IfBranch->Statements.List[N].Data);
        IfBranch->Statements.List[N].Data = Res.NewData;
        IfBranch->Statements.List[N].Type = Res.NewType;
    }

    return Res;
}

static crawl_result_t CrawlWhileLoop(while_loop_token_t* WhileLoop)
{
    crawl_result_t Res;
    Res.NewData = WhileLoop;
    Res.NewType = WHILE_LOOP;

    crawl_result_t LRes = DispatchCall(WhileLoop->L.Data, WhileLoop->L.Type);
    WhileLoop->L.Data = LRes.NewData;
    WhileLoop->L.Type = LRes.NewType;

    crawl_result_t RRes = DispatchCall(WhileLoop->R.Data, WhileLoop->R.Type);
    WhileLoop->R.Data = RRes.NewData;
    WhileLoop->R.Type = RRes.NewType;

    for (size_t N = 0; N < WhileLoop->Statements.CurrSize; N++)
    {
        crawl_result_t Res = CrawlStatement((statement_token_t*)WhileLoop->Statements.List[N].Data);
        WhileLoop->Statements.List[N].Data = Res.NewData;
        WhileLoop->Statements.List[N].Type = Res.NewType;
    }

    return Res;
}

static crawl_result_t CrawlStatement(statement_token_t* Stmt)
{
    crawl_result_t Res;

    if (Stmt->Child.Type == ASSIGNMENT_TOKEN)
    {
        Res = CrawlAssignment((assignment_token_t*)Stmt->Child.Data);
    }
    else if (Stmt->Child.Type == FUNCTION_TOKEN)
    {
        Res.NewData = Stmt->Child.Data;
        Res.NewType = FUNCTION_TOKEN;
    }
    else if (Stmt->Child.Type == PRINT_VAR)
    {
        Res.NewData = Stmt->Child.Data;
        Res.NewType = PRINT_VAR;
    }
    else if (Stmt->Child.Type == WHILE_LOOP)
    {
        Res = CrawlWhileLoop((while_loop_token_t*)Stmt->Child.Data);
    }
    else if (Stmt->Child.Type == IF_BRANCH)
    {
        Res = CrawlIfBranch((if_branch_token_t*)Stmt->Child.Data);
    }

    free(Stmt);
    return Res;
}

static crawl_result_t CrawlProcCreation(proc_creation_token_t* ProcCreation)
{
    crawl_result_t Res;
    Res.NewData = ProcCreation;
    Res.NewType = PROC_CREATION;

    for (size_t N = 0; N < ProcCreation->Statements.CurrSize; N++)
    {
        crawl_result_t Res = CrawlStatement((statement_token_t*)ProcCreation->Statements.List[N].Data);
        ProcCreation->Statements.List[N].Data = Res.NewData;
        ProcCreation->Statements.List[N].Type = Res.NewType;
    }

    return Res;
}

static crawl_result_t CrawlBlock(block_token_t* Block)
{
    crawl_result_t Res;

    if (Block->Child.Type == VAR_CREATION)
    {
        Res.NewType = Block->Child.Type;
        Res.NewData = Block->Child.Data;
    }
    else if (Block->Child.Type == PROC_CREATION)
    {
        Res = CrawlProcCreation((proc_creation_token_t*)Block->Child.Data);
    }

    free(Block);
    return Res;
}

void DoTreeTransform(program_token_t* AST)
{
    for (size_t N = 0; N < AST->Blocks.CurrSize; N++)
    {
        crawl_result_t Res = CrawlBlock(((block_token_t*)AST->Blocks.List[N].Data));
        AST->Blocks.List[N].Data = Res.NewData;
        AST->Blocks.List[N].Type = Res.NewType;
    }
}