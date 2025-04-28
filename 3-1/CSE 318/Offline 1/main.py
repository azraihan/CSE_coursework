import argparse
from typing import Dict, List
from board import Board
from heuristics import Heuristic, HammingDistance, ManhattanDistance, EuclideanDistance, LinearConflict
from a_star import AStar
from node import Node
from utils import is_solvable, read_board_from_input, print_solution

def main():
    # parse arguments
    parser = argparse.ArgumentParser(description='Solve N-puzzle using A* search algorithm')
    parser.add_argument('--heuristic', choices=['hamming', 'manhattan', 'euclidean', 'linear-conflict'],
                        default='manhattan', help='Heuristic function to use (default: manhattan)')
    args = parser.parse_args()
    
    # Map heuristic name
    heuristic_map: Dict[str, Heuristic] = {
        'hamming': HammingDistance,
        'manhattan': ManhattanDistance,
        'euclidean': EuclideanDistance,
        'linear-conflict': LinearConflict
    }
    
    heuristic = heuristic_map[args.heuristic]
    
    # Read the board configuration from input
    size, tiles = read_board_from_input()
    initial_board: Board = Board(size, tiles)
    
    # is puzzle solvable
    if not is_solvable(initial_board):
        print("Unsolvable puzzle")
        return
    
    # Solve the puzzle
    a_star: AStar = AStar(initial_board, heuristic)
    solution_path: List[Node] = a_star.solve()
    
    # Print the solution
    print_solution(solution_path)
    
    # Print statistics
    nodes_explored, nodes_expanded = a_star.get_statistics()
    print(f"Number of nodes explored: {nodes_explored}")
    print(f"Number of nodes expanded: {nodes_expanded}")

if __name__ == "__main__":
    main()