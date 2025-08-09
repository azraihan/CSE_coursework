#!/usr/bin/env python3
"""
Decision Tree Analysis Visualization Script
Generates comprehensive and aesthetic visualizations for decision tree performance analysis.

Usage: python visualize_results.py <iris_csv_path> <adult_csv_path>
"""

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import argparse
import sys
from pathlib import Path

# Set style for aesthetic plots
plt.style.use('seaborn-v0_8-darkgrid')
sns.set_palette("husl")

def load_and_validate_data(iris_path, adult_path):
    """Load and validate the CSV files."""
    try:
        iris_df = pd.read_csv(iris_path)
        adult_df = pd.read_csv(adult_path)
        
        # Validate expected columns
        required_cols = ['Criterion', 'MaxDepth', 'AvgAccuracy', 'NodeCount', 'TreeDepth']
        run_cols = [f'Run{i}' for i in range(1, 21)]
        all_required = required_cols + run_cols
        
        for df, name in [(iris_df, 'Iris'), (adult_df, 'Adult')]:
            missing_cols = set(all_required) - set(df.columns)
            if missing_cols:
                raise ValueError(f"{name} CSV missing columns: {missing_cols}")
        
        print(f"✅ Successfully loaded data:")
        print(f"   - Iris: {len(iris_df)} experiments")
        print(f"   - Adult: {len(adult_df)} experiments")
        
        return iris_df, adult_df
        
    except FileNotFoundError as e:
        print(f"❌ Error: Could not find file - {e}")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Error loading data: {e}")
        sys.exit(1)

def calculate_statistics(df):
    """Calculate additional statistics for each experiment."""
    run_columns = [f'Run{i}' for i in range(1, 21)]
    
    # Calculate standard deviation and confidence intervals
    df['StdDev'] = df[run_columns].std(axis=1)
    df['Min'] = df[run_columns].min(axis=1)
    df['Max'] = df[run_columns].max(axis=1)
    
    # 95% confidence interval (assuming normal distribution)
    df['CI_Lower'] = df['AvgAccuracy'] - 1.96 * (df['StdDev'] / np.sqrt(20))
    df['CI_Upper'] = df['AvgAccuracy'] + 1.96 * (df['StdDev'] / np.sqrt(20))
    
    return df

def create_accuracy_vs_depth_iris(iris_df, save_path='iris_accuracy_vs_depth.png'):
    """Create accuracy vs depth plot for Iris dataset."""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    criteria = ['IG', 'IGR', 'NWIG']
    colors = ['#FF0000', '#00AA00', '#0066FF']  # High contrast: Red, Green, Blue
    markers = ['o', 's', '^']
    
    for i, criterion in enumerate(criteria):
        subset = iris_df[iris_df['Criterion'] == criterion].sort_values('MaxDepth')
        ax.errorbar(subset['MaxDepth'], subset['AvgAccuracy'], 
                   yerr=[subset['AvgAccuracy'] - subset['CI_Lower'], 
                         subset['CI_Upper'] - subset['AvgAccuracy']],
                   label=criterion, color=colors[i], marker=markers[i], 
                   linewidth=3, markersize=10, capsize=6, capthick=2)
    
    ax.set_title('Iris Dataset - Accuracy vs Maximum Depth', fontsize=16, fontweight='bold')
    ax.set_xlabel('Maximum Depth', fontsize=14)
    ax.set_ylabel('Average Accuracy', fontsize=14)
    ax.legend(fontsize=12)
    ax.grid(True, alpha=0.3)
    ax.set_ylim(0, 1.05)
    ax.tick_params(axis='both', which='major', labelsize=12)
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"📊 Saved: {save_path}")
    return fig

def create_accuracy_vs_depth_adult(adult_df, save_path='adult_accuracy_vs_depth.png'):
    """Create accuracy vs depth plot for Adult dataset."""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    criteria = ['IG', 'IGR', 'NWIG']
    colors = ['#FF0000', '#00AA00', '#0066FF']  # High contrast: Red, Green, Blue
    markers = ['o', 's', '^']
    
    for i, criterion in enumerate(criteria):
        subset = adult_df[adult_df['Criterion'] == criterion].sort_values('MaxDepth')
        ax.errorbar(subset['MaxDepth'], subset['AvgAccuracy'], 
                   yerr=[subset['AvgAccuracy'] - subset['CI_Lower'], 
                         subset['CI_Upper'] - subset['AvgAccuracy']],
                   label=criterion, color=colors[i], marker=markers[i], 
                   linewidth=3, markersize=10, capsize=6, capthick=2)
    
    ax.set_title('Adult Dataset - Accuracy vs Maximum Depth', fontsize=16, fontweight='bold')
    ax.set_xlabel('Maximum Depth', fontsize=14)
    ax.set_ylabel('Average Accuracy', fontsize=14)
    ax.legend(fontsize=12)
    ax.grid(True, alpha=0.3)
    ax.set_ylim(0, 1.05)
    ax.tick_params(axis='both', which='major', labelsize=12)
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"📊 Saved: {save_path}")
    return fig

def create_nodes_vs_depth_iris(iris_df, save_path='iris_nodes_vs_depth.png'):
    """Create tree complexity (nodes) vs depth plot for Iris dataset."""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    criteria = ['IG', 'IGR', 'NWIG']
    colors = ['#FF0000', '#00AA00', '#0066FF']  # High contrast: Red, Green, Blue
    markers = ['o', 's', '^']
    
    for i, criterion in enumerate(criteria):
        subset = iris_df[iris_df['Criterion'] == criterion].sort_values('MaxDepth')
        ax.plot(subset['MaxDepth'], subset['NodeCount'], 
               label=criterion, color=colors[i], marker=markers[i], 
               linewidth=3, markersize=10)
    
    ax.set_title('Iris Dataset - Tree Size vs Maximum Depth', fontsize=16, fontweight='bold')
    ax.set_xlabel('Maximum Depth', fontsize=14)
    ax.set_ylabel('Number of Nodes', fontsize=14)
    ax.legend(fontsize=12)
    ax.grid(True, alpha=0.3)
    ax.tick_params(axis='both', which='major', labelsize=12)
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"📊 Saved: {save_path}")
    return fig

def create_nodes_vs_depth_adult(adult_df, save_path='adult_nodes_vs_depth.png'):
    """Create tree complexity (nodes) vs depth plot for Adult dataset."""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    criteria = ['IG', 'IGR', 'NWIG']
    colors = ['#FF0000', '#00AA00', '#0066FF']  # High contrast: Red, Green, Blue
    markers = ['o', 's', '^']
    
    for i, criterion in enumerate(criteria):
        subset = adult_df[adult_df['Criterion'] == criterion].sort_values('MaxDepth')
        ax.plot(subset['MaxDepth'], subset['NodeCount'], 
               label=criterion, color=colors[i], marker=markers[i], 
               linewidth=3, markersize=10)
    
    ax.set_title('Adult Dataset - Tree Size vs Maximum Depth', fontsize=16, fontweight='bold')
    ax.set_xlabel('Maximum Depth', fontsize=14)
    ax.set_ylabel('Number of Nodes', fontsize=14)
    ax.legend(fontsize=12)
    ax.grid(True, alpha=0.3)
    ax.tick_params(axis='both', which='major', labelsize=12)
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"📊 Saved: {save_path}")
    return fig

def create_heatmap_iris(iris_df, save_path='iris_performance_heatmap.png'):
    """Create heatmap showing performance across criteria and depths for Iris."""
    fig, ax = plt.subplots(figsize=(8, 6))
    
    criteria = ['IG', 'IGR', 'NWIG']
    depths = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    
    def create_heatmap_data(df):
        heatmap_data = np.zeros((len(criteria), len(depths)))
        for i, criterion in enumerate(criteria):
            for j, depth in enumerate(depths):
                subset = df[(df['Criterion'] == criterion) & (df['MaxDepth'] == depth)]
                if not subset.empty:
                    heatmap_data[i, j] = subset['AvgAccuracy'].iloc[0]
        return heatmap_data
    
    iris_heatmap = create_heatmap_data(iris_df)
    
    sns.heatmap(iris_heatmap, annot=True, fmt='.3f', cmap='YlOrRd', 
                xticklabels=depths, yticklabels=criteria, ax=ax,
                cbar_kws={'label': 'Accuracy'}, square=True, linewidths=0.5)
    ax.set_title('Iris Dataset - Performance Heatmap', fontsize=16, fontweight='bold')
    ax.set_xlabel('Maximum Depth', fontsize=14)
    ax.set_ylabel('Criterion', fontsize=14)
    ax.tick_params(axis='both', which='major', labelsize=12)
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"📊 Saved: {save_path}")
    return fig

def create_heatmap_adult(adult_df, save_path='adult_performance_heatmap.png'):
    """Create heatmap showing performance across criteria and depths for Adult."""
    fig, ax = plt.subplots(figsize=(8, 6))
    
    criteria = ['IG', 'IGR', 'NWIG']
    depths = [1, 2, 3, 4, 5]
    
    def create_heatmap_data(df):
        heatmap_data = np.zeros((len(criteria), len(depths)))
        for i, criterion in enumerate(criteria):
            for j, depth in enumerate(depths):
                subset = df[(df['Criterion'] == criterion) & (df['MaxDepth'] == depth)]
                if not subset.empty:
                    heatmap_data[i, j] = subset['AvgAccuracy'].iloc[0]
        return heatmap_data
    
    adult_heatmap = create_heatmap_data(adult_df)
    
    sns.heatmap(adult_heatmap, annot=True, fmt='.3f', cmap='YlOrRd', 
                xticklabels=depths, yticklabels=criteria, ax=ax,
                cbar_kws={'label': 'Accuracy'}, square=True, linewidths=0.5)
    ax.set_title('Adult Dataset - Performance Heatmap', fontsize=16, fontweight='bold')
    ax.set_xlabel('Maximum Depth', fontsize=14)
    ax.set_ylabel('Criterion', fontsize=14)
    ax.tick_params(axis='both', which='major', labelsize=12)
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"📊 Saved: {save_path}")
    return fig

def create_boxplot_iris(iris_df, save_path='iris_consistency_boxplot.png'):
    """Create box plot showing consistency across runs for Iris dataset."""
    fig, ax = plt.subplots(figsize=(12, 6))
    
    run_columns = [f'Run{i}' for i in range(1, 21)]
    
    def prepare_boxplot_data(df):
        data_for_plot = []
        labels = []
        for _, row in df.iterrows():
            values = [row[col] for col in run_columns if not pd.isna(row[col])]
            data_for_plot.append(values)
            labels.append(f"{row['Criterion']}-D{row['MaxDepth']}")
        return data_for_plot, labels
    
    iris_data, iris_labels = prepare_boxplot_data(iris_df)
    
    bp = ax.boxplot(iris_data, labels=iris_labels, patch_artist=True)
    ax.set_title('Iris Dataset - Accuracy Distribution Across 20 Runs', fontsize=16, fontweight='bold')
    ax.set_ylabel('Accuracy', fontsize=14)
    ax.tick_params(axis='x', rotation=45, labelsize=10)
    ax.tick_params(axis='y', labelsize=12)
    ax.grid(True, alpha=0.3)
    
    # Color the boxes with high contrast colors
    colors = ['#FF0000', '#00AA00', '#0066FF'] * 5  # Red, Green, Blue repeated
    for patch, color in zip(bp['boxes'], colors):
        patch.set_facecolor(color)
        patch.set_alpha(0.7)
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"📊 Saved: {save_path}")
    return fig

def create_boxplot_adult(adult_df, save_path='adult_consistency_boxplot.png'):
    """Create box plot showing consistency across runs for Adult dataset."""
    fig, ax = plt.subplots(figsize=(12, 6))
    
    run_columns = [f'Run{i}' for i in range(1, 21)]
    
    def prepare_boxplot_data(df):
        data_for_plot = []
        labels = []
        for _, row in df.iterrows():
            values = [row[col] for col in run_columns if not pd.isna(row[col])]
            data_for_plot.append(values)
            labels.append(f"{row['Criterion']}-D{row['MaxDepth']}")
        return data_for_plot, labels
    
    adult_data, adult_labels = prepare_boxplot_data(adult_df)
    
    bp = ax.boxplot(adult_data, labels=adult_labels, patch_artist=True)
    ax.set_title('Adult Dataset - Accuracy Distribution Across 20 Runs', fontsize=16, fontweight='bold')
    ax.set_ylabel('Accuracy', fontsize=14)
    ax.tick_params(axis='x', rotation=45, labelsize=10)
    ax.tick_params(axis='y', labelsize=12)
    ax.grid(True, alpha=0.3)
    
    # Color the boxes with high contrast colors
    colors = ['#FF0000', '#00AA00', '#0066FF'] * 5  # Red, Green, Blue repeated
    for patch, color in zip(bp['boxes'], colors):
        patch.set_facecolor(color)
        patch.set_alpha(0.7)
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"📊 Saved: {save_path}")
    return fig

def create_stddev_comparison(iris_df, adult_df, save_path='stddev_comparison.png'):
    """Create standard deviation comparison plot."""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    criteria = ['IG', 'IGR', 'NWIG']
    x_pos = np.arange(len(criteria))
    
    iris_std_by_criterion = [iris_df[iris_df['Criterion'] == c]['StdDev'].mean() for c in criteria]
    adult_std_by_criterion = [adult_df[adult_df['Criterion'] == c]['StdDev'].mean() for c in criteria]
    
    width = 0.35
    ax.bar(x_pos - width/2, iris_std_by_criterion, width, label='Iris', alpha=0.8, color='#FF6600')
    ax.bar(x_pos + width/2, adult_std_by_criterion, width, label='Adult', alpha=0.8, color='#9900FF')
    ax.set_title('Average Standard Deviation by Criterion', fontsize=16, fontweight='bold')
    ax.set_ylabel('Standard Deviation', fontsize=14)
    ax.set_xlabel('Criterion', fontsize=14)
    ax.set_xticks(x_pos)
    ax.set_xticklabels(criteria)
    ax.legend(fontsize=12)
    ax.grid(True, alpha=0.3)
    ax.tick_params(axis='both', which='major', labelsize=12)
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"📊 Saved: {save_path}")
    return fig

def create_performance_vs_complexity(iris_df, adult_df, save_path='performance_vs_complexity.png'):
    """Create performance vs complexity scatter plot."""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    # Create scatter plot with different markers for each dataset
    iris_scatter = ax.scatter(iris_df['NodeCount'], iris_df['AvgAccuracy'], 
                             c=iris_df['MaxDepth'], cmap='viridis', alpha=0.8, s=120, 
                             label='Iris', marker='o', edgecolors='black')
    adult_scatter = ax.scatter(adult_df['NodeCount'], adult_df['AvgAccuracy'], 
                              c=adult_df['MaxDepth'], cmap='plasma', alpha=0.8, s=120, 
                              marker='^', label='Adult', edgecolors='black')
    
    ax.set_title('Performance vs Tree Complexity', fontsize=16, fontweight='bold')
    ax.set_xlabel('Number of Nodes', fontsize=14)
    ax.set_ylabel('Average Accuracy', fontsize=14)
    ax.legend(fontsize=12)
    ax.grid(True, alpha=0.3)
    ax.tick_params(axis='both', which='major', labelsize=12)
    
    # Add colorbar
    cbar = plt.colorbar(iris_scatter)
    cbar.set_label('Maximum Depth', fontsize=12)
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"📊 Saved: {save_path}")
    return fig

def create_summary_table(iris_df, adult_df, save_path='summary_statistics.png'):
    """Create a summary statistics table."""
    fig, ax = plt.subplots(figsize=(14, 8))
    ax.axis('tight')
    ax.axis('off')
    
    def get_best_performance(df, dataset_name):
        best_overall = df.loc[df['AvgAccuracy'].idxmax()]
        best_by_criterion = {}
        for criterion in ['IG', 'IGR', 'NWIG']:
            subset = df[df['Criterion'] == criterion]
            if not subset.empty:
                best = subset.loc[subset['AvgAccuracy'].idxmax()]
                best_by_criterion[criterion] = {
                    'depth': best['MaxDepth'],
                    'accuracy': best['AvgAccuracy'],
                    'nodes': best['NodeCount'],
                    'std': best['StdDev']
                }
        
        return best_overall, best_by_criterion
    
    iris_best, iris_by_criterion = get_best_performance(iris_df, 'Iris')
    adult_best, adult_by_criterion = get_best_performance(adult_df, 'Adult')
    
    # Create summary table
    table_data = []
    table_data.append(['Dataset', 'Criterion', 'Best Depth', 'Accuracy', 'Std Dev', 'Nodes'])
    table_data.append(['', '', '', '', '', ''])
    
    # Iris results
    for criterion in ['IG', 'IGR', 'NWIG']:
        if criterion in iris_by_criterion:
            data = iris_by_criterion[criterion]
            table_data.append([
                'Iris' if criterion == 'IG' else '',
                criterion,
                f"{data['depth']}",
                f"{data['accuracy']:.4f}",
                f"{data['std']:.4f}",
                f"{data['nodes']}"
            ])
    
    table_data.append(['', '', '', '', '', ''])
    
    # Adult results
    for criterion in ['IG', 'IGR', 'NWIG']:
        if criterion in adult_by_criterion:
            data = adult_by_criterion[criterion]
            table_data.append([
                'Adult' if criterion == 'IG' else '',
                criterion,
                f"{data['depth']}",
                f"{data['accuracy']:.4f}",
                f"{data['std']:.4f}",
                f"{data['nodes']}"
            ])
    
    table = ax.table(cellText=table_data[2:], colLabels=table_data[0],
                    cellLoc='center', loc='center', bbox=[0, 0, 1, 1])
    table.auto_set_font_size(False)
    table.set_fontsize(11)
    table.scale(1, 2)
    
    # Style the table
    for i in range(len(table_data[0])):
        table[(0, i)].set_facecolor('#333333')  # Dark header
        table[(0, i)].set_text_props(weight='bold', color='white')
    
    ax.set_title('Best Performance Summary by Criterion', 
                fontsize=16, fontweight='bold', pad=20)
    
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"📊 Saved: {save_path}")
    return fig

def generate_insights_report(iris_df, adult_df, save_path='insights_report.txt'):
    """Generate a text report with key insights."""
    with open(save_path, 'w') as f:
        f.write("DECISION TREE ANALYSIS - KEY INSIGHTS REPORT\n")
        f.write("=" * 50 + "\n\n")
        
        # Overall best performers
        iris_best = iris_df.loc[iris_df['AvgAccuracy'].idxmax()]
        adult_best = adult_df.loc[adult_df['AvgAccuracy'].idxmax()]
        
        f.write("1. BEST OVERALL PERFORMANCE:\n")
        f.write(f"   Iris:  {iris_best['Criterion']} at depth {iris_best['MaxDepth']} - {iris_best['AvgAccuracy']:.4f} accuracy\n")
        f.write(f"   Adult: {adult_best['Criterion']} at depth {adult_best['MaxDepth']} - {adult_best['AvgAccuracy']:.4f} accuracy\n\n")
        
        # Consistency analysis
        f.write("2. CONSISTENCY ANALYSIS (Average Std Dev):\n")
        for criterion in ['IG', 'IGR', 'NWIG']:
            iris_std = iris_df[iris_df['Criterion'] == criterion]['StdDev'].mean()
            adult_std = adult_df[adult_df['Criterion'] == criterion]['StdDev'].mean()
            f.write(f"   {criterion}: Iris={iris_std:.4f}, Adult={adult_std:.4f}\n")
        f.write("\n")
        
        # Overfitting analysis
        f.write("3. OVERFITTING PATTERNS:\n")
        for dataset_name, df in [('Iris', iris_df), ('Adult', adult_df)]:
            f.write(f"   {dataset_name} Dataset:\n")
            for criterion in ['IG', 'IGR', 'NWIG']:
                subset = df[df['Criterion'] == criterion].sort_values('MaxDepth')
                if len(subset) >= 2:
                    peak_acc = subset['AvgAccuracy'].max()
                    final_acc = subset['AvgAccuracy'].iloc[-1]
                    if peak_acc > final_acc:
                        f.write(f"     {criterion}: Shows overfitting (peak: {peak_acc:.4f}, final: {final_acc:.4f})\n")
                    else:
                        f.write(f"     {criterion}: No clear overfitting pattern\n")
            f.write("\n")
        
        # Tree complexity vs performance
        f.write("4. TREE COMPLEXITY INSIGHTS:\n")
        for dataset_name, df in [('Iris', iris_df), ('Adult', adult_df)]:
            correlation = df['NodeCount'].corr(df['AvgAccuracy'])
            f.write(f"   {dataset_name}: Correlation between nodes and accuracy = {correlation:.4f}\n")
        f.write("\n")
        
        f.write("5. RECOMMENDATIONS:\n")
        f.write("   - Compare criteria performance across different depths\n")
        f.write("   - Consider the trade-off between accuracy and model complexity\n")
        f.write("   - Pay attention to consistency (lower std dev = more reliable)\n")
        f.write("   - Watch for overfitting patterns in deeper trees\n")
    
    print(f"📝 Saved: {save_path}")

def main():
    """Main function to generate all visualizations."""
    parser = argparse.ArgumentParser(description='Generate comprehensive decision tree visualizations')
    parser.add_argument('iris_csv', help='Path to iris results CSV file')
    parser.add_argument('adult_csv', help='Path to adult results CSV file')
    parser.add_argument('--output-dir', default='.', help='Output directory for plots (default: current directory)')
    
    args = parser.parse_args()
    
    # Validate file paths
    iris_path = Path(args.iris_csv)
    adult_path = Path(args.adult_csv)
    output_dir = Path(args.output_dir)
    
    if not iris_path.exists():
        print(f"❌ Error: Iris CSV file not found: {iris_path}")
        sys.exit(1)
    
    if not adult_path.exists():
        print(f"❌ Error: Adult CSV file not found: {adult_path}")
        sys.exit(1)
    
    output_dir.mkdir(exist_ok=True)
    
    print("🚀 Starting Decision Tree Visualization Analysis...")
    print(f"📁 Output directory: {output_dir}")
    
    # Load and process data
    iris_df, adult_df = load_and_validate_data(iris_path, adult_path)
    iris_df = calculate_statistics(iris_df)
    adult_df = calculate_statistics(adult_df)
    
    # Generate all visualizations
    print("\n📊 Generating visualizations...")
    
    # Accuracy vs Depth plots (separate for each dataset)
    create_accuracy_vs_depth_iris(iris_df, output_dir / 'iris_accuracy_vs_depth.png')
    create_accuracy_vs_depth_adult(adult_df, output_dir / 'adult_accuracy_vs_depth.png')
    
    # Tree complexity plots (separate for each dataset)
    create_nodes_vs_depth_iris(iris_df, output_dir / 'iris_nodes_vs_depth.png')
    create_nodes_vs_depth_adult(adult_df, output_dir / 'adult_nodes_vs_depth.png')
    
    # Performance heatmaps (separate for each dataset)
    create_heatmap_iris(iris_df, output_dir / 'iris_performance_heatmap.png')
    create_heatmap_adult(adult_df, output_dir / 'adult_performance_heatmap.png')
    
    # Consistency analysis plots (separate for each dataset)
    create_boxplot_iris(iris_df, output_dir / 'iris_consistency_boxplot.png')
    create_boxplot_adult(adult_df, output_dir / 'adult_consistency_boxplot.png')
    
    # Comparison plots
    create_stddev_comparison(iris_df, adult_df, output_dir / 'stddev_comparison.png')
    create_performance_vs_complexity(iris_df, adult_df, output_dir / 'performance_vs_complexity.png')
    
    # Summary table and insights
    create_summary_table(iris_df, adult_df, output_dir / 'summary_statistics.png')
    generate_insights_report(iris_df, adult_df, output_dir / 'insights_report.txt')
    
    print(f"\n✅ All visualizations completed!")
    print(f"📁 Check the {output_dir} directory for all generated files.")
    print("\n📋 Generated files:")
    print("   - iris_accuracy_vs_depth.png")
    print("   - adult_accuracy_vs_depth.png")
    print("   - iris_nodes_vs_depth.png")
    print("   - adult_nodes_vs_depth.png")
    print("   - iris_performance_heatmap.png")
    print("   - adult_performance_heatmap.png")
    print("   - iris_consistency_boxplot.png")
    print("   - adult_consistency_boxplot.png")
    print("   - stddev_comparison.png")
    print("   - performance_vs_complexity.png")
    print("   - summary_statistics.png")
    print("   - insights_report.txt")

if __name__ == "__main__":
    main()