# Lexical Analyzer for C Subset

This implements a lexical analyzer for a subset of the C programming language using Flex (Fast Lexical Analyzer Generator). It scans source code and produces tokens for a compiler frontend, maintaining a symbol table for identifiers and constants.

## Project Structure

The project consists of several modules:
- Lexical analyzer (`.l` file)
- Symbol table implementation (from Assignment 1)
- Hash function implementations

## Building and Running

### Prerequisites

- Flex (Fast Lexical Analyzer Generator)
- GCC or similar C++ compiler

### Build Process

The project uses a Makefile with several targets:

```bash
# Build the lexical analyzer and run on all test inputs
make build-and-run-all
# or
make 

# Just generate C code from the flex file
make flex

# Compile the generated C code
make compile

# Run the lexical analyzer on test inputs
make test

# Clean up generated files
make clean
```

### Running Manually

You can also run the lexical analyzer manually with a specific input file:

```bash
flex 2105009.l
g++ -o lexer lex.yy.c SymbolTable/2105009_SymbolTable.cpp SymbolTable/2105009_ScopeTable.cpp SymbolTable/2105009_SymbolInfo.cpp SymbolTable/2105009_hash.cpp -I.
./lexer <input_file_path>
```

This will generate two output files:
- `2105009_<base_name>_token.txt` - Contains the token stream
- `2105009_<base_name>_log.txt` - Contains detailed log information