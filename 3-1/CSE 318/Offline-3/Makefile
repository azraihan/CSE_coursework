# Makefile for Chain Reaction Game

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SRCDIR = game-logic
UIFILE = ui/game_ui.py
GAMEINFODIR = game-info
EXPERIMENTDIR = experiments
EXECUTABLE = game-logic/ai_engine

# Source files
SOURCES = $(SRCDIR)/game.cpp $(SRCDIR)/minimax.cpp $(SRCDIR)/heuristics.cpp $(SRCDIR)/ai_engine.cpp
HEADERS = $(SRCDIR)/game.h $(SRCDIR)/minimax.h $(SRCDIR)/heuristics.h
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: setup compile

# Create necessary directories
setup:
	@echo "Creating project directories..."
	@mkdir -p $(SRCDIR)
	@mkdir -p ui
	@mkdir -p $(GAMEINFODIR)
	@mkdir -p $(EXPERIMENTDIR)
	@echo "Project structure created successfully!"

# Compile the AI engine
compile: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo "Linking AI engine..."
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "AI engine compiled successfully!"

# Compile object files
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Kill any existing AI processes
kill-ai:
	@echo "Killing any existing AI processes..."
	@pkill -f "ai_engine" 2>/dev/null || true
	@sleep 1

# Run Human vs AI game (default UI mode)
run-ui: kill-ai
	@echo "Starting Human vs AI game..."
	@echo "HUMAN_VS_AI" > $(GAMEINFODIR)/game_mode.txt
	@cd ui && python3 game_ui.py

# Run Human vs AI game
run-human-ai: compile kill-ai
	@echo "Starting Human vs AI game..."
	@echo "HUMAN_VS_AI" > $(GAMEINFODIR)/game_mode.txt
	@cd ui && python3 game_ui.py &
	@sleep 2
	@cd $(SRCDIR) && ./ai_engine BLUE
	@pkill -f "game_ui.py" 2>/dev/null || true

# Run AI vs AI game
run-ai-vs-ai: compile kill-ai
	@echo "Starting AI vs AI game..."
	@echo "AI_VS_AI" > $(GAMEINFODIR)/game_mode.txt
	@cd ui && python3 game_ui.py &
	@sleep 2
	@cd $(SRCDIR) && ./ai_engine RED &
	@sleep 2
	@cd $(SRCDIR) && ./ai_engine BLUE
	@pkill -f "game_ui.py" 2>/dev/null || true
	@pkill -f "ai_engine" 2>/dev/null || true

# Run the Blue AI engine only
run-blue-ai: compile kill-ai
	@echo "Starting Blue AI engine..."
	@cd $(SRCDIR) && ./ai_engine BLUE

# Run the Red AI engine only
run-red-ai: compile kill-ai
	@echo "Starting Red AI engine..."
	@cd $(SRCDIR) && ./ai_engine RED

# Run AI vs AI without UI (for experiments)
run-both-ai: compile kill-ai
	@echo "Starting AI vs AI without UI..."
	@echo "AI_VS_AI" > $(GAMEINFODIR)/game_mode.txt
	@cd $(SRCDIR) && ./ai_engine RED &
	@sleep 1
	@cd $(SRCDIR) && ./ai_engine BLUE
	@pkill -f "ai_engine" 2>/dev/null || true

# Quick test - single AI vs AI game
quick-test: compile kill-ai
	@echo "Running quick AI vs AI test..."
	@echo "AI_VS_AI" > $(GAMEINFODIR)/game_mode.txt
	@echo "Human Move:" > $(GAMEINFODIR)/gamestate.txt
	@for i in {1..9}; do \
		echo "0 0 0 0 0 0" >> $(GAMEINFODIR)/gamestate.txt; \
	done
	@cd $(SRCDIR) && timeout 30 ./ai_engine RED &
	@cd $(SRCDIR) && timeout 30 ./ai_engine BLUE &
	@sleep 25
	@pkill -f "ai_engine" 2>/dev/null || true
	@echo "Quick test completed!"

# Run experiments
run-experiments: compile kill-ai
	@echo "Running heuristic comparison experiments..."
	@echo "AI_VS_AI" > $(GAMEINFODIR)/game_mode.txt
	@cd $(EXPERIMENTDIR) && python3 experiment_runner.py heuristic

run-depth-experiments: compile kill-ai
	@echo "Running depth analysis experiments..."
	@echo "AI_VS_AI" > $(GAMEINFODIR)/game_mode.txt
	@cd $(EXPERIMENTDIR) && python3 experiment_runner.py depth

run-random-experiments: compile kill-ai
	@echo "Running AI vs random experiments..."
	@echo "AI_VS_AI" > $(GAMEINFODIR)/game_mode.txt
	@cd $(EXPERIMENTDIR) && python3 experiment_runner.py random

# Run all experiments in sequence
run-all-experiments: compile kill-ai
	@echo "Running all experiments..."
	@echo "AI_VS_AI" > $(GAMEINFODIR)/game_mode.txt
	@cd $(EXPERIMENTDIR) && python3 experiment_runner.py heuristic
	@cd $(EXPERIMENTDIR) && python3 experiment_runner.py depth
	@cd $(EXPERIMENTDIR) && python3 experiment_runner.py random
	@cd $(EXPERIMENTDIR) && python3 experiment_runner.py report
	@echo "All experiments completed!"

# Generate experiment report
experiment-report:
	@echo "Generating experiment report..."
	@cd $(EXPERIMENTDIR) && python3 experiment_runner.py report

# Performance test - run AI vs AI for analysis
perf-test: compile kill-ai
	@echo "Running performance test (AI vs AI games)..."
	@echo "AI_VS_AI" > $(GAMEINFODIR)/game_mode.txt
	@cd $(EXPERIMENTDIR) && python3 experiment_runner.py heuristic

# Clean compiled files
clean:
	@echo "Cleaning compiled files..."
	@rm -f $(OBJECTS) $(EXECUTABLE)
	@echo "Clean completed!"

# Clean experiment data
clean-experiments:
	@echo "Cleaning experiment data..."
	@rm -f $(EXPERIMENTDIR)/*.log $(EXPERIMENTDIR)/*.json $(EXPERIMENTDIR)/*.csv
	@echo "Experiment data cleaned!"

# Install Python dependencies
install-deps:
	@echo "Installing Python dependencies..."
	@pip3 install -r requirements.txt
	@echo "Dependencies installed!"

# Test compilation without running
test-compile: compile
	@echo "Compilation test successful!"

# Create sample game state file
create-sample:
	@echo "Creating sample game state..."
	@echo "Human Move:" > $(GAMEINFODIR)/gamestate.txt
	@for i in {1..9}; do \
		echo "0 0 0 0 0 0" >> $(GAMEINFODIR)/gamestate.txt; \
	done
	@echo "Sample game state created!"

# Package for submission
package:
	@echo "Creating submission package..."
	@zip -r submission.zip game-logic/ ui/ $(GAMEINFODIR)/ Makefile requirements.txt README.md
	@echo "Package created: submission.zip"

# Monitor AI vs AI game in terminal
monitor-game:
	@echo "Monitoring game state..."
	@watch -n 1 'cat $(GAMEINFODIR)/gamestate.txt 2>/dev/null || echo "No game file found"'

# Quick analysis of game logs
analyze-logs:
	@echo "Analyzing game logs..."
	@find . -name "*.log" -exec tail -10 {} \; 2>/dev/null || echo "No log files found"

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: compile
	@echo "Debug build completed!"

# Help target
help:
	@echo "Chain Reaction Game Makefile"
	@echo "Available targets:"
	@echo ""
	@echo "Basic Commands:"
	@echo "  all               - Setup directories and compile (default)"
	@echo "  setup             - Create project directory structure"
	@echo "  compile           - Compile AI engine"
	@echo "  clean             - Remove compiled files"
	@echo "  install-deps      - Install Python dependencies"
	@echo "  kill-ai           - Kill any running AI processes"
	@echo ""
	@echo "Running Games:"
	@echo "  run-ui            - Run Human vs AI game (default UI)"
	@echo "  run-human-ai      - Run Human vs AI game with both UI and AI"
	@echo "  run-ai-vs-ai      - Run AI vs AI game with UI"
	@echo "  run-both-ai       - Run AI vs AI without UI"
	@echo "  quick-test        - Quick 30-second AI vs AI test"
	@echo "  monitor-game      - Monitor AI vs AI game in terminal"
	@echo ""
	@echo "Individual AI Engines:"
	@echo "  run-blue-ai       - Run only Blue AI engine"
	@echo "  run-red-ai        - Run only Red AI engine"
	@echo ""
	@echo "Usage Examples:"
	@echo "  ./ai_engine BLUE  - Run as Blue player"
	@echo "  ./ai_engine RED   - Run as Red player"
	@echo "  ./ai_engine       - Run as Blue player (default)"
	@echo ""
	@echo "Experiments:"
	@echo "  run-experiments   - Run heuristic comparison"
	@echo "  run-depth-experiments - Run depth analysis"
	@echo "  run-random-experiments - Run AI vs weak AI"
	@echo "  run-all-experiments - Run all experiments"
	@echo "  experiment-report - Generate experiment report"
	@echo "  analyze-logs      - Quick analysis of game logs"
	@echo "  clean-experiments - Clean experiment data"
	@echo ""
	@echo "Development:"
	@echo "  test-compile      - Test compilation without running"
	@echo "  debug             - Build with debug information"
	@echo "  perf-test         - Run performance tests"
	@echo "  create-sample     - Create sample game state file"
	@echo "  package           - Create submission package"
	@echo "  help              - Show this help message"
	@echo ""
	@echo "Game Modes:"
	@echo "  Human vs AI: Use run-ui or run-human-ai"
	@echo "  AI vs AI: Use run-ai-vs-ai or run-both-ai"

# Phony targets
.PHONY: all setup compile run-ui run-human-ai run-ai-vs-ai run-blue-ai run-red-ai run-both-ai clean clean-experiments install-deps test-compile create-sample package debug perf-test quick-test run-experiments run-depth-experiments run-random-experiments run-all-experiments experiment-report analyze-logs monitor-game help kill-ai