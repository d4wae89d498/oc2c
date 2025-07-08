# oc2c

A **partial Objective‑C 1.0 → C99 transpiler** written in plain C99, designed as a small, easily bootstrap‑able OOP language toolchain for embedded, exotic hardware, or experimental environments.

---

## Project Overview

`oc2c` takes a subset of Objective‑C syntax (interfaces, implementations, method calls, etc.) and “desugars” it directly into equivalent C code and can emit `libobjc` calls using a flag. All other C constructs are treated as opaque “raw” nodes and passed through unchanged—no full C grammar parsing is attempted. This lightweight, **partial transpiler** approach keeps the binary small and the bootstrap process straightforward.

---

## Motivation

Objective‑C’s minimal syntactic overlay on C makes it uniquely amenable to a **partial transpiler**:

1. **Sparse Grammar Extension**
   Only a small set of Obj‑C tokens (`@interface`, `@implementation`, message syntax) augment C. All other constructs remain valid C, allowing a lightweight recursive‑descent parser with backtracking to recognize just these elements and pass through raw C unchanged.

2. **Direct Semantic Lowering**
   Classes map to C `struct` + vtable pointers, and message sends desugar to `objc_msgSend` calls. No deep semantic analysis or full AST is required—syntax‑level rewriting suffices.

3. **Runtime‑Driven Binding**
   Dynamic dispatch and lack of compile‑time type resolution eliminate the need for symbol tables or overload resolution, further reducing compiler complexity.

4. **Opaque C Pass‑Through**
   Unmodified C code (expressions, preprocessor directives, macros) is treated as opaque “raw” nodes, obviating full C grammar support and enabling rapid bootstrap on constrained platforms.

Because Objective‑C was explicitly designed as a thin layer over C, a partial transpiler of only a few dozen lines of C99 can fully capture its semantics while leaving the underlying C code intact.

---

## Use cases:

- **Embedded & Exotic HW**  
  Minimize dependencies so you can build on bare‐metal or in freestanding toolchains. This transpiler was written in order to build compilers in objective-c.
- **Experiments & Prototyping**  
  Quickly test OOP ideas without a heavy compiler frontend.  
- **Bootstrap Simplicity**  
  C99 only—no C++ or external parser generators—ideal for bootstrapping new architectures.

---

## Key Features

- **Partial Transpilation**  
  Only Objective‑C constructs (classes, methods, ivars, messages) are recognized and translated. All other C code is left untouched.
- **C99‑Only Implementation**  
  No external libraries or language extensions—just a single ANSI‑compatible toolchain.
- **Struct‑based Polymorphism**  
  Every AST node carries a function pointer “accept” method; visitors (e.g. pretty‑printer or C‑code generator) implement polymorphic behavior via struct of callbacks.
- **Backtracking‑enabled Recursive Descent Parser**  
  The `try_parse` macro + `save/restore` parser state let you compose complex, backtracking grammar rules in a compact way—no separate lexer, no parser generator.
- **Visitor Pattern**  
  Two built‑in visitors:  
  - **Dumper** (`visitors/dumper.c`): prints the AST for debugging  
  - **Transpiler** (`visitors/transpiler.c`): emits C code for recognized Objective‑C nodes  
- **Optional** support for Objective-C runtime library `libobjc` and interoperability.

---

## Architecture

1. **Parsing**  
   - Input is scanned character‑by‑character.  
   - Whitespace is skipped; identifiers and Objective‑C keywords are recognized.  
   - On failure, `try_parse` restores parser position and tries the next alternative.

2. **AST & Polymorphism**  
   - Every node type (e.g. `interface_node`, `method_node`) embeds a base `ast_node` with an `accept` pointer.  
   - Visitors implement callbacks for each node type—this gives you classic OOP‑style dispatch in pure C.

3. **Code Generation**  
   - The “transpiler” visitor walks the AST and emits the equivalent C code for Objective‑C constructs.  
   - Raw C fragments are emitted verbatim.

---

## Getting Started

```sh
git clone https://…/oc2c.git
cd oc2c
make
./oc2c.sh example.m > output.c
gcc -std=c99 -o prog output.c
````

* **`oc2c.sh`**: simple wrapper around `./oc2c` to read `spec.m` or custom files.
* **`Makefile`**: builds into a single small `oc2c` executable with no external dependencies.

---

## Example

**Input (`example.m`):**

```objc
@interface Foo : Object
- (void)hello;
@end

@implementation Foo
- (void)hello { printf("Hello, world!\n"); }
@end
```

**Generated C (`output.c`):**

```c
/* desugared Foo */
typedef struct Foo { Object base; } Foo;
void Foo_hello(Foo *self) { printf("Hello, world!\n"); }
/* … */
```



---

## License

[MIT License](LICENSE)