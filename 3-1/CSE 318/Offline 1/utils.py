from node import Node
from board import Board
from typing import Tuple, List

# To check if the given board configuration is solvable.
def is_solvable(board: Board) -> bool:
    # Flatten the board to calculate inversions
    flat_board: List[int] = []
    for i in range(board.size):
        for j in range(board.size):
            if board.tiles[i][j] != 0:  # Skip the blank tile
                flat_board.append(board.tiles[i][j])
    
    # Calculate inversions
    inversions: int = 0
    for i in range(len(flat_board)):
        for j in range(i + 1, len(flat_board)):
            if flat_board[i] > flat_board[j]:
                inversions += 1
    
    # testing for solvability
    if board.size % 2 == 1:
        return inversions % 2 == 0 # If grid size is odd, then puzzle is solvable if number of inversions is even
    else:  # Even grid size
        blank_row_position_from_bottom: int = board.size - board.blank_row # blank row position from the bottom starts from 1
        
        if blank_row_position_from_bottom % 2 == 0:  # Blank on even row from bottom
            return inversions % 2 == 1 # puzzle is solvable if number of inversions is odd
        else:  # Blank on odd row from bottom
            return inversions % 2 == 0 # puzzle is solvable if number of inversions is even

# Read the board configuration from standard input.
def read_board_from_input() -> Tuple[int, List]:
    # Read the board size
    size: int = int(input().strip())
    
    # Read the board configuration
    tiles: List[List[int]] = []
    for _ in range(size):
        row: List[int] = list(map(int, input().strip().split()))
        tiles.append(row)
    
    return size, tiles

# Print the solution path from initial state to goal state.
def print_solution(path : List[Node]) -> None:
    if not path:
        print("No solution found!")
        return
    
    print(f"\nMinimum number of moves = {len(path) - 1}\n")
    
    for board in path:
        print(board)
        print()  # Empty line between configurations