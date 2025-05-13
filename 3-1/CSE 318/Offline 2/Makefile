CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -std=c++17

# Algorithm parameters (can be modified as needed)
TEST_GRAPH = g12
RANDOMIZED_N = 1000
SEMI_GREEDY_ALPHA = 0.8
LOCAL_SEARCH_N_RUNS = 5
GRASP_MAX_ITERATIONS = 50
GRASP_ALPHA = 0.8

# Report parameters
RESULTS_DIR = results
RESULTS_CSV = $(RESULTS_DIR)/results.csv
GRAPH_OUTPUT_DIR = $(RESULTS_DIR)/graphs
GRAPH_SCRIPT = scripts/generate_graph.py

# Add filesystem library if needed (for GCC < 9)
# UNAME_S := $(shell uname -s)
# ifeq ($(UNAME_S),Linux)
#     GCC_VERSION := $(shell $(CXX) -dumpversion | cut -f1 -d.)
#     ifeq ($(shell test $(GCC_VERSION) -lt 9; echo $$?),0)
#         LDFLAGS += -lstdc++fs
#     endif
# endif


# Source files
SRCS = main.cpp graph.cpp randomized.cpp greedy.cpp semi_greedy.cpp local_search.cpp grasp.cpp utils.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Output executable
TARGET = max_cut

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program with all graphs
run: $(TARGET)
	./$(TARGET) --all --n $(RANDOMIZED_N) --alpha $(SEMI_GREEDY_ALPHA) --n_runs $(LOCAL_SEARCH_N_RUNS) --max_iterations $(GRASP_MAX_ITERATIONS)

# Run specific algorithms on a graph
run_randomized: $(TARGET)
	./$(TARGET) --graph $(TEST_GRAPH) --algorithm randomized --n $(RANDOMIZED_N)

run_greedy: $(TARGET)
	./$(TARGET) --graph $(TEST_GRAPH) --algorithm greedy

run_semi_greedy: $(TARGET)
	./$(TARGET) --graph $(TEST_GRAPH) --algorithm semi_greedy --alpha $(SEMI_GREEDY_ALPHA)

run_local_search: $(TARGET)
	./$(TARGET) --graph $(TEST_GRAPH) --algorithm local_search --n_runs $(LOCAL_SEARCH_N_RUNS)

run_grasp: $(TARGET)
	./$(TARGET) --graph $(TEST_GRAPH) --algorithm grasp --max_iterations $(GRASP_MAX_ITERATIONS) --alpha $(GRASP_ALPHA)

# Run all algorithms on a specific graph
run_all_on_graph: $(TARGET)
	./$(TARGET) --graph $(TEST_GRAPH) --n $(RANDOMIZED_N) --alpha $(SEMI_GREEDY_ALPHA) --n_runs $(LOCAL_SEARCH_N_RUNS) --max_iterations $(GRASP_MAX_ITERATIONS)


# Dependency tracking
main.o: main.cpp graph.h randomized.h greedy.h semi_greedy.h local_search.h grasp.h utils.h
graph.o: graph.cpp graph.h
randomized.o: randomized.cpp randomized.h graph.h
greedy.o: greedy.cpp greedy.h graph.h
semi_greedy.o: semi_greedy.cpp semi_greedy.h graph.h
local_search.o: local_search.cpp local_search.h graph.h
grasp.o: grasp.cpp grasp.h semi_greedy.h local_search.h
utils.o: utils.cpp utils.h graph.h

# Generate the report graph
report: $(GRAPH_SCRIPT) $(RESULTS_DIR)
	@echo "Generating performance graph from results..."
	python $(GRAPH_SCRIPT) $(RESULTS_CSV) --output_dir $(GRAPH_OUTPUT_DIR)
	@echo "Report graph generated at $(GRAPH_OUTPUT_DIR)"

# Generate full report (run algorithms + generate graph)
full_report: run report
	@echo "Full report generation complete"

.PHONY: all clean run run_randomized run_greedy run_semi_greedy run_local_search run_grasp run_all_on_graph report full_report