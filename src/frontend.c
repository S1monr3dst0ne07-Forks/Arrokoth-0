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
#include <stdint.h>

#include <arrokoth-0/frontend.h>
#include <arrokoth-0/lexer.h>
#include <arrokoth-0/parser.h>
#include <arrokoth-0/tree_transform.h>
#include <arrokoth-0/ast_graph.h>
#include <arrokoth-0/semantic_analyzer.h>

#define I_ARG_HASH 5861499
#define O_ARG_HASH 5861505
#define A_ARG_HASH 5861491
#define NTTA_ARG_HASH 210644999497

compiler_flags_t CompilerFlags;

// djb2 hash function (http://www.cse.yorku.ca/~oz/hash.html)
static uint64_t Hash(const char* Str)
{
    uint64_t StartHash = 5381;
    size_t N = 0;

    while (Str[N])
    {
        StartHash = ((StartHash << 5) + StartHash) + Str[N];
        N++;
    }

    return StartHash;
}

static void ZeroFlags(compiler_flags_t* CF)
{
    CF->InputFile = NULL;
    CF->OutputFile = NULL;
}

static void CheckArgValidity(char** Argv, size_t Pos)
{
    if (!Argv[Pos])
    {
        fputs("error: unexpected end of cmd arg list", stderr);
        exit(-1);
    }
}

static void InputArg(char** Argv, size_t Pos)
{
    CheckArgValidity(Argv, Pos + 1);
    if (CompilerFlags.InputFile)
    {
        fputs("error: '-i' is already set", stderr);
        exit(-1);
    }
    CompilerFlags.InputFile = Argv[Pos + 1];
}

static void OutputArg(char** Argv, size_t Pos)
{
    CheckArgValidity(Argv, Pos + 1);
    if (CompilerFlags.OutputFile)
    {
        fputs("error: '-o' is already set", stderr);
        exit(-1);
    }
    CompilerFlags.OutputFile = Argv[Pos + 1];
}

static void AstArg()
{
    if (CompilerFlags.GenerateAstGraph)
    {
        fputs("error: '-a' or '-ntta' are already set", stderr);
        exit(-1);
    }
    CompilerFlags.GenerateAstGraph = 1;
}

static void NttAstArg()
{
    if (CompilerFlags.GenerateAstGraph)
    {
        fputs("error: '-a' or '-ntta' are already set", stderr);
        exit(-1);
    }
    if (CompilerFlags.GenerateAstGraphNTT)
    {
        fputs("error: '-ntta' is already set", stderr);
        exit(-1);
    }
    CompilerFlags.GenerateAstGraph = 1;
    CompilerFlags.GenerateAstGraphNTT = 1;
}

int main(int argc, char** argv)
{
    ZeroFlags(&CompilerFlags);

    for (int N = 1; N < argc; N++)
    {
        switch (Hash(argv[N]))
        {
            case I_ARG_HASH:
                InputArg(argv, N);
                break;
            case O_ARG_HASH:
                OutputArg(argv, N);
                break;
            case A_ARG_HASH:
                AstArg();
                break;
            case NTTA_ARG_HASH:
                NttAstArg();
                break;
        }
    }

    FILE* ToRead = NULL;
    if (!strcmp(CompilerFlags.InputFile, "-"))
    {
        ToRead = stdin;
    }
    else
    {
        ToRead = fopen(CompilerFlags.InputFile, "rb");
    }

    linked_list_node_t* Tokens = NULL;

    char Line[4096];
    size_t LineC = 1;
    while (fgets(Line, 4096, ToRead) != NULL)
    {
        if (Tokens)
        {
            linked_list_node_t* NewTokens = DoLexicalAnalysis(Line, LineC);
            LinkedListAppendDirectly(Tokens, NewTokens);
        }
        else
        {
            Tokens = DoLexicalAnalysis(Line, LineC);
        }
        LineC++;
    }

    program_token_t* AST = DoParseAST(Tokens);

    if (CompilerFlags.GenerateAstGraph)
    {
        if (!CompilerFlags.GenerateAstGraphNTT)
        {
            DoTreeTransform(AST);
        }
        DoSemanticAnalyzation(AST);
        GenerateAstGraph(AST);
        return 0;
    }

    return 0;
}