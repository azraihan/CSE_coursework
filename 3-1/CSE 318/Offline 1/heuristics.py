import math
from board import Board

class Heuristic:
    """Base class for heuristic functions."""
    
    @staticmethod
    def calculate(board: Board) -> None:
        """Calculate the heuristic value for a given board."""
        pass

class HammingDistance(Heuristic):
    
    @staticmethod
    def calculate(board: Board) -> int:
        """Calculate the Hamming distance for a board."""
        count: int = 0
        size: int = board.size
        
        for i in range(size):
            for j in range(size):
                # Skip the blank tile
                if board.tiles[i][j] == 0:
                    continue
                
                # Calculate the correct value at this position
                correct_value = i * size + j + 1
                
                # Check if tile is in the bottom right corner which should be blank
                if i == size - 1 and j == size - 1:
                    if board.tiles[i][j] != 0:
                        count += 1
                # Check if the current tile is misplaced
                elif board.tiles[i][j] != correct_value:
                    count += 1
        
        return count

class ManhattanDistance(Heuristic):
    @staticmethod
    def calculate(board: Board) -> int:
        """Calculate the Manhattan distance for a board."""
        distance: int = 0
        size: int = board.size
        
        for i in range(size):
            for j in range(size):
                # Skip the blank tile
                if board.tiles[i][j] == 0:
                    continue
                
                # Calculate the goal position for this tile
                value: int = board.tiles[i][j]
                goal_row, goal_col = (value - 1) // size, (value - 1) % size
                
                # Add the Manhattan distance for this tile
                distance += abs(i - goal_row) + abs(j - goal_col)
        
        return distance

class EuclideanDistance(Heuristic):
    @staticmethod
    def calculate(board: Board) -> float:
        """Calculate the Euclidean distance for a board."""
        distance: float = 0
        size: int = board.size
        
        for i in range(size):
            for j in range(size):
                # Skip the blank tile
                if board.tiles[i][j] == 0:
                    continue
                
                # Calculate the goal position for this tile
                value: int = board.tiles[i][j]
                goal_row, goal_col = (value - 1) // size, (value - 1) % size
                
                # Add the Euclidean distance for this tile
                distance += math.sqrt((i - goal_row)**2 + (j - goal_col)**2)
        
        return distance

class LinearConflict(Heuristic):
    @staticmethod
    def calculate(board: Board) -> int:
        """Calculate the Linear Conflict heuristic for a board."""

        # Start with the Manhattan distance
        distance: int = ManhattanDistance.calculate(board)
        
        # Add the linear conflict contribution
        linear_conflicts: int = LinearConflict._count_linear_conflicts(board)
        
        # Each linear conflict adds 2 moves to the Manhattan distance
        return distance + 2 * linear_conflicts
    
    @staticmethod
    def _count_linear_conflicts(board: Board) -> int:
        """Count the number of linear conflicts in a board."""
        conflicts: int = 0
        size: int = board.size
        
        # Check conflicts in rows
        for i in range(size):
            conflicts += LinearConflict._count_conflicts_in_row(board, i)
        
        # Check conflicts in columns
        for j in range(size):
            conflicts += LinearConflict._count_conflicts_in_column(board, j)
        
        return conflicts
    
    @staticmethod
    def _count_conflicts_in_row(board: Board, row: int) -> int:
        """Count conflicts in a particular row."""
        size: int = board.size
        conflicts: int = 0
        
        for j in range(size):
            if board.tiles[row][j] == 0:  # Skip blank tile
                continue
                
            # Check if this tile belongs to this row in the goal state
            tile_value: int = board.tiles[row][j]
            goal_row: int = (tile_value - 1) // size
            
            if goal_row == row:  # If tile belongs to this row
                # Check for conflicts with other tiles in this row
                for k in range(j + 1, size):
                    if board.tiles[row][k] == 0:  # Skip blank tile
                        continue
                        
                    other_tile_value: int = board.tiles[row][k]
                    other_goal_row: int = (other_tile_value - 1) // size
                    
                    if other_goal_row == row:  # If other tile also belongs to this row
                        # Get goal columns for both tiles
                        goal_col_j: int = (tile_value - 1) % size
                        goal_col_k: int = (other_tile_value - 1) % size
                        
                        # Check if they're in wrong order
                        if goal_col_j > goal_col_k:
                            conflicts += 1
        
        return conflicts
    
    @staticmethod
    def _count_conflicts_in_column(board: Board, col: int) -> int:
        """Count conflicts in a particular column."""
        size: int = board.size
        conflicts: int = 0
        
        for i in range(size):
            if board.tiles[i][col] == 0:  # Skip blank tile
                continue
                
            # Check if this tile belongs to this column in the goal state
            tile_value: int = board.tiles[i][col]
            goal_col: int = (tile_value - 1) % size
            
            if goal_col == col:  # If tile belongs to this column
                # Check for conflicts with other tiles in this column
                for k in range(i + 1, size):
                    if board.tiles[k][col] == 0:  # Skip blank tile
                        continue
                        
                    other_tile_value: int = board.tiles[k][col]
                    other_goal_col: int = (other_tile_value - 1) % size
                    
                    if other_goal_col == col:  # If other tile also belongs to this column
                        # Get goal rows for both tiles
                        goal_row_i: int = (tile_value - 1) // size
                        goal_row_k: int = (other_tile_value - 1) // size
                        
                        # Check if they're in wrong order
                        if goal_row_i > goal_row_k:
                            conflicts += 1
        
        return conflicts