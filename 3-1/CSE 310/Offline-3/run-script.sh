#antlr4 -v 4.13.2 -Dlanguage=Cpp C2105009Lexer.g4
#antlr4 -v 4.13.2 -Dlanguage=Cpp C2105009Parser.g4

java -Xmx500M -cp "/usr/local/lib/antlr-4.13.2-complete.jar:$CLASSPATH" org.antlr.v4.Tool -Dlanguage=Cpp C2105009Lexer.g4
java -Xmx500M -cp "/usr/local/lib/antlr-4.13.2-complete.jar:$CLASSPATH" org.antlr.v4.Tool -Dlanguage=Cpp C2105009Parser.g4

# g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -c C2105009Lexer.cpp C2105009Parser.cpp Ctester.cpp
# g++ -std=c++17 -w C2105009Lexer.o C2105009Parser.o Ctester.o -L/usr/local/lib/ -lantlr4-runtime -o Ctester.out -pthread

# Compile all source files (including symbol table files)
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -ISymbolTable -c \
    C2105009Lexer.cpp \
    C2105009Parser.cpp \
    Ctester.cpp
    
# Compile symbol table files (output to SymbolTable directory)
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -ISymbolTable -c SymbolTable/2105009_SymbolInfo.cpp -o SymbolTable/2105009_SymbolInfo.o
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -ISymbolTable -c SymbolTable/2105009_ScopeTable.cpp -o SymbolTable/2105009_ScopeTable.o
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -ISymbolTable -c SymbolTable/2105009_SymbolTable.cpp -o SymbolTable/2105009_SymbolTable.o
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime -ISymbolTable -c SymbolTable/2105009_hash.cpp -o SymbolTable/2105009_hash.o

# Link all object files
g++ -std=c++17 -w \
    C2105009Lexer.o \
    C2105009Parser.o \
    Ctester.o \
    SymbolTable/2105009_SymbolInfo.o \
    SymbolTable/2105009_ScopeTable.o \
    SymbolTable/2105009_SymbolTable.o \
    SymbolTable/2105009_hash.o \
    -L/usr/local/lib/ -lantlr4-runtime -o Ctester.out -pthread

LD_LIBRARY_PATH=/usr/local/lib ./Ctester.out $1
