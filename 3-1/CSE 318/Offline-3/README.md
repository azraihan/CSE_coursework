# Chain Reaction AI Game

A strategic board game implementation with AI agents using Minimax algorithm and various heuristics. Players place orbs that explode when they reach critical mass, creating chain reactions to capture opponent cells.

## Game Rules

- **Grid**: 9×6 board with different critical masses:
  - **Corners**: 2 orbs to explode
  - **Edges**: 3 orbs to explode  
  - **Center**: 4 orbs to explode
- **Objective**: Eliminate all opponent orbs from the board
- **Mechanics**: When cells reach critical mass, they explode and distribute orbs to neighboring cells, potentially causing chain reactions

## Project Structure

```
Offline-3/
├── game-logic/          # Core game engine
│   ├── ai_engine.cpp    # AI player implementation
│   ├── game.cpp/.h      # Game state and rules
│   ├── minimax.cpp/.h   # Minimax algorithm with alpha-beta pruning
│   └── heuristics.cpp/.h # 7 different evaluation heuristics
├── ui/                  # Python-based GUI
│   └── game_ui.py       # Pygame interface
├── experiments/         # AI performance analysis
│   ├── run_experiment.sh # Automated experiment runner
│   ├── generate_graphs.py # Results visualization
│   └── results/         # CSV data and analysis
├── game-info/           # Game state files
└── Makefile            # Build and run automation
```

## Quick Start

### Prerequisites
- **C++17** compiler (g++)
- **Python 3** with pygame, matplotlib, pandas, numpy
- **Make** utility

### Installation
```bash
# Install Python dependencies
pip3 install -r requirements.txt

# Build the project
make all
```

### Running Games

#### Human vs AI
```bash
make run-ui                    # GUI mode
make run-human-ai             # With both UI and AI engine
```

#### AI vs AI
```bash
make run-ai-vs-ai             # With UI visualization
make run-both-ai              # Headless mode
make quick-test               # 30-second test game
```

#### Individual AI Engines
```bash
make run-red-ai               # Red player only
make run-blue-ai              # Blue player only

# Or directly with custom parameters:
./game-logic/ai_engine [PLAYER] [DEPTH] [HEURISTIC] [TIME_LIMIT]
# Example: ./game-logic/ai_engine BLUE 3 5 5000
```

## AI Configuration

### Heuristics (1-7)
1. **Simple Orb Difference** - Basic orb count comparison
2. **Positional Advantage** - Weighted by cell importance
3. **Critical Cell Count** - Focus on cells near explosion
4. **Threat Analysis** - Advanced tactical evaluation
5. **Combined Strategy** - Balanced multi-factor approach
6. **Advanced Strategy** - Territory control and connectivity
7. **Random** - For experimental baseline

### Parameters
- **Depth**: Search depth (1-6, capped at 4 for performance)
- **Time Limit**: Maximum thinking time in milliseconds
- **Player**: RED or BLUE

## Experiments & Analysis

### Run Experiments
```bash
make run-experiments          # Heuristic comparison
make run-depth-experiments    # Depth analysis  
make run-random-experiments   # AI vs random player
make run-all-experiments      # Complete test suite
```

### Generate Analysis
```bash
# Create visualizations from results
cd experiments
python3 generate_graphs.py heuristic_comparison.csv random_vs_ai.csv
```

### Key Findings
- **Best Heuristic**: H5 (Combined Strategy) shows optimal balance
- **Optimal Depth**: 3-4 for best performance vs time trade-off
- **Performance**: All heuristics significantly outperform random play
- **Complexity**: Higher depths don't always improve performance due to time constraints

## Architecture Highlights

### Game Engine
- **Efficient explosion handling** with queue-based chain reaction processing
- **Robust state management** with file-based communication
- **Flexible AI interface** supporting different configurations

### AI Implementation
- **Minimax with alpha-beta pruning** for optimal play
- **Iterative deepening** for time management
- **Move ordering** for better pruning efficiency
- **Quiescence search** for tactical positions

### Experimental Framework
- **Automated testing** across multiple configurations
- **Statistical analysis** with 20 runs per configuration
- **Comprehensive visualization** of results
- **Performance profiling** and optimization

## Development

### Building
```bash
make compile                  # Build only
make clean                    # Remove compiled files
make debug                    # Debug build
make test-compile            # Test compilation
```

### Monitoring
```bash
make monitor-game            # Watch game state in terminal
make analyze-logs            # Quick log analysis
make kill-ai                 # Stop all AI processes
```

### Packaging
```bash
make package                 # Create submission zip
```

## Performance Notes

- **Optimized for speed**: Reduced branching factor and depth limits
- **Time management**: Configurable time limits with graceful degradation
- **Memory efficient**: Minimal state copying and smart pruning
- **Scalable**: Handles both quick games and deep analysis

## Troubleshooting

### Common Issues
- **AI timeout**: Reduce depth or increase time limit
- **File conflicts**: Use `make kill-ai` to clean processes
- **UI not responding**: Check Python dependencies installation
- **Compilation errors**: Ensure C++17 support and proper includes

### Debug Logs
- Check `experiments/logs/` for detailed AI decision logs
- Game state files in `game-info/` for state inspection
- Use `make monitor-game` for real-time state watching