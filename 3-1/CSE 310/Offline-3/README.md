# Syntax & Semantic Analysis

A C compiler frontend built with ANTLR4 that performs lexical analysis, syntax analysis, and semantic analysis with comprehensive error detection.

## Overview

This assignment implements a complete frontend for a subset of the C programming language, featuring:
- **Lexical Analysis**: Token recognition with detailed logging
- **Syntax Analysis**: Parse tree generation using ANTLR4 grammar
- **Semantic Analysis**: Symbol table management, type checking, scope validation
- **Error Handling**: Comprehensive syntax and semantic error reporting

## Project Structure

```
├── C2105009Lexer.g4          # ANTLR4 lexer grammar definition
├── C2105009Parser.g4         # ANTLR4 parser grammar definition
├── Ctester.cpp               # Main driver program
├── SymbolTable/              # Symbol table implementation
│   ├── 2105009_SymbolTable.h/cpp    # Main symbol table class
│   ├── 2105009_ScopeTable.h/cpp     # Individual scope management
│   ├── 2105009_SymbolInfo.h/cpp     # Symbol information storage
│   └── 2105009_hash.h/cpp           # Hash functions for symbol lookup
├── sampleio_abs/             # Test cases and expected outputs
├── input/                    # Sample input C programs
├── run-script.sh            # Build and run script
└── clean-script.sh          # Cleanup script
```

## Features

### Language Support
- Variable declarations (`int`, `float`, `void`)
- Function declarations and definitions
- Control structures (`if-else`, `for`, `while`)
- Expressions with operators (`+`, `-`, `*`, `/`, `%`, relational, logical)
- Arrays and function calls
- Print statements (`printf`)

### Error Detection
- Undeclared variables and functions
- Type mismatches in assignments and operations
- Multiple declarations in same scope
- Array access validation
- Function parameter mismatches
- Void function usage in expressions

## Installation

Ensure ANTLR4 is installed. See the [installation guide](https://github.com/azraihan/CSE_coursework/blob/main/3-1/CSE%20310/Offline-3/Getting_Started_with_ANTLR4.pdf).

## Usage

### Build and Run
```bash
bash run-script.sh sampleio_abs/<input_file>
```

Example:
```bash
bash run-script.sh sampleio_abs/input1.txt
```

### Output Files
All outputs are generated in the `output/` directory:
- `parserLog.txt` - Parsing log
- `errorLog.txt` - Syntax and semantic errors log
- `lexerLog.txt` - Token recognition log

### Clean Build
```bash
bash clean-script.sh
```

## Testing

The `sampleio_abs/` directory contains comprehensive test cases:
- `input1.txt` to `input5.txt` - Various C program examples
- `error2.txt`, `error4.txt`, `error5.txt` - Expected error outputs
- `log1.txt` to `log5.txt` - Expected parser logs

## Technical Details

- **Grammar**: Context-free grammar for C subset with error recovery
- **Symbol Table**: Hash table-based with scope chaining
- **Error Recovery**: Continues parsing after errors for comprehensive reporting
- **Memory Management**: Proper cleanup of parse trees and symbol tables