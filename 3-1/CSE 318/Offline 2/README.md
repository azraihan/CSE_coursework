# Max-Cut Problem Solver

This repository contains implementations of several algorithms for solving the Maximum Cut (MAX-CUT) problem on graphs, including Randomized, Greedy, Semi-Greedy, Local Search, and GRASP (Greedy Randomized Adaptive Search Procedure).

## Requirements

- C++17 compatible compiler (e.g., g++ 9.0 or higher)
- Make

## Building the Project

To build the project, run:

```bash
make
```

This will compile the source code and create an executable named `max_cut`.

## Running the Algorithms

### Run All Algorithms on All Graphs

```bash
./max_cut --all
```

Optional parameters:
- `--n <value>`: Number of iterations for randomized algorithm (default: 10)
- `--alpha <value>`: Alpha parameter for semi-greedy and GRASP algorithms (default: 0.5)
- `--n_runs <value>`: Number of runs for local search algorithm (default: 10)
- `--max_iterations <value>`: Maximum iterations for GRASP algorithm (default: 10)

Example with custom parameters:
```bash
./max_cut --all --n 1000 --alpha 0.8 --n_runs 5 --max_iterations 50
```

### Run All Algorithms on a Specific Graph

```bash
./max_cut --graph <graph_name>
```

Example:
```bash
./max_cut --graph g12
```

### Run a Specific Algorithm on a Specific Graph

```bash
./max_cut --graph <graph_name> --algorithm <algorithm_name>
```

Available algorithms:
- `randomized`: Randomized algorithm
- `greedy`: Greedy algorithm
- `semi_greedy`: Semi-Greedy algorithm
- `local_search`: Local Search algorithm
- `grasp`: GRASP algorithm

Examples:

```bash
# Run randomized algorithm on graph g12 with 1000 iterations
./max_cut --graph g12 --algorithm randomized --n 1000

# Run greedy algorithm on graph g12
./max_cut --graph g12 --algorithm greedy

# Run semi-greedy algorithm on graph g12 with alpha=0.8
./max_cut --graph g12 --algorithm semi_greedy --alpha 0.8

# Run local search algorithm on graph g12 with 5 runs
./max_cut --graph g12 --algorithm local_search --n_runs 5

# Run GRASP algorithm on graph g12 with 50 max iterations and alpha=0.8
./max_cut --graph g12 --algorithm grasp --max_iterations 50 --alpha 0.8
```

## Results

Results are written to the `results` directory in CSV format. If the directory doesn't exist, it will be created automatically.

## Convenience Makefile Targets

The Makefile includes several targets for convenience:

```bash
# Run all algorithms on all graphs with predefined parameters
make run

# Run specific algorithms on the test graph defined in Makefile (default: g12)
make run_randomized
make run_greedy
make run_semi_greedy
make run_local_search
make run_grasp

# Run all algorithms on a specific graph
make run_all_on_graph TEST_GRAPH=g14

# Clean up object files and executable
make clean
```

You can modify the algorithm parameters in the Makefile before running these targets.