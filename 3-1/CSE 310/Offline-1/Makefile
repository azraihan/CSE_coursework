# CC = g++
# CFLAGS = -fsanitize=address
# TARGET = symboltable
# SRCS = main.cpp hash.cpp SymbolInfo.cpp ScopeTable.cpp SymbolTable.cpp
# OBJS = $(SRCS:.cpp=.o)

# all: $(TARGET)

# $(TARGET): $(OBJS)
# 	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# %.o: %.cpp
# 	$(CC) $(CFLAGS) -c $< -o $@

# clean:
# 	rm -f $(OBJS) $(TARGET)

# run: $(TARGET)
# 	./$(TARGET) sample_input.txt output.txt

# .PHONY: all clean run

CC = g++
CFLAGS = -fsanitize=address
TARGET = symboltable
MAIN = 2105009_main.cpp
SRCS = 2105009_hash.cpp 2105009_SymbolInfo.cpp 2105009_ScopeTable.cpp 2105009_SymbolTable.cpp 2105009_utils.cpp
INPUT_FILE = sample_input.txt
OUTPUT_FILE = output.txt

REPORT_GENERATION_FILE = 2105009_ReportGeneration.cpp
REPORT_TARGET = report
TEST_FILE = testcases.txt
REPORT_FILE = 2105009_Report.txt
NUM_BUCKETS= 15

run-all: clean build run

build:
	$(CC) $(CFLAGS) $(MAIN) $(SRCS) -o $(TARGET)
	@echo "Build Done. To generate output use \"make run\""

clean:
	@rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET) $(INPUT_FILE) $(OUTPUT_FILE)

report:
	@rm -f $(REPORT_TARGET)
	$(CC) $(CFLAGS) $(REPORT_GENERATION_FILE) $(SRCS) -o $(REPORT_TARGET)
	./$(REPORT_TARGET) $(TEST_FILE) $(REPORT_FILE) $(NUM_BUCKETS)

.PHONY: build clean run run-all report