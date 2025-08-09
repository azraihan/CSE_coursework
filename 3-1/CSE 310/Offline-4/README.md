# CSE 310 Offline-4: Intermediate Code Generation

A complete C-to-8086 Assembly compiler that translates C programs into optimized x86 assembly code with comprehensive code generation and optimization.

## Overview

This assignment extends the previous syntax/semantic analyzer to generate executable 8086 assembly code from C programs, featuring:
- **Code Generation**: Direct translation to 8086 assembly instructions
- **Memory Management**: Stack-based local variables and global variable handling
- **Control Flow**: Proper label generation for conditionals and loops
- **Function Calls**: Parameter passing and return value handling
- **Optimization**: Peephole optimizations for improved code quality

## Project Structure

```
├── C8086Lexer.g4             # ANTLR4 lexer grammar (8086 target)
├── C8086Parser.g4            # ANTLR4 parser with code generation actions
├── Ctester.cpp               # Main compiler driver
├── SymbolTable/              # Enhanced symbol table for code generation
│   ├── 2105009_SymbolTable.h/cpp    # Symbol table with offset tracking
│   ├── 2105009_ScopeTable.h/cpp     # Scope management
│   ├── 2105009_SymbolInfo.h/cpp     # Symbol info with memory layout
│   └── 2105009_hash.h/cpp           # Hash functions
├── optimizer/                # Code optimization module
│   ├── peephole_optimizer.h/cpp     # Peephole optimization implementation
├── lib/                      # Assembly runtime library
│   └── print.asm            # Print functions for output
├── input/                    # Test C programs
│   ├── test1_i.c to test7_i.c      # Basic functionality tests
│   ├── bonustest1_i.c, bonustest2_i.c  # Advanced recursive functions
│   ├── func.c, loop.c, exp.c       # Specific feature tests
├── run-script.sh            # Build and run script
└── clean-script.sh          # Cleanup script
```

## Features

### Code Generation
- **Arithmetic Operations**: Full expression evaluation with proper operator precedence
- **Memory Management**: Global variables in data segment, locals on stack
- **Control Structures**: Conditional jumps for if-else, loops with proper labeling
- **Function Calls**: Parameter pushing, stack management, return values
- **Array Handling**: Index calculation and memory addressing

### Assembly Output
- **8086 Compatible**: Generates standard x86 assembly code
- **Runtime Library**: Includes print functions for program output
- **Memory Layout**: Proper segment organization (.DATA, .CODE, .STACK)

### Optimization
- **Peephole Optimization**: Removes redundant instructions
- **Dead Code Elimination**: Removes unreferenced labels
- **Stack Optimization**: Eliminates unnecessary PUSH/POP pairs

## Installation

Requires ANTLR4. See the [installation guide](https://github.com/azraihan/CSE_coursework/blob/main/3-1/CSE%20310/Offline-3/Getting_Started_with_ANTLR4.pdf).

## Usage

### Compile C to Assembly
```bash
bash run-script.sh input/<input_file>
```

Example:
```bash
bash run-script.sh input/test1_i.c
```

### Output Files
Generated in the `output/` directory:
- `code.asm` - Generated 8086 assembly code
- `optcode.asm` - Optimized assembly code
- Additional log files for debugging

### Clean Build
```bash
bash clean-script.sh
```

## Test Programs

### Basic Tests
- `test1_i.c` - Arithmetic operations and variable assignments
- `test2_i.c` - Conditional statements (if-else)
- `test3_i.c` - Loop constructs (for, while)

### Advanced Tests  
- `test4_i.c` - Function calls and parameter passing
- `test5_i.c` - Complex expressions (e.g. nested function calls) and nested loops
- `test6_i.c` - Array operations and global variables
- `test7_i.c` - Complex logical expressions (short circuit codes)

### Bonus Tests
- `bonustest1_i.c` - Recursive function calls
- `bonustest2_i.c` - Nested recursive fucntion calls

## Technical Implementation

### Code Generation Strategy
- **Single-Pass**: Generates assembly during parsing
- **Stack-Based**: Expression evaluation using runtime stack
- **Label Management**: Automatic label generation for control flow
- **Symbol Resolution**: Offset calculation for variable access

### Memory Model
- **Global Variables**: Stored in data segment with direct addressing
- **Local Variables**: Stack-allocated with BP-relative addressing
- **Function Parameters**: Passed via stack with calling convention
- **Arrays**: Contiguous memory with computed indexing

### Register Usage
- **AX**: Primary accumulator for computations
- **BP/SP**: Stack frame management
- **BX/CX/DX**: Temporary storage and specialized operations
- **SI**: Array indexing support