# Orcus-0

**Orcus-0**, name inspired by the trans-Neptunian object **Orcus** and inspired by the **PL/0**, is a very basic and simple programming language, created as a learning project about creating parsers, generating ASTs and writing a compiler. Orcus-0 generates an **LLVM IR** object that can be then compiled into a binary for any CPU architecture you desire.

## Grammar

In EBNF grammar form, the **Orcus-0** language can be defined as:

```ebnf
program = "program" statement ";" {statement ";"} "end" ;

statement = assignment | function_call ;

assignment = identifier "=" expression ;

function_call = "run" identifier ;

expression = ["-"] term {("+"|"-") term} ;

term = factor {("*"|"/") factor} ;

factor = atom {"^" atom} ;

atom = identifier | number | "(" expression ")"
```
