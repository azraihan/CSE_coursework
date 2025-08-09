#antlr4 -v 4.13.2 -Dlanguage=Cpp C8086Lexer.g4
#antlr4 -v 4.13.2 -Dlanguage=Cpp C8086Parser.g4

java -Xmx500M -cp "/usr/local/lib/antlr-4.13.2-complete.jar:$CLASSPATH" org.antlr.v4.Tool -Dlanguage=Cpp C8086Lexer.g4
java -Xmx500M -cp "/usr/local/lib/antlr-4.13.2-complete.jar:$CLASSPATH" org.antlr.v4.Tool -Dlanguage=Cpp C8086Parser.g4

# g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -c C8086Lexer.cpp C8086Parser.cpp Ctester.cpp
# g++ -std=c++17 -w C8086Lexer.o C8086Parser.o Ctester.o -L/usr/local/lib/ -lantlr4-runtime -o Ctester.out -pthread

# Compile all source files (including symbol table files)
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -ISymbolTable -c \
    C8086Lexer.cpp \
    C8086Parser.cpp \
    Ctester.cpp
    
# Compile symbol table files (output to SymbolTable directory)
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -ISymbolTable -c SymbolTable/2105009_SymbolInfo.cpp -o SymbolTable/2105009_SymbolInfo.o
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -ISymbolTable -c SymbolTable/2105009_ScopeTable.cpp -o SymbolTable/2105009_ScopeTable.o
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -ISymbolTable -c SymbolTable/2105009_SymbolTable.cpp -o SymbolTable/2105009_SymbolTable.o
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -ISymbolTable -c SymbolTable/2105009_hash.cpp -o SymbolTable/2105009_hash.o

# Compile optimizer
g++ -std=c++17 -w -c optimizer/peephole_optimizer.cpp -o optimizer/peephole_optimizer.o

# Link all object files
g++ -std=c++17 -w \
    C8086Lexer.o \
    C8086Parser.o \
    Ctester.o \
    SymbolTable/2105009_SymbolInfo.o \
    SymbolTable/2105009_ScopeTable.o \
    SymbolTable/2105009_SymbolTable.o \
    SymbolTable/2105009_hash.o \
    optimizer/peephole_optimizer.o \
    -L/usr/local/lib/ -lantlr4-runtime -o Ctester.out -pthread

LD_LIBRARY_PATH=/usr/local/lib ./Ctester.out $1
