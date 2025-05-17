# Symbol Table Implementation

This implements a symbol table for compiler construction, with support for multiple hash functions and scope management.

## Building and Running

Uses a makefile for easy compilation and execution. Below are the available commands:

### Basic Commands

```bash
# Clean all build files
make clean

# Build the project
make build

# Run the program with sample input
make run

# Clean, build, and run in sequence
make run-all
# or
make
```

### Running with Custom Input/Output

By default, the program uses `sample_input.txt` as input and writes to `output.txt`:

```bash
g++ -fsanitize=address 2105009_main.cpp 2105009_hash.cpp 2105009_SymbolInfo.cpp 2105009_ScopeTable.cpp 2105009_SymbolTable.cpp 2105009_utils.cpp -o symboltable
./symboltable <input_file> <output_file>
```

### Hash Function Evaluation

This also includes a utility to evaluate different hash functions:

```bash
# Generate a report comparing hash functions
make report
```

This command compiles and runs the report generator that evaluates SDBM, DJB2, and FNV1a hash functions on a test dataset. The report is saved to `2105009_Report.txt`.

## Project Structure

- `2105009_SymbolInfo.*`: Symbol information class
- `2105009_ScopeTable.*`: Hash table implementation for a single scope
- `2105009_SymbolTable.*`: Main symbol table class that manages multiple scopes
- `2105009_hash.*`: Hash function implementations
- `2105009_utils.*`: Utility functions for parsing and formatting
- `2105009_main.cpp`: Main program entry point
- `2105009_ReportGeneration.cpp`: Tool for hash function evaluation

## Input Format

The input file should have the following format:
- First line: number of buckets in the hash table
- Subsequent lines: commands with their parameters
  - `I <name> <type>` - Insert a symbol
  - `L <name>` - Look up a symbol
  - `D <name>` - Delete a symbol
  - `P A/C` - Print all scopes or current scope
  - `S` - Enter a new scope
  - `E` - Exit current scope
  - `Q` - Quit the program