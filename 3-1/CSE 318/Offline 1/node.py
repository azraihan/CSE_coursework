from board import Board
from typing import Self

class Node:
    """
    A node in the search tree.
    """
    def __init__(self, board: Board, g: int=0, h: int=0, parent: Self =None) -> None:
        """
        Initialize a search node.
        """
        self.board = board
        self.g = g          # Cost to reach this node
        self.h = h          # Heuristic value
        self.parent = parent
        self.f = g + h      # Total estimated cost
    
    def __lt__(self, other):
        """
        Comparison method for priority queue ordering.
        Nodes with lower f value have higher priority.
        If f values are equal, compare h values to break ties.
        """
        if self.f == other.f:
            return self.h < other.h
        return self.f < other.f
    
    def __eq__(self, other: Self):
        """
        Check if two nodes are equal (have the same board configuration).
        """
        # if not isinstance(other, Node):
        #     return False
        return self.board == other.board
    
    def __hash__(self):
        """
        Hash function for the node to use in sets and dictionaries.
        """
        return hash(self.board)