import copy
from typing import List, Tuple, Self

class Board:
    """
    Class to represent the N-Puzzle board.
    """

    def __init__(self, size: int, tiles: List[List[int]] = None) -> None:
        """Initializes the board with given size and tiles."""
        self.size = size
        if tiles:
            self.tiles = tiles
        else:
            # Create goal board if no initial configuration is provided
            self.tiles = self._create_goal_board()
        
        # Find the blank position (0)
        self.blank_row, self.blank_col = self._find_blank()
        
    def _create_goal_board(self) -> List[List[int]]:
        """Create the goal board configuration."""
        tiles: List[List[int]] = []
        count: int = 1
        for i in range(self.size):
            row: List[int] = []
            for j in range(self.size):
                if i == self.size - 1 and j == self.size - 1:
                    row.append(0)  # Blank tile at the bottom right
                else:
                    row.append(count)
                count += 1
            tiles.append(row)
        return tiles
    
    def _find_blank(self) -> Tuple[int, int]:
        """Find the position of the blank tile (0)."""
        for i in range(self.size):
            for j in range(self.size):
                if self.tiles[i][j] == 0:
                    return i, j
        return -1, -1  # Should never happen if the board is valid
    
    def is_goal(self) -> bool:
        """Check if the current board is in the goal state."""
        count: int = 1
        for i in range(self.size):
            for j in range(self.size):
                # Skip the bottom right which should be blank (0)
                if i == self.size - 1 and j == self.size - 1:
                    if self.tiles[i][j] != 0:
                        return False
                else:
                    if self.tiles[i][j] != count:
                        return False
                    count += 1
        return True
    
    def get_neighbors(self) -> List[Self]:
        """Generate all possible neighboring boards by moving the blank tile."""
        neighbors: List[Self] = []
        
        # Possible moves: down, up, right, left
        moves: List[Tuple[int, int]] = [(-1, 0), (1, 0), (0, -1), (0, 1)]
        
        for dx, dy in moves:
            new_row, new_col = self.blank_row + dx, self.blank_col + dy
            
            # Check if the new position is valid
            if 0 <= new_row < self.size and 0 <= new_col < self.size:
                # Create a new board with the moved tile
                new_tiles: List[List[int]] = copy.deepcopy(self.tiles)
                
                # Swap the blank tile with the adjacent tile
                new_tiles[self.blank_row][self.blank_col] = new_tiles[new_row][new_col]
                new_tiles[new_row][new_col] = 0
                
                # Create a new board with the new configuration
                new_board: Board = Board(self.size, new_tiles)
                neighbors.append(new_board)
        
        return neighbors
    
    def flat_tiles(self) -> Tuple[int, ...]:
        """Return tiles as a flattened list for hashing and comparison."""
        flat = []
        for row in self.tiles:
            flat.extend(row)
        return tuple(flat)
    
    def __eq__(self, other: Self):
        """Check if two boards are equal."""
        # if not isinstance(other, Board):
        #     return False
        return self.flat_tiles() == other.flat_tiles()
    
    def __hash__(self):
        """Hash function for the board to use in sets and dictionaries."""
        return hash(self.flat_tiles())
    
    def __str__(self):
        """String representation of the board."""
        result: str = ""
        for row in self.tiles:
            result += " ".join(str(tile) for tile in row) + "\n"
        return result.strip()