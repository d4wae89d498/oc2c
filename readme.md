# oc2c

A **partial Objective‑C 1.0 → C99 transpiler** written in plain C99, designed as a small, easily bootstrap‑able OOP language toolchain for embedded, exotic hardware, or experimental environments.


`oc2c` takes a subset of early Objective‑C 1.0 syntax (interfaces, implementations, message passings, encode, selector) and “desugars” it directly into equivalent C code and emit `libobjc` calls. Only C expressions are parsed to avoid ambiguities between nested C/OBJC expressions; a fully TranslationUnit parsing is not needed hence reducing parser size.

Because Objective‑C was explicitly designed as a thin layer over C, a partial transpiler of only a few dozen lines of C99 can fully capture its semantics while leaving the underlying C code intact.

---


## Key Features

- **Partial Transpilation**  
  Only few Objective‑C constructs are recognized and translated. All other C code is left untouched.
- **Support** for officials Objective-C runtime library `libobjc`. A small `libobjc` is also provided and aims to be runtime compatible with officials implementations.
- **C99‑Only Implementation**  
  No external libraries or language extensions—just a single ANSI‑compatible toolchain.
- **Struct‑based Polymorphism**  
  Every AST node carries a function pointer “accept” method; visitors (e.g. pretty‑printer or C‑code generator) implement polymorphic behavior via struct of callbacks.
- **Backtracking / Recursive Descent Parser**  
  The `try_parse` macro + `save/restore` parser state let you compose complex, backtracking grammar rules in a compact way—no separate lexer, no parser generator.
- **Extensible via a 'Visitor alike' Pattern**  
  Built‑in 'visitors':  
  - **Dumper** (`visitors/dumper.c`): prints the AST for debugging  
  - **Transpiler** (`visitors/transpiler.c`): emits C code for recognized Objective‑C nodes  

