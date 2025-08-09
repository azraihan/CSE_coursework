# Decision Tree Implementation

A comprehensive implementation of decision tree algorithms with multiple splitting criteria, supporting both categorical and numerical features. Includes experimental analysis on Iris and Adult datasets with automated testing and visualization.

## Features

### Splitting Criteria
- **IG (Information Gain)** - Classic entropy-based splitting
- **IGR (Information Gain Ratio)** - Normalized by intrinsic value
- **NWIG (Normalized Weighted Information Gain)** - Custom criterion with penalty factors

### Capabilities
- **Mixed data types**: Automatic detection of numerical vs categorical features
- **Missing value handling**: Robust preprocessing for incomplete data
- **Pruning support**: Configurable maximum depth limits
- **Performance analysis**: Statistical evaluation across multiple runs

## Project Structure

```
Offline-4/
├── 2105009.cpp              # Main implementation
├── Datasets/                # Training data
│   ├── Iris.csv            # Iris flower classification
│   └── adult.data          # Census income prediction
├── results/                 # Experimental results
│   ├── iris_results.csv    # Iris performance data
│   └── adult_results.csv   # Adult performance data
├── visualize_results.py     # Analysis and visualization
└── README.md               # This file
```

## Quick Start

### Prerequisites
- **C++17** compiler (g++ recommended)
- **Python 3** with pandas, matplotlib, seaborn, numpy (for visualization)

### Compilation
```bash
g++ -std=c++17 -O2 2105009.cpp -o decision_tree
```

### Basic Usage
```bash
# Run with specific criterion and depth
./decision_tree <criterion> <maxDepth>

# Examples:
./decision_tree IG 0        # Information Gain, no pruning
./decision_tree IGR 5       # Information Gain Ratio, max depth 5
./decision_tree NWIG 3      # NWIG criterion, max depth 3
```

### Parameters
- **criterion**: `IG`, `IGR`, or `NWIG`
- **maxDepth**: `0` for no pruning, or positive integer for depth limit

## Datasets

### Iris Dataset
- **Features**: 4 numerical (sepal/petal length & width)
- **Classes**: 3 iris species (setosa, versicolor, virginica)
- **Size**: 150 samples
- **Challenge**: Multi-class classification with continuous features

### Adult Dataset  
- **Features**: 14 mixed (age, workclass, education, etc.)
- **Classes**: 2 income levels (≤50K, >50K)
- **Size**: ~32,000 samples
- **Challenge**: Large-scale binary classification with missing values

## Experimental Framework

### Automated Testing
Each configuration runs **20 independent trials** with:
- Random 80/20 train/test splits
- Consistent preprocessing pipeline
- Statistical analysis of results

### Output Metrics
- **Average Accuracy**: Mean performance across 20 runs
- **Standard Deviation**: Consistency measure
- **Tree Complexity**: Node count and depth
- **Individual Run Results**: Complete performance breakdown

### Results Analysis

#### Run All Experiments
```bash
# Test all criteria and depths
for criterion in IG IGR NWIG; do
  for depth in {1..10}; do
    ./decision_tree $criterion $depth
  done
done
```

#### Visualization & Analysis
```bash
# Generate comprehensive analysis plots
python3 visualize_results.py results/iris_results.csv results/adult_results.csv

# Output files:
# - *_accuracy_vs_depth.png      # Performance trends
# - *_performance_heatmap.png     # Criterion comparison
# - *_consistency_boxplot.png     # Variance analysis
# - *_nodes_vs_depth.png          # Complexity analysis
# - performance_vs_complexity.png # Trade-off analysis
# - summary_statistics.png        # Best configurations
# - insights_report.txt           # Key findings
```

## Algorithm Details

### Tree Construction
1. **Feature Type Detection**: Automatic numerical/categorical classification
2. **Missing Value Handling**: Smart replacement strategies
3. **Split Selection**: Optimal threshold finding for numerical features
4. **Recursive Building**: Depth-first construction with pruning

### Splitting Strategies

#### Numerical Features
- **Midpoint Strategy**: Splits between consecutive unique values
- **Binary Splits**: ≤ threshold vs > threshold
- **Optimal Selection**: Exhaustive search for best information gain

#### Categorical Features
- **Multi-way Splits**: One branch per unique value
- **Direct Comparison**: No threshold needed
- **Missing Value Handling**: Special empty category

### Optimization Features
- **Performance Monitoring**: Timing and statistics tracking
- **Memory Efficient**: Minimal copying during tree building
- **Debug Logging**: Comprehensive decision trail recording

## Key Findings

### Performance Insights
- **IGR Generally Superior**: Better handling of bias toward multi-valued attributes
- **Depth Sweet Spot**: Typically 3-5 for optimal accuracy vs complexity
- **Dataset Dependency**: Optimal configurations vary by data characteristics
- **Overfitting Patterns**: Visible in deeper trees on Adult dataset

### Complexity Analysis
- **Iris**: Simple trees (5-20 nodes) achieve high accuracy
- **Adult**: Larger trees (hundreds to thousands of nodes) needed
- **Trade-offs**: More nodes don't always mean better performance

## Implementation Highlights

### Robust Design
- **Type Safety**: Automatic feature type detection
- **Error Handling**: Graceful missing value management
- **Flexible Interface**: Configurable criteria and depth limits
- **Comprehensive Logging**: Detailed debug information

### Performance Optimizations
- **Efficient Sorting**: O(n log n) split finding
- **Memory Management**: Smart object lifecycle
- **Statistical Tracking**: Built-in performance monitoring
- **Scalable Architecture**: Handles large datasets effectively

## Troubleshooting

### Common Issues
- **Compilation Errors**: Ensure C++17 support (`-std=c++17`)
- **Dataset Not Found**: Check file paths in `./Datasets/`
- **Memory Issues**: Large datasets may require system memory
- **Python Dependencies**: Install required packages for visualization

### Debug Information
- **Debug Files**: `iris_debug.txt` and `adult_debug.txt` created during execution
- **Verbose Logging**: Complete decision trail and statistics
- **Performance Timing**: Individual operation timing data

### Expected Runtime
- **Iris**: ~1-2 seconds per configuration
- **Adult**: ~10-30 seconds per configuration (varies by depth)