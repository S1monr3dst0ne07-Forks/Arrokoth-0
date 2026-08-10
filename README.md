# Arrokoth-0

**Arrokoth-0**, name inspired by the trans-Neptunian object **[Arrokoth](https://en.wikipedia.org/wiki/486958_Arrokoth)** and inspired by the **PL/0**, is a very basic and simple programming language, created as a learning project about creating parsers, generating ASTs and writing a compiler. Arrokoth-0 generates an **LLVM IR** object that can be then compiled into a binary for any CPU architecture you desire.

## Grammar

In EBNF grammar form, the **Arrokoth-0** language can be defined as:

```ebnf
program = "program" statement ";" {statement ";"} "end" ;

statement = assignment | function_call ;

assignment = "var" identifier "=" expression ;

function_call = "run" identifier ;

expression = ["-"] term {("+"|"-") term} ;

term = factor {("*"|"/") factor} ;

factor = atom {"^" atom} ;

atom = identifier | string | number | "(" expression ")"
```

## Compiler stages

### Lexer

The program is broken up into lexical tokens, as defined in `lexer.c`, to be later parsed into the Abstract Syntax Tree.

### Parser

A recursive descent parser goes token by token, assembling the tree. This tree, however, is not perfect as it contains
residual `Expression`, `Term`, `Factor`, `Atom` tokens.

### Tree transform

Gets rid of `Expression`, `Term`, `Factor`, `Atom` and `Statement` tokens, while also enforcing rules such as a BinOp token can only have kids of type `Number`, `String` or `BinOp` and other rules. This is done to make code generation a bit smoother.

### Code generation

And the last stage, walking down the AST and emitting an LLVM IR file. This file is then compiled using `llc` (LLVM static compiler) and linked using `lld` (LLVM linker).
