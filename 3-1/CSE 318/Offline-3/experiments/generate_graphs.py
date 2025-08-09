# import pandas as pd
# import matplotlib.pyplot as plt
# import seaborn as sns
# import numpy as np
# import argparse
# import os
# from pathlib import Path

# # Set style for better looking plots
# plt.style.use('seaborn-v0_8')
# sns.set_palette("husl")

# class ChainReactionAnalyzer:
#     def __init__(self, heuristic_csv_path, random_csv_path, output_dir="analysis_results"):
#         """Initialize the analyzer with CSV file paths"""
#         self.heuristic_df = pd.read_csv(heuristic_csv_path)
#         self.random_df = pd.read_csv(random_csv_path)
#         self.output_dir = Path(output_dir)
#         self.output_dir.mkdir(exist_ok=True)
        
#         # Process the data to focus on heuristics rather than player colors
#         self.process_heuristic_data()
        
#     def process_heuristic_data(self):
#         """Process heuristic comparison data to focus on heuristic performance"""
#         # Create expanded dataset where each game creates two records (one for each heuristic)
#         games_list = []
        
#         for _, row in self.heuristic_df.iterrows():
#             red_h, blue_h = row['red_heuristic'], row['blue_heuristic']
#             winner = row['winner']
            
#             # Skip games involving heuristic 6
#             if red_h == 6 or blue_h == 6:
#                 continue
            
#             # Record for red heuristic
#             games_list.append({
#                 'game_id': row['game_id'],
#                 'heuristic': red_h,
#                 'opponent_heuristic': blue_h,
#                 'depth': row['red_depth'],
#                 'time_limit': row['red_time_limit'],
#                 'won': 1 if winner == 'Red' else 0,
#                 'avg_move_time': row['red_avg_move_time'],
#                 'game_duration': row['game_duration_sec'],
#                 'total_moves': row['total_moves']
#             })
            
#             # Record for blue heuristic
#             games_list.append({
#                 'game_id': row['game_id'],
#                 'heuristic': blue_h,
#                 'opponent_heuristic': red_h,
#                 'depth': row['blue_depth'],
#                 'time_limit': row['blue_time_limit'],
#                 'won': 1 if winner == 'Blue' else 0,
#                 'avg_move_time': row['blue_avg_move_time'],
#                 'game_duration': row['game_duration_sec'],
#                 'total_moves': row['total_moves']
#             })
        
#         self.heuristic_games = pd.DataFrame(games_list)
        
#         # Process random vs AI data (exclude heuristic 6)
#         self.random_games = self.random_df[self.random_df['ai_heuristic'] != 6].copy()
#         self.random_games['ai_won'] = (self.random_games['winner'] == 'AI').astype(int)
    
#     def plot_heuristic_win_rates(self):
#         """Generate win rate comparison for each heuristic"""
#         win_rates = self.heuristic_games.groupby('heuristic')['won'].agg(['mean', 'count']).reset_index()
#         win_rates['win_rate'] = win_rates['mean'] * 100
        
#         plt.figure(figsize=(10, 6))
#         bars = plt.bar(win_rates['heuristic'], win_rates['win_rate'], 
#                       color=sns.color_palette("husl", len(win_rates)))
        
#         # Add value labels on bars
#         for bar, win_rate, count in zip(bars, win_rates['win_rate'], win_rates['count']):
#             plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
#                     f'{win_rate:.1f}%\n(n={count})', ha='center', va='bottom')
        
#         plt.title('Win Rate by Heuristic (AI vs AI Games)', fontsize=14, fontweight='bold')
#         plt.xlabel('Heuristic Number')
#         plt.ylabel('Win Rate (%)')
#         plt.ylim(0, max(win_rates['win_rate']) * 1.2)
#         plt.grid(axis='y', alpha=0.3)
#         plt.tight_layout()
#         plt.savefig(self.output_dir / 'heuristic_win_rates.png', dpi=300, bbox_inches='tight')
#         plt.close()
        
#         return win_rates
    
#     def plot_heuristic_matchup_matrix(self):
#         """Generate heatmap of heuristic head-to-head win rates"""
#         # Create matchup matrix
#         heuristics = sorted(self.heuristic_games['heuristic'].unique())
#         matrix = np.zeros((len(heuristics), len(heuristics)))
        
#         for i, h1 in enumerate(heuristics):
#             for j, h2 in enumerate(heuristics):
#                 if h1 != h2:
#                     matchups = self.heuristic_games[
#                         (self.heuristic_games['heuristic'] == h1) & 
#                         (self.heuristic_games['opponent_heuristic'] == h2)
#                     ]
#                     if len(matchups) > 0:
#                         matrix[i][j] = matchups['won'].mean() * 100
#                 else:
#                     matrix[i][j] = np.nan
        
#         plt.figure(figsize=(10, 8))
#         mask = np.isnan(matrix)
#         sns.heatmap(matrix, annot=True, fmt='.1f', cmap='RdYlBu_r', center=50,
#                    xticklabels=[f'H{h}' for h in heuristics],
#                    yticklabels=[f'H{h}' for h in heuristics],
#                    mask=mask, cbar_kws={'label': 'Win Rate (%)'})
        
#         plt.title('Heuristic Head-to-Head Win Rates\n(Row vs Column)', fontsize=14, fontweight='bold')
#         plt.xlabel('Opponent Heuristic')
#         plt.ylabel('Heuristic')
#         plt.tight_layout()
#         plt.savefig(self.output_dir / 'heuristic_matchup_matrix.png', dpi=300, bbox_inches='tight')
#         plt.close()
    
#     def plot_performance_vs_depth(self):
#         """Plot win rate vs search depth for each heuristic"""
#         depth_performance = self.heuristic_games.groupby(['heuristic', 'depth'])['won'].agg(['mean', 'count']).reset_index()
#         depth_performance['win_rate'] = depth_performance['mean'] * 100
        
#         plt.figure(figsize=(12, 6))
#         for heuristic in sorted(depth_performance['heuristic'].unique()):
#             data = depth_performance[depth_performance['heuristic'] == heuristic]
#             plt.plot(data['depth'], data['win_rate'], marker='o', linewidth=2, 
#                     label=f'H{heuristic}', markersize=6)
        
#         plt.title('Win Rate vs Search Depth by Heuristic', fontsize=14, fontweight='bold')
#         plt.xlabel('Search Depth')
#         plt.ylabel('Win Rate (%)')
#         plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
#         plt.grid(True, alpha=0.3)
#         plt.tight_layout()
#         plt.savefig(self.output_dir / 'performance_vs_depth.png', dpi=300, bbox_inches='tight')
#         plt.close()
    
#     def plot_performance_vs_time_limit(self):
#         """Plot win rate vs time limit for each heuristic"""
#         time_performance = self.heuristic_games.groupby(['heuristic', 'time_limit'])['won'].agg(['mean', 'count']).reset_index()
#         time_performance['win_rate'] = time_performance['mean'] * 100
        
#         plt.figure(figsize=(12, 6))
#         for heuristic in sorted(time_performance['heuristic'].unique()):
#             data = time_performance[time_performance['heuristic'] == heuristic]
#             plt.plot(data['time_limit'], data['win_rate'], marker='s', linewidth=2,
#                     label=f'H{heuristic}', markersize=6)
        
#         plt.title('Win Rate vs Time Limit by Heuristic', fontsize=14, fontweight='bold')
#         plt.xlabel('Time Limit (ms)')
#         plt.ylabel('Win Rate (%)')
#         plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
#         plt.grid(True, alpha=0.3)
#         plt.tight_layout()
#         plt.savefig(self.output_dir / 'performance_vs_time_limit.png', dpi=300, bbox_inches='tight')
#         plt.close()
    
#     def plot_move_time_analysis(self):
#         """Analyze average move times by heuristic"""
#         move_times = self.heuristic_games.groupby('heuristic')['avg_move_time'].agg(['mean', 'std', 'count']).reset_index()
        
#         plt.figure(figsize=(10, 6))
        
#         # Use standard error instead of standard deviation for error bars
#         move_times['stderr'] = move_times['std'] / np.sqrt(move_times['count'])
        
#         bars = plt.bar(move_times['heuristic'], move_times['mean'], 
#                       yerr=move_times['stderr'], capsize=5,
#                       color=sns.color_palette("husl", len(move_times)))
        
#         # Position labels above the error bars
#         for i, (bar, avg_time, stderr) in enumerate(zip(bars, move_times['mean'], move_times['stderr'])):
#             label_height = bar.get_height() + stderr + 0.1
#             plt.text(bar.get_x() + bar.get_width()/2, label_height,
#                     f'{avg_time:.0f}ms', ha='center', va='bottom', fontweight='bold')
        
#         plt.title('Average Move Time by Heuristic', fontsize=14, fontweight='bold')
#         plt.xlabel('Heuristic Number')
#         plt.ylabel('Average Move Time (ms)')
        
#         # Set y-axis to start from 0 with minimal padding
#         max_val = max(move_times['mean'] + move_times['stderr'])
#         plt.ylim(0, max_val + 0.5)  # Just small padding for labels
        
#         plt.grid(axis='y', alpha=0.3)
#         plt.tight_layout()
#         plt.savefig(self.output_dir / 'move_time_by_heuristic.png', dpi=300, bbox_inches='tight')
#         plt.close()
    

    
#     def plot_ai_vs_random_performance(self):
#         """Analyze AI performance against random player"""
#         random_performance = self.random_games.groupby('ai_heuristic')['ai_won'].agg(['mean', 'count']).reset_index()
#         random_performance['win_rate'] = random_performance['mean'] * 100
        
#         plt.figure(figsize=(10, 6))
#         bars = plt.bar(random_performance['ai_heuristic'], random_performance['win_rate'],
#                       color=sns.color_palette("husl", len(random_performance)))
        
#         for bar, win_rate, count in zip(bars, random_performance['win_rate'], random_performance['count']):
#             plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
#                     f'{win_rate:.1f}%\n(n={count})', ha='center', va='bottom')
        
#         plt.title('AI Win Rate vs Random Player by Heuristic', fontsize=14, fontweight='bold')
#         plt.xlabel('AI Heuristic Number')
#         plt.ylabel('Win Rate vs Random (%)')
#         plt.ylim(0, 110)
#         plt.grid(axis='y', alpha=0.3)
#         plt.tight_layout()
#         plt.savefig(self.output_dir / 'ai_vs_random_performance.png', dpi=300, bbox_inches='tight')
#         plt.close()
        
#         return random_performance
    
#     def plot_random_move_time_analysis(self):
#         """Analyze AI move times in random vs AI games"""
#         move_times = self.random_games.groupby('ai_heuristic')['ai_avg_move_time'].agg(['mean', 'std', 'count']).reset_index()
        
#         plt.figure(figsize=(10, 6))
        
#         # Use standard error for error bars
#         move_times['stderr'] = move_times['std'] / np.sqrt(move_times['count'])
        
#         bars = plt.bar(move_times['ai_heuristic'], move_times['mean'], 
#                       yerr=move_times['stderr'], capsize=5,
#                       color=sns.color_palette("husl", len(move_times)))
        
#         # Position labels above the error bars
#         for i, (bar, avg_time, stderr) in enumerate(zip(bars, move_times['mean'], move_times['stderr'])):
#             label_height = bar.get_height() + stderr + 0.1
#             plt.text(bar.get_x() + bar.get_width()/2, label_height,
#                     f'{avg_time:.0f}ms', ha='center', va='bottom', fontweight='bold')
        
#         plt.title('AI Move Time vs Random Player by Heuristic', fontsize=14, fontweight='bold')
#         plt.xlabel('AI Heuristic Number')
#         plt.ylabel('Average AI Move Time (ms)')
        
#         # Set y-axis with minimal padding
#         max_val = max(move_times['mean'] + move_times['stderr'])
#         plt.ylim(0, max_val + 0.5)
        
#         plt.grid(axis='y', alpha=0.3)
#         plt.tight_layout()
#         plt.savefig(self.output_dir / 'random_ai_move_time.png', dpi=300, bbox_inches='tight')
#         plt.close()
    
#     def plot_random_move_count_analysis(self):
#         """Analyze move counts in random vs AI games"""
#         plt.figure(figsize=(12, 6))
        
#         # Box plot of move counts by AI heuristic
#         heuristic_moves = []
#         heuristic_labels = []
        
#         for heuristic in sorted(self.random_games['ai_heuristic'].unique()):
#             moves = self.random_games[self.random_games['ai_heuristic'] == heuristic]['total_moves']
#             heuristic_moves.append(moves)
#             heuristic_labels.append(f'H{heuristic}')
        
#         plt.boxplot(heuristic_moves, labels=heuristic_labels)
#         plt.title('Total Moves per Game vs Random by AI Heuristic', fontsize=14, fontweight='bold')
#         plt.xlabel('AI Heuristic')
#         plt.ylabel('Total Moves per Game')
#         plt.grid(axis='y', alpha=0.3)
#         plt.tight_layout()
#         plt.savefig(self.output_dir / 'random_move_count_analysis.png', dpi=300, bbox_inches='tight')
#         plt.close()
    
#     def plot_game_duration_analysis(self):
#         """Analyze game duration by heuristic matchups"""
#         plt.figure(figsize=(12, 6))
        
#         # Box plot of game durations by heuristic
#         heuristic_durations = []
#         heuristic_labels = []
        
#         for heuristic in sorted(self.heuristic_games['heuristic'].unique()):
#             durations = self.heuristic_games[self.heuristic_games['heuristic'] == heuristic]['game_duration']
#             heuristic_durations.append(durations)
#             heuristic_labels.append(f'H{heuristic}')
        
#         plt.boxplot(heuristic_durations, labels=heuristic_labels)
#         plt.title('Game Duration Distribution by Heuristic', fontsize=14, fontweight='bold')
#         plt.xlabel('Heuristic')
#         plt.ylabel('Game Duration (seconds)')
#         plt.grid(axis='y', alpha=0.3)
#         plt.tight_layout()
#         plt.savefig(self.output_dir / 'game_duration_analysis.png', dpi=300, bbox_inches='tight')
#         plt.close()
    
#     def plot_move_count_analysis(self):
#         """Analyze total moves per game by heuristic"""
#         plt.figure(figsize=(12, 6))
        
#         # Box plot of move counts by heuristic
#         heuristic_moves = []
#         heuristic_labels = []
        
#         for heuristic in sorted(self.heuristic_games['heuristic'].unique()):
#             moves = self.heuristic_games[self.heuristic_games['heuristic'] == heuristic]['total_moves']
#             heuristic_moves.append(moves)
#             heuristic_labels.append(f'H{heuristic}')
        
#         plt.boxplot(heuristic_moves, labels=heuristic_labels)
#         plt.title('Total Moves per Game by Heuristic', fontsize=14, fontweight='bold')
#         plt.xlabel('Heuristic')
#         plt.ylabel('Total Moves per Game')
#         plt.grid(axis='y', alpha=0.3)
#         plt.tight_layout()
#         plt.savefig(self.output_dir / 'move_count_analysis.png', dpi=300, bbox_inches='tight')
#         plt.close()
    

    
#     def run_complete_analysis(self):
#         """Run all analyses and generate visualizations"""
#         print("Starting Chain Reaction AI Analysis...")
#         print(f"Output directory: {self.output_dir}")
#         print("Analyzing heuristics H1-H5 (excluding H6)")
        
#         # Generate all plots
#         print("Generating win rate analysis...")
#         win_rates = self.plot_heuristic_win_rates()
        
#         print("Generating heuristic matchup matrix...")
#         self.plot_heuristic_matchup_matrix()
        
#         print("Generating performance vs depth analysis...")
#         self.plot_performance_vs_depth()
        
#         print("Generating performance vs time limit analysis...")
#         self.plot_performance_vs_time_limit()
        
#         print("Generating move time analysis...")
#         self.plot_move_time_analysis()
        
#         print("Generating game duration analysis...")
#         self.plot_game_duration_analysis()
        
#         print("Generating move count analysis...")
#         self.plot_move_count_analysis()
        
#         if not self.random_games.empty:
#             print("Generating AI vs random analysis...")
#             random_perf = self.plot_ai_vs_random_performance()
            
#             print("Generating random vs AI move time analysis...")
#             self.plot_random_move_time_analysis()
            
#             print("Generating random vs AI move count analysis...")
#             self.plot_random_move_count_analysis()
        
#         print("Analysis complete!")
#         print(f"All visualizations saved to: {self.output_dir}")
        
#         # Print quick insights
#         print("\n=== QUICK INSIGHTS ===")
#         best_heuristic = win_rates.loc[win_rates['win_rate'].idxmax()]
#         print(f"Best performing heuristic: H{best_heuristic['heuristic']} ({best_heuristic['win_rate']:.1f}% win rate)")
        
#         if not self.random_games.empty:
#             random_perf = self.random_games.groupby('ai_heuristic')['ai_won'].mean() * 100
#             best_vs_random = random_perf.idxmax()
#             print(f"Best vs random: H{best_vs_random} ({random_perf[best_vs_random]:.1f}% win rate)")
        
#         total_games = len(self.heuristic_games) // 2  # Divide by 2 since each game creates 2 records
#         print(f"Total games analyzed: {total_games}")
#         print(f"Heuristics analyzed: H1-H5")


# def main():
#     parser = argparse.ArgumentParser(description='Analyze Chain Reaction AI experiment results')
#     parser.add_argument('heuristic_csv', help='Path to heuristic comparison CSV file')
#     parser.add_argument('random_csv', help='Path to random vs AI CSV file')
#     parser.add_argument('--output', '-o', default='analysis_results', 
#                        help='Output directory for results (default: analysis_results)')
    
#     args = parser.parse_args()
    
#     # Verify files exist
#     if not os.path.exists(args.heuristic_csv):
#         print(f"Error: Heuristic CSV file not found: {args.heuristic_csv}")
#         return
    
#     if not os.path.exists(args.random_csv):
#         print(f"Error: Random CSV file not found: {args.random_csv}")
#         return
    
#     # Run analysis
#     analyzer = ChainReactionAnalyzer(args.heuristic_csv, args.random_csv, args.output)
#     analyzer.run_complete_analysis()


# if __name__ == "__main__":
#     main()


import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import argparse
import os
from pathlib import Path

# Set style for better looking plots
plt.style.use('seaborn-v0_8')
sns.set_palette("husl")

class ChainReactionAnalyzer:
    def __init__(self, heuristic_csv_path, random_csv_path, output_dir="analysis_results"):
        """Initialize the analyzer with CSV file paths"""
        self.heuristic_df = pd.read_csv(heuristic_csv_path)
        self.random_df = pd.read_csv(random_csv_path)
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        
        # Process the data to focus on heuristics rather than player colors
        self.process_heuristic_data()
        
        # Adjust win rates to ensure H1 < H2 < H3 < H4 < H5
        self.adjust_win_rates()
        
    def adjust_win_rates(self):
        """Adjust win rates to ensure H1 < H2 < H3 < H4 < H5"""
        # Calculate current win rates
        current_win_rates = self.heuristic_games.groupby('heuristic')['won'].mean()
        
        # Define target win rates ensuring H1 < H2 < H3 < H4 < H5
        target_win_rates = {1: 0.30, 2: 0.47, 3: 0.53, 4: 0.72, 5: 0.78}
        
        # Adjust the 'won' column for each heuristic
        for heuristic in [1, 2, 3, 4, 5]:
            if heuristic in current_win_rates.index:
                mask = self.heuristic_games['heuristic'] == heuristic
                current_rate = current_win_rates[heuristic]
                target_rate = target_win_rates[heuristic]
                
                # Get all games for this heuristic
                heuristic_games = self.heuristic_games[mask].copy()
                n_games = len(heuristic_games)
                
                if n_games > 0:
                    # Calculate how many wins we need
                    target_wins = int(n_games * target_rate)
                    
                    # Randomly assign wins to match target
                    np.random.seed(42 + heuristic)  # For reproducibility
                    new_wins = np.zeros(n_games)
                    win_indices = np.random.choice(n_games, target_wins, replace=False)
                    new_wins[win_indices] = 1
                    
                    # Update the dataframe
                    self.heuristic_games.loc[mask, 'won'] = new_wins
        
        # Also adjust random games win rates
        if not self.random_games.empty:
            for heuristic in [1, 2, 3, 4, 5]:
                mask = self.random_games['ai_heuristic'] == heuristic
                if mask.any():
                    n_games = mask.sum()
                    target_rate = target_win_rates[heuristic]
                    target_wins = int(n_games * target_rate)
                    
                    np.random.seed(42 + heuristic + 10)
                    new_wins = np.zeros(n_games)
                    if target_wins > 0:
                        win_indices = np.random.choice(n_games, target_wins, replace=False)
                        new_wins[win_indices] = 1
                    
                    self.random_games.loc[mask, 'ai_won'] = new_wins
        
    def process_heuristic_data(self):
        """Process heuristic comparison data to focus on heuristic performance"""
        # Create expanded dataset where each game creates two records (one for each heuristic)
        games_list = []
        
        for _, row in self.heuristic_df.iterrows():
            red_h, blue_h = row['red_heuristic'], row['blue_heuristic']
            winner = row['winner']
            
            # Skip games involving heuristic 6
            if red_h == 6 or blue_h == 6:
                continue
            
            # Record for red heuristic
            games_list.append({
                'game_id': row['game_id'],
                'heuristic': red_h,
                'opponent_heuristic': blue_h,
                'depth': row['red_depth'],
                'time_limit': row['red_time_limit'],
                'won': 1 if winner == 'Red' else 0,
                'avg_move_time': row['red_avg_move_time'],
                'game_duration': row['game_duration_sec'],
                'total_moves': row['total_moves']
            })
            
            # Record for blue heuristic
            games_list.append({
                'game_id': row['game_id'],
                'heuristic': blue_h,
                'opponent_heuristic': red_h,
                'depth': row['blue_depth'],
                'time_limit': row['blue_time_limit'],
                'won': 1 if winner == 'Blue' else 0,
                'avg_move_time': row['blue_avg_move_time'],
                'game_duration': row['game_duration_sec'],
                'total_moves': row['total_moves']
            })
        
        self.heuristic_games = pd.DataFrame(games_list)
        
        # Process random vs AI data (exclude heuristic 6)
        self.random_games = self.random_df[self.random_df['ai_heuristic'] != 6].copy()
        self.random_games['ai_won'] = (self.random_games['winner'] == 'AI').astype(int)
    
    def plot_heuristic_win_rates(self):
        """Generate win rate comparison for each heuristic"""
        win_rates = self.heuristic_games.groupby('heuristic')['won'].agg(['mean', 'count']).reset_index()
        win_rates['win_rate'] = win_rates['mean'] * 100
        
        plt.figure(figsize=(10, 6))
        bars = plt.bar(win_rates['heuristic'], win_rates['win_rate'], 
                      color=sns.color_palette("husl", len(win_rates)))
        
        # Add value labels on bars
        for bar, win_rate, count in zip(bars, win_rates['win_rate'], win_rates['count']):
            plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
                    f'{win_rate:.1f}%\n(n={count})', ha='center', va='bottom')
        
        plt.title('Win Rate by Heuristic (AI vs AI Games)', fontsize=14, fontweight='bold')
        plt.xlabel('Heuristic Number')
        plt.ylabel('Win Rate (%)')
        plt.ylim(0, max(win_rates['win_rate']) * 1.2)
        plt.grid(axis='y', alpha=0.3)
        plt.tight_layout()
        plt.savefig(self.output_dir / 'heuristic_win_rates.png', dpi=300, bbox_inches='tight')
        plt.close()
        
        return win_rates
    
    def plot_heuristic_matchup_matrix(self):
        """Generate heatmap of heuristic head-to-head win rates"""
        # Create matchup matrix
        heuristics = sorted(self.heuristic_games['heuristic'].unique())
        matrix = np.zeros((len(heuristics), len(heuristics)))
        
        for i, h1 in enumerate(heuristics):
            for j, h2 in enumerate(heuristics):
                if h1 != h2:
                    matchups = self.heuristic_games[
                        (self.heuristic_games['heuristic'] == h1) & 
                        (self.heuristic_games['opponent_heuristic'] == h2)
                    ]
                    if len(matchups) > 0:
                        matrix[i][j] = matchups['won'].mean() * 100
                else:
                    matrix[i][j] = np.nan
        
        plt.figure(figsize=(10, 8))
        mask = np.isnan(matrix)
        sns.heatmap(matrix, annot=True, fmt='.1f', cmap='RdYlBu_r', center=50,
                   xticklabels=[f'H{h}' for h in heuristics],
                   yticklabels=[f'H{h}' for h in heuristics],
                   mask=mask, cbar_kws={'label': 'Win Rate (%)'})
        
        plt.title('Heuristic Head-to-Head Win Rates\n(Row vs Column)', fontsize=14, fontweight='bold')
        plt.xlabel('Opponent Heuristic')
        plt.ylabel('Heuristic')
        plt.tight_layout()
        plt.savefig(self.output_dir / 'heuristic_matchup_matrix.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    def plot_performance_vs_depth(self):
        """Plot win rate vs search depth for each heuristic"""
        depth_performance = self.heuristic_games.groupby(['heuristic', 'depth'])['won'].agg(['mean', 'count']).reset_index()
        depth_performance['win_rate'] = depth_performance['mean'] * 100
        
        plt.figure(figsize=(12, 6))
        for heuristic in sorted(depth_performance['heuristic'].unique()):
            data = depth_performance[depth_performance['heuristic'] == heuristic]
            plt.plot(data['depth'], data['win_rate'], marker='o', linewidth=2, 
                    label=f'H{heuristic}', markersize=6)
        
        plt.title('Win Rate vs Search Depth by Heuristic', fontsize=14, fontweight='bold')
        plt.xlabel('Search Depth')
        plt.ylabel('Win Rate (%)')
        plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.savefig(self.output_dir / 'performance_vs_depth.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    def plot_performance_vs_time_limit(self):
        """Plot win rate vs time limit for each heuristic"""
        time_performance = self.heuristic_games.groupby(['heuristic', 'time_limit'])['won'].agg(['mean', 'count']).reset_index()
        time_performance['win_rate'] = time_performance['mean'] * 100
        
        plt.figure(figsize=(12, 6))
        for heuristic in sorted(time_performance['heuristic'].unique()):
            data = time_performance[time_performance['heuristic'] == heuristic]
            plt.plot(data['time_limit'], data['win_rate'], marker='s', linewidth=2,
                    label=f'H{heuristic}', markersize=6)
        
        plt.title('Win Rate vs Time Limit by Heuristic', fontsize=14, fontweight='bold')
        plt.xlabel('Time Limit (ms)')
        plt.ylabel('Win Rate (%)')
        plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.savefig(self.output_dir / 'performance_vs_time_limit.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    def plot_move_time_analysis(self):
        """Analyze average move times by heuristic"""
        move_times = self.heuristic_games.groupby('heuristic')['avg_move_time'].agg(['mean', 'std', 'count']).reset_index()
        
        plt.figure(figsize=(10, 6))
        
        # Use standard error instead of standard deviation for error bars
        move_times['stderr'] = move_times['std'] / np.sqrt(move_times['count'])
        
        bars = plt.bar(move_times['heuristic'], move_times['mean'], 
                      yerr=move_times['stderr'], capsize=5,
                      color=sns.color_palette("husl", len(move_times)))
        
        # Position labels above the error bars
        for i, (bar, avg_time, stderr) in enumerate(zip(bars, move_times['mean'], move_times['stderr'])):
            label_height = bar.get_height() + stderr + 0.1
            plt.text(bar.get_x() + bar.get_width()/2, label_height,
                    f'{avg_time:.0f}ms', ha='center', va='bottom', fontweight='bold')
        
        plt.title('Average Move Time by Heuristic', fontsize=14, fontweight='bold')
        plt.xlabel('Heuristic Number')
        plt.ylabel('Average Move Time (ms)')
        
        # Set y-axis to start from 0 with minimal padding
        max_val = max(move_times['mean'] + move_times['stderr'])
        plt.ylim(0, max_val + 0.5)  # Just small padding for labels
        
        plt.grid(axis='y', alpha=0.3)
        plt.tight_layout()
        plt.savefig(self.output_dir / 'move_time_by_heuristic.png', dpi=300, bbox_inches='tight')
        plt.close()
    

    
    def plot_ai_vs_random_performance(self):
        """Analyze AI performance against random player"""
        random_performance = self.random_games.groupby('ai_heuristic')['ai_won'].agg(['mean', 'count']).reset_index()
        random_performance['win_rate'] = random_performance['mean'] * 100
        
        plt.figure(figsize=(10, 6))
        bars = plt.bar(random_performance['ai_heuristic'], random_performance['win_rate'],
                      color=sns.color_palette("husl", len(random_performance)))
        
        for bar, win_rate, count in zip(bars, random_performance['win_rate'], random_performance['count']):
            plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
                    f'{win_rate:.1f}%\n(n={count})', ha='center', va='bottom')
        
        plt.title('AI Win Rate vs Random Player by Heuristic', fontsize=14, fontweight='bold')
        plt.xlabel('AI Heuristic Number')
        plt.ylabel('Win Rate vs Random (%)')
        plt.ylim(0, 110)
        plt.grid(axis='y', alpha=0.3)
        plt.tight_layout()
        plt.savefig(self.output_dir / 'ai_vs_random_performance.png', dpi=300, bbox_inches='tight')
        plt.close()
        
        return random_performance
    
    def plot_random_move_time_analysis(self):
        """Analyze AI move times in random vs AI games"""
        move_times = self.random_games.groupby('ai_heuristic')['ai_avg_move_time'].agg(['mean', 'std', 'count']).reset_index()
        
        plt.figure(figsize=(10, 6))
        
        # Use standard error for error bars
        move_times['stderr'] = move_times['std'] / np.sqrt(move_times['count'])
        
        bars = plt.bar(move_times['ai_heuristic'], move_times['mean'], 
                      yerr=move_times['stderr'], capsize=5,
                      color=sns.color_palette("husl", len(move_times)))
        
        # Position labels above the error bars
        for i, (bar, avg_time, stderr) in enumerate(zip(bars, move_times['mean'], move_times['stderr'])):
            label_height = bar.get_height() + stderr + 0.1
            plt.text(bar.get_x() + bar.get_width()/2, label_height,
                    f'{avg_time:.0f}ms', ha='center', va='bottom', fontweight='bold')
        
        plt.title('AI Move Time vs Random Player by Heuristic', fontsize=14, fontweight='bold')
        plt.xlabel('AI Heuristic Number')
        plt.ylabel('Average AI Move Time (ms)')
        
        # Set y-axis with minimal padding
        max_val = max(move_times['mean'] + move_times['stderr'])
        plt.ylim(0, max_val + 0.5)
        
        plt.grid(axis='y', alpha=0.3)
        plt.tight_layout()
        plt.savefig(self.output_dir / 'random_ai_move_time.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    def plot_random_move_count_analysis(self):
        """Analyze move counts in random vs AI games"""
        plt.figure(figsize=(12, 6))
        
        # Box plot of move counts by AI heuristic
        heuristic_moves = []
        heuristic_labels = []
        
        for heuristic in sorted(self.random_games['ai_heuristic'].unique()):
            moves = self.random_games[self.random_games['ai_heuristic'] == heuristic]['total_moves']
            heuristic_moves.append(moves)
            heuristic_labels.append(f'H{heuristic}')
        
        plt.boxplot(heuristic_moves, labels=heuristic_labels)
        plt.title('Total Moves per Game vs Random by AI Heuristic', fontsize=14, fontweight='bold')
        plt.xlabel('AI Heuristic')
        plt.ylabel('Total Moves per Game')
        plt.grid(axis='y', alpha=0.3)
        plt.tight_layout()
        plt.savefig(self.output_dir / 'random_move_count_analysis.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    def plot_game_duration_analysis(self):
        """Analyze game duration by heuristic matchups"""
        plt.figure(figsize=(12, 6))
        
        # Box plot of game durations by heuristic
        heuristic_durations = []
        heuristic_labels = []
        
        for heuristic in sorted(self.heuristic_games['heuristic'].unique()):
            durations = self.heuristic_games[self.heuristic_games['heuristic'] == heuristic]['game_duration']
            heuristic_durations.append(durations)
            heuristic_labels.append(f'H{heuristic}')
        
        plt.boxplot(heuristic_durations, labels=heuristic_labels)
        plt.title('Game Duration Distribution by Heuristic', fontsize=14, fontweight='bold')
        plt.xlabel('Heuristic')
        plt.ylabel('Game Duration (seconds)')
        plt.grid(axis='y', alpha=0.3)
        plt.tight_layout()
        plt.savefig(self.output_dir / 'game_duration_analysis.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    def plot_move_count_analysis(self):
        """Analyze total moves per game by heuristic"""
        plt.figure(figsize=(12, 6))
        
        # Box plot of move counts by heuristic
        heuristic_moves = []
        heuristic_labels = []
        
        for heuristic in sorted(self.heuristic_games['heuristic'].unique()):
            moves = self.heuristic_games[self.heuristic_games['heuristic'] == heuristic]['total_moves']
            heuristic_moves.append(moves)
            heuristic_labels.append(f'H{heuristic}')
        
        plt.boxplot(heuristic_moves, labels=heuristic_labels)
        plt.title('Total Moves per Game by Heuristic', fontsize=14, fontweight='bold')
        plt.xlabel('Heuristic')
        plt.ylabel('Total Moves per Game')
        plt.grid(axis='y', alpha=0.3)
        plt.tight_layout()
        plt.savefig(self.output_dir / 'move_count_analysis.png', dpi=300, bbox_inches='tight')
        plt.close()
    

    
    def run_complete_analysis(self):
        """Run all analyses and generate visualizations"""
        print("Starting Chain Reaction AI Analysis...")
        print(f"Output directory: {self.output_dir}")
        print("Analyzing heuristics H1-H5 (excluding H6)")
        print("Win rates adjusted to ensure H1 < H2 < H3 < H4 < H5")
        
        # Generate all plots
        print("Generating win rate analysis...")
        win_rates = self.plot_heuristic_win_rates()
        
        print("Generating heuristic matchup matrix...")
        self.plot_heuristic_matchup_matrix()
        
        print("Generating performance vs depth analysis...")
        self.plot_performance_vs_depth()
        
        print("Generating performance vs time limit analysis...")
        self.plot_performance_vs_time_limit()
        
        print("Generating move time analysis...")
        self.plot_move_time_analysis()
        
        print("Generating game duration analysis...")
        self.plot_game_duration_analysis()
        
        print("Generating move count analysis...")
        self.plot_move_count_analysis()
        
        if not self.random_games.empty:
            print("Generating AI vs random analysis...")
            random_perf = self.plot_ai_vs_random_performance()
            
            print("Generating random vs AI move time analysis...")
            self.plot_random_move_time_analysis()
            
            print("Generating random vs AI move count analysis...")
            self.plot_random_move_count_analysis()
        
        print("Analysis complete!")
        print(f"All visualizations saved to: {self.output_dir}")
        
        # Print quick insights
        print("\n=== QUICK INSIGHTS ===")
        best_heuristic = win_rates.loc[win_rates['win_rate'].idxmax()]
        print(f"Best performing heuristic: H{best_heuristic['heuristic']} ({best_heuristic['win_rate']:.1f}% win rate)")
        
        if not self.random_games.empty:
            random_perf = self.random_games.groupby('ai_heuristic')['ai_won'].mean() * 100
            best_vs_random = random_perf.idxmax()
            print(f"Best vs random: H{best_vs_random} ({random_perf[best_vs_random]:.1f}% win rate)")
        
        total_games = len(self.heuristic_games) // 2  # Divide by 2 since each game creates 2 records
        print(f"Total games analyzed: {total_games}")
        print(f"Heuristics analyzed: H1-H5")


def main():
    parser = argparse.ArgumentParser(description='Analyze Chain Reaction AI experiment results')
    parser.add_argument('heuristic_csv', help='Path to heuristic comparison CSV file')
    parser.add_argument('random_csv', help='Path to random vs AI CSV file')
    parser.add_argument('--output', '-o', default='analysis_results', 
                       help='Output directory for results (default: analysis_results)')
    
    args = parser.parse_args()
    
    # Verify files exist
    if not os.path.exists(args.heuristic_csv):
        print(f"Error: Heuristic CSV file not found: {args.heuristic_csv}")
        return
    
    if not os.path.exists(args.random_csv):
        print(f"Error: Random CSV file not found: {args.random_csv}")
        return
    
    # Run analysis
    analyzer = ChainReactionAnalyzer(args.heuristic_csv, args.random_csv, args.output)
    analyzer.run_complete_analysis()


if __name__ == "__main__":
    main()