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
//#include <arrokoth-0/tree_transform.h>
//#include <arrokoth-0/ast_graph.h>
//#include <arrokoth-0/semantic_analyzer.h>
//#include <arrokoth-0/codegen.h>

void usage()
{
    fprintf(stderr, "Usage: ./a.out -i <input> -o <output> [-a] [-ntta]\n");
    exit(1);
}

void error(char* msg)
{
    fprintf(stderr, "Error: %s.\n", msg);
    usage();
}

void arg_error(char* before)
{
    fprintf(stderr, "Error: Expected argument after '%s'.\n", before);
    usage();
}


int main(int argc, char** argv)
{
    compiler_params_t CompilerFlags = {
        .InputFile           = NULL,
        .OutputFile          = NULL,
        .GenerateAstGraph    = false,
        .GenerateAstGraphNTT = false,
    };

    #define READ_ARG ((n+1) < argc ? argv[++n] : (arg_error(argv[n]), NULL))

    if (argc == 0) usage();

    for (int n = 1; n < argc; n++)
    {
        /**/ if (!strcmp(argv[n], "-i"))    CompilerFlags.InputFile = READ_ARG;
        else if (!strcmp(argv[n], "-o"))    CompilerFlags.OutputFile = READ_ARG;
        else if (!strcmp(argv[n], "-a"))    CompilerFlags.GenerateAstGraph = true;
        else if (!strcmp(argv[n], "-ntta"))
            CompilerFlags.GenerateAstGraph = 
            CompilerFlags.GenerateAstGraphNTT = true;
    }

    if (!CompilerFlags.InputFile)  error("No input  file specified");
    if (!CompilerFlags.OutputFile) error("No output file specified");

    bool IsFromStdin = !strcmp(CompilerFlags.InputFile, "-");
    FILE* InputFd = IsFromStdin ? stdin : fopen(CompilerFlags.InputFile, "rb");

    if (!InputFd) error("Unable to open input file");

    token_t stream = DoLexicalAnalysis(InputFd);
    token_t streamer = stream;
    node_program_t* root = ParseProg(&streamer);

    /*
    if (CompilerFlags.GenerateAstGraph)
    {
        if (!CompilerFlags.GenerateAstGraphNTT)
        {
            DoTreeTransform(AST);
        }
        DoSemanticAnalyzation(AST);
        GenerateAstGraph(CompilerFlags, AST);
        return 0;
    }

    DoTreeTransform(AST);
    DoSemanticAnalyzation(AST);
    DoCodegen(CompilerFlags, AST);
    */

    return 0;
}
