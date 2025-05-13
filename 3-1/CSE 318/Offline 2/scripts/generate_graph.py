import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import argparse
import os
import re
import math
from matplotlib.patches import PathPatch
from matplotlib.path import Path

def create_max_cut_graphs(csv_path, output_dir="results/graphs", max_graphs_per_image=6):
    """
    Create multiple bar charts comparing algorithm performance on MAX-CUT problem,
    with each image containing at most max_graphs_per_image instances
    
    Parameters:
    -----------
    csv_path : str
        Path to the CSV file containing algorithm results
    output_dir : str
        Directory to save the generated graphs
    max_graphs_per_image : int
        Maximum number of graph instances to include in each image
    """
    # Use a clean style with white background
    plt.style.use('default')
    
    # Override any remaining styling to ensure clean look
    plt.rcParams['figure.facecolor'] = 'white'
    plt.rcParams['axes.facecolor'] = 'white'
    plt.rcParams['savefig.facecolor'] = 'white'
    plt.rcParams['legend.frameon'] = False
    plt.rcParams['axes.grid'] = False
    
    # Read the CSV file - manually parse to handle the merged column format
    data = []
    
    with open(csv_path, 'r') as f:
        lines = f.readlines()
    
    # Skip header rows, directly extract the data
    for line in lines:
        if line.startswith('g'):  # Line starts with graph name like g1, g2, etc.
            parts = line.strip().split(',')
            if len(parts) >= 12:  # Ensure line has enough elements
                try:
                    # Convert 'N/A' to None for the benchmark value
                    known_best = None
                    if len(parts) > 12 and parts[12] and parts[12].strip() != 'N/A':
                        known_best = float(parts[12])
                    
                    record = {
                        'Name': parts[0],
                        'Vertices': int(parts[1]) if parts[1] else 0,
                        'Edges': int(parts[2]) if parts[2] else 0,
                        'Randomized': float(parts[3]) if parts[3] else 0,
                        'Greedy': float(parts[4]) if parts[4] else 0,
                        'SemiGreedy': float(parts[5]) if parts[5] else 0,
                        'LocalSearch': float(parts[7]) if parts[7] and parts[7] != '' else 0,
                        'GRASP': float(parts[10]) if parts[10] and parts[10] != '' else 0,
                        'KnownBest': known_best
                    }
                    data.append(record)
                except (ValueError, IndexError) as e:
                    print(f"Error parsing line: {line.strip()}")
                    print(f"Error details: {e}")
    
    if not data:
        print("No valid data found in the CSV file")
        return
    
    # Create dataframe from the parsed data
    df = pd.DataFrame(data)
    
    # Sort by graph number
    df['graph_num'] = df['Name'].apply(lambda x: int(re.search(r'(\d+)', x).group(1)) if re.search(r'(\d+)', x) else float('inf'))
    df = df.sort_values('graph_num')
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Calculate how many images we need
    total_graphs = len(df)
    num_images = math.ceil(total_graphs / max_graphs_per_image)
    
    print(f"Total graphs: {total_graphs}")
    
    # Define improved colors with better contrast
    colors = {
        'Randomized': '#4E79A7',  # Blue
        'Greedy': '#F28E2B',      # Orange
        'Semi-Greedy': '#59A14F', # Green
        'Local Search': '#E15759', # Red
        'GRASP': '#B07AA1',       # Purple
        'Benchmark': '#000000'    # Black for benchmark
    }
    
    # Generate separate images
    for img_idx in range(num_images):
        start_idx = img_idx * max_graphs_per_image
        end_idx = min((img_idx + 1) * max_graphs_per_image, total_graphs)
        
        # Get subset of data for this image
        subset = df.iloc[start_idx:end_idx]
        
        # Extract data for plotting - convert to NumPy arrays for easier indexing
        graph_names = subset['Name'].str.lower().values
        randomized = subset['Randomized'].values
        greedy = subset['Greedy'].values
        semi_greedy = subset['SemiGreedy'].values
        local_search = subset['LocalSearch'].values
        grasp = subset['GRASP'].values
        
        # Get benchmark values for this subset
        benchmark_data = []
        for value in subset['KnownBest']:
            benchmark_data.append(value if pd.notnull(value) else 0)
        
        # Convert to numpy array
        benchmark_data = np.array(benchmark_data)
        
        # Set up the plot with improved aesthetics
        fig, ax = plt.subplots(figsize=(16, 10))
        fig.set_facecolor('white')  # Pure white background
        ax.set_facecolor('white')   # Pure white background for plot area
        
        # Set the width of the bars
        bar_width = 0.14
        
        # Set the positions of the bars on the x-axis
        r1 = np.arange(len(graph_names))
        r2 = [x + bar_width for x in r1]
        r3 = [x + bar_width for x in r2]
        r4 = [x + bar_width for x in r3]
        r5 = [x + bar_width for x in r4]
        r6 = [x + bar_width for x in r5]  # Position for benchmark values
        
        # Create the bars with enhanced styling
        randomized_bars = ax.bar(r1, randomized, width=bar_width, label='Randomized', 
                       color=colors['Randomized'], edgecolor='black', linewidth=0.5, alpha=0.9)
        greedy_bars = ax.bar(r2, greedy, width=bar_width, label='Greedy', 
                   color=colors['Greedy'], edgecolor='black', linewidth=0.5, alpha=0.9)
        semi_greedy_bars = ax.bar(r3, semi_greedy, width=bar_width, label='Semi-Greedy', 
                       color=colors['Semi-Greedy'], edgecolor='black', linewidth=0.5, alpha=0.9)
        local_search_bars = ax.bar(r4, local_search, width=bar_width, label='Local Search', 
                        color=colors['Local Search'], edgecolor='black', linewidth=0.5, alpha=0.9)
        grasp_bars = ax.bar(r5, grasp, width=bar_width, label='GRASP', 
                  color=colors['GRASP'], edgecolor='black', linewidth=0.5, alpha=0.9)
        
        # Add benchmark values with a distinctive crosshatch pattern
        has_benchmark = any(value != 0 for value in benchmark_data)
        if has_benchmark:
            benchmark_bars = ax.bar(r6, benchmark_data, width=bar_width, label='Benchmark', 
                         color='white', edgecolor='black', linewidth=1.5, hatch='xxx')
        
        # Calculate value threshold for displaying labels (10% of max value or 500, whichever is larger)
        all_values = np.concatenate([randomized, greedy, semi_greedy, local_search, grasp])
        if len(all_values) > 0:
            value_threshold = max(np.max(all_values) * 0.1, 500)
        else:
            value_threshold = 500
        
        # Add value labels on bars (horizontal)
        for i in range(len(graph_names)):
            # Only label GRASP values if they're above the threshold
            if grasp[i] > value_threshold:
                ax.text(r5[i], grasp[i] + 100, f"{int(grasp[i])}", 
                        ha='center', va='bottom', fontsize=8, fontweight='bold')
            
            # Only label benchmark values if they exist and are above zero
            if has_benchmark and benchmark_data[i] > 0:
                ax.text(r6[i], benchmark_data[i] + 100, f"{int(benchmark_data[i])}", 
                        ha='center', va='bottom', fontsize=8, fontweight='bold')
        
        # Improve the design: Add labels and title with enhanced styling
        ax.set_xlabel('Graph Instance', fontweight='bold', fontsize=12)
        ax.set_ylabel('Cut Weight', fontweight='bold', fontsize=12)
        start_graph = subset['Name'].iloc[0]
        end_graph = subset['Name'].iloc[-1]
        
        # Add a title
        title = f'Max Cut Performance Comparison ({start_graph.upper()}-{end_graph.upper()})'
        ax.set_title(title, fontweight='bold', fontsize=16, pad=20, color='#333333')
        
        # Add xticks on the middle of the group bars with HORIZONTAL labels
        ax.set_xticks([r + bar_width*2.5 for r in range(len(graph_names))])
        ax.set_xticklabels(graph_names, rotation=0, ha='center', fontweight='bold')
        
        # Remove grid lines completely for cleaner look
        ax.grid(False)
        
        # Frame the plot for a more professional look
        for spine in ax.spines.values():
            spine.set_visible(True)
            spine.set_color('#CCCCCC')
        
        # Set y-axis to start from 0
        ax.set_ylim(bottom=0)
        
        # Create a minimal legend with no frame
        legend = ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.12), 
                 ncol=6, frameon=False, fontsize=10)
                 
        # Make sure we override the seaborn style
        plt.rcParams['legend.frameon'] = False
        
        # Adjust layout to make room for labels
        plt.tight_layout(rect=[0, 0, 1, 0.93])  # Leave space for title
        
        # Save the plot with higher quality
        output_path = os.path.join(output_dir, f"max_cut_part{img_idx+1}.jpg")
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        plt.close()  # Close the figure to free memory
    
    # Create an overall summary graph with all data
    create_summary_graph(df, output_dir)
    
    print(f"All graphs saved to {output_dir} directory")

def create_summary_graph(df, output_dir):
    """
    Create a summary graph showing algorithm performance for all graphs
    """
    # Use a clean style with white background
    plt.style.use('default')
    
    # Override any remaining styling to ensure clean look
    plt.rcParams['figure.facecolor'] = 'white'
    plt.rcParams['axes.facecolor'] = 'white'
    plt.rcParams['savefig.facecolor'] = 'white'
    plt.rcParams['legend.frameon'] = False
    plt.rcParams['axes.grid'] = False
    
    # Calculate average performance ratio compared to best known solution
    df_with_known = df.dropna(subset=['KnownBest'])
    
    if len(df_with_known) == 0:
        print("No graphs with benchmark values found for summary graph")
        return
    
    # Calculate performance ratios for each algorithm
    performance = {
        'Randomized': (df_with_known['Randomized'] / df_with_known['KnownBest']).mean() * 100,
        'Greedy': (df_with_known['Greedy'] / df_with_known['KnownBest']).mean() * 100,
        'Semi-Greedy': (df_with_known['SemiGreedy'] / df_with_known['KnownBest']).mean() * 100,
        'Local Search': (df_with_known['LocalSearch'] / df_with_known['KnownBest']).mean() * 100,
        'GRASP': (df_with_known['GRASP'] / df_with_known['KnownBest']).mean() * 100,
    }
    
    # Define enhanced colors for consistency
    colors = {
        'Randomized': '#4E79A7',  # Blue
        'Greedy': '#F28E2B',      # Orange
        'Semi-Greedy': '#59A14F', # Green
        'Local Search': '#E15759', # Red
        'GRASP': '#B07AA1',       # Purple
    }
    
    # Create the summary plot with a clean, minimal aesthetic
    fig, ax = plt.subplots(figsize=(12, 8))
    
    # Remove seaborn grid styling
    plt.rcParams['axes.grid'] = False
    
    fig.set_facecolor('white')  # Pure white background
    ax.set_facecolor('white')   # Pure white background for plot area
    
    algorithms = list(performance.keys())
    values = list(performance.values())
    
    # Get the color list in the same order as algorithms
    color_list = [colors[algo] for algo in algorithms]
    
    # Create bars with enhanced styling
    bars = ax.bar(algorithms, values, color=color_list, edgecolor='black', linewidth=0.8, alpha=0.9)
    
    # Add pattern to the best performing algorithm's bar
    best_idx = values.index(max(values))
    bars[best_idx].set_hatch('\\\\')
    
    # Add labels and title with enhanced styling
    ax.set_xlabel('Algorithm', fontweight='bold', fontsize=12)
    ax.set_ylabel('Average Performance (% of Known Best)', fontweight='bold', fontsize=12)
    
    # Add a title with a subtle background
    title = 'Algorithm Performance Summary'
    ax.set_title(title, fontweight='bold', fontsize=18, pad=20)
    
    # Add clean value labels on top of each bar without boxes
    for i, (bar, v) in enumerate(zip(bars, values)):
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height + 0.5,
                f"{v:.1f}%",
                ha='center', va='bottom', fontsize=11, fontweight='bold')
    
    # Set y-axis to start from 0 and add some padding at the top
    max_value = max(values) * 1.15
    ax.set_ylim(bottom=0, top=max_value)
    
    # Remove grid lines
    ax.grid(False)
    
    # nice
    for spine in ax.spines.values():
        spine.set_visible(True)
        spine.set_color('#CCCCCC')
    
    # First explicitly set_ticks
    ax.set_xticks(np.arange(len(algorithms)))
    ax.set_xticklabels(algorithms, fontweight='bold')
    
    # Add additional information
    num_graphs = len(df_with_known)
    fig.text(0.5, 0.01, 
             f"Based on {num_graphs} graphs with known best solutions",
             ha='center', fontsize=10, style='normal')
    
    # Adjust layout
    plt.tight_layout(rect=[0, 0.03, 1, 0.93])
    
    # Save the plot with higher quality
    output_path = os.path.join(output_dir, "performance_summary.jpg")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()
    
    print(f"Summary graph saved to {output_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Generate MAX-CUT performance graphs from CSV data')
    parser.add_argument('csv_path', type=str, help='Path to the CSV file with algorithm results')
    parser.add_argument('--output_dir', type=str, default="results/graphs", help='Directory to save the output graphs')
    parser.add_argument('--max_per_image', type=int, default=6, help='Maximum number of graphs per image')
    
    args = parser.parse_args()
    
    if not os.path.exists(args.csv_path):
        print(f"Error: CSV file not found at {args.csv_path}")
    else:
        create_max_cut_graphs(args.csv_path, args.output_dir, args.max_per_image)