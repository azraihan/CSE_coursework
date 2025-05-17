LEXICAL_ANALYZER = lexer
LEXER_FILE = 2105009.l
SRCS = $(SYMBOL_TABLE_MODULE)/2105009_SymbolTable.cpp\
		$(SYMBOL_TABLE_MODULE)/2105009_ScopeTable.cpp\
		$(SYMBOL_TABLE_MODULE)/2105009_SymbolInfo.cpp\
		$(SYMBOL_TABLE_MODULE)/2105009_hash.cpp

SYMBOL_TABLE_MODULE = SymbolTable

TEST_FILE_DIR = "Test io-20250515"
INPUT_FILE_1 = input1.txt
INPUT_FILE_2 = input2.txt
INPUT_FILE_3 = input3.txt

TOKEN_FILE = 2105009_*_token.txt
LOG_FILE = 2105009_*_log.txt

build-and-run-all: clean flex compile test

flex:
	flex $(LEXER_FILE)

compile:
	g++ -o $(LEXICAL_ANALYZER) lex.yy.c $(SRCS) -I.

test:
	./$(LEXICAL_ANALYZER) $(TEST_FILE_DIR)/$(INPUT_FILE_1)
	./$(LEXICAL_ANALYZER) $(TEST_FILE_DIR)/$(INPUT_FILE_2)
	./$(LEXICAL_ANALYZER) $(TEST_FILE_DIR)/$(INPUT_FILE_3)

clean:
	rm -f $(LEXICAL_ANALYZER) lex.yy.c $(TOKEN_FILE) $(LOG_FILE)

.PHONY: build-and-run flex compile test clean