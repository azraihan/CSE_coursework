import heapq
from collections import defaultdict
from typing import Self, List, Tuple, Set, DefaultDict
from board import Board
from node import Node
from heuristics import Heuristic

class AStar:
    """
    A* search algorithm implementation for the N-puzzle problem.
    """
    def __init__(self, initial_board: Board, heuristic_func: Heuristic) -> None:
        """
        Initialize the A* solver.
        """
        self.initial_board = initial_board
        self.heuristic_func = heuristic_func
        
        # Statistics
        self.nodes_explored = 0
        self.nodes_expanded = 0
    
    def solve(self) -> List[Node]:
        """
        Solve the puzzle using A* search algorithm.
        Returns:
            list: The solution path or None if no solution is found
        """
        # Initialize open list and closed sets
        open_set: List = []  # Priority queue
        closed_set: Set = set()
        
        # Track the best g score for each board configuration
        g_scores: DefaultDict[Board, float] = defaultdict(lambda: float('inf'))
        
        # Create the initial node
        h_value: float = self.heuristic_func.calculate(self.initial_board)
        initial_node: Node = Node(self.initial_board, 0, h_value) # Parent node in 'None' by default
        
        # Add the initial node to the open set
        heapq.heappush(open_set, initial_node)
        self.nodes_explored += 1    ########################################################################
        g_scores[self.initial_board] = 0
        
        while open_set:
            # Get the node with the lowest f value
            current_node = heapq.heappop(open_set)
            # self.nodes_explored += 1    ########################################################################
            
            # Check if we've reached the goal state
            if current_node.board.is_goal():
                return self._reconstruct_path(current_node)
            
            # Add current node to closed set
            closed_set.add(current_node.board)
            
            # Expand the current node
            self.nodes_expanded += 1    ########################################################################
            
            # Generate all possible neighbors
            for neighbor_board in current_node.board.get_neighbors():
                # Skip if the neighbor is already explored
                if neighbor_board in closed_set:
                    continue
                
                # Calculate tentative g score (cost to reach neighbor)
                tentative_g = current_node.g + 1
                
                # proceed if this path to neighbor is better than any previous one
                if tentative_g < g_scores[neighbor_board]:
                    g_scores[neighbor_board] = tentative_g
                    
                    # Calculate heuristic value for the neighbor
                    h_value = self.heuristic_func.calculate(neighbor_board)
                    
                    # Create a new node
                    neighbor_node = Node(neighbor_board, tentative_g, h_value, current_node)
                    
                    # Add to open set
                    heapq.heappush(open_set, neighbor_node)
                    self.nodes_explored += 1    ########################################################################
        
        # No solution found
        return None
    
    def _reconstruct_path(self, node) -> List[Node]:
        """
        Reconstruct the path from the initial state to the goal state.
        Args:
            node (Node): The goal node
        Returns:
            list: The path from initial to goal state
        """
        path: List[Node] = []
        current: Node = node
        
        while current:
            path.append(current.board)
            current = current.parent
        
        # from initial to goal
        return path[::-1]
    
    def get_statistics(self) -> Tuple[int,int]:
        """
        Get search statistics.
        Returns:
            tuple: Number of nodes explored and expanded
        """
        return self.nodes_explored, self.nodes_expanded