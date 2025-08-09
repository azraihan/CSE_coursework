import pygame
import sys
import os
import time
from enum import Enum

# Initialize Pygame
pygame.init()

# Constants
ROWS = 9
COLS = 6
CELL_SIZE = 80
GRID_WIDTH = COLS * CELL_SIZE
GRID_HEIGHT = ROWS * CELL_SIZE
SIDEBAR_WIDTH = 200
WINDOW_WIDTH = GRID_WIDTH + SIDEBAR_WIDTH
WINDOW_HEIGHT = GRID_HEIGHT  # Removed the +100 gap

# Professional Colors - RGB values optimized for the colorful orb background
WHITE = (255, 255, 255)              # Maximum contrast for titles
LIGHT_GRAY = (240, 240, 240)         # Softer white for headings
BODY_TEXT = (220, 220, 220)          # Body text color
SECONDARY_TEXT = (200, 210, 220)     # Secondary text with blue tint
SECONDARY_BORDER = (200, 210, 220)  
MEDIUM_GRAY = (180, 180, 180)        # De-emphasized text
GRAY = (200, 200, 200)
INTERACTIVE = (180, 220, 240)        # Interactive elements
STRONG_YELLOW = (255, 255, 100)      # Very light yellowish for specific text
LIGHT_YELLOW = (255, 255, 150)
PASTE = (219, 245, 255)
LIGHT_PURPLE = (255, 230, 255)

# Original colors for game elements
BLACK = (0, 0, 0)
GRAY = (128, 128, 128)
DARK_GRAY = (64, 64, 64)
RED = (220, 50, 50)
BLUE = (50, 50, 220)
LIGHT_RED = (255, 150, 150)
LIGHT_BLUE = (150, 150, 255)
GREEN = (50, 200, 50)

class Player(Enum):
    RED = 1
    BLUE = 2

class Cell:
    def __init__(self, orbs=0, owner=Player.RED):
        self.orbs = orbs
        self.owner = owner

class ChainReactionUI:
    def __init__(self):
        self.screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        pygame.display.set_caption("Chain Reaction")
        
        # Load and process the background GIF
        self.background_frames = []
        self.current_frame = 0
        self.frame_timer = 0
        self.frame_delay = 100  # milliseconds between frames
        self.scaled_gif_height = 0
        self.load_background_gif()
        
        # Load professional fonts
        # Try to use system fonts first, fallback to pygame defaults
        try:
            # For title - bold, modern sans-serif
            self.title_font = pygame.font.SysFont('segoeui', 40, bold=True)  # Windows
            if not self.title_font:
                self.title_font = pygame.font.SysFont('arial', 40, bold=True)  # Fallback
        except:
            self.title_font = pygame.font.Font(None, 42)
        
        try:
            # For regular text - clean, readable
            self.font = pygame.font.SysFont('segoeui', 20, bold=False)
            if not self.font:
                self.font = pygame.font.SysFont('arial', 20, bold=False)
        except:
            self.font = pygame.font.Font(None, 28)
        
        try:
            # For small text - lighter weight
            self.small_font = pygame.font.SysFont('segoeui', 16, bold=False)
            if not self.small_font:
                self.small_font = pygame.font.SysFont('arial', 16, bold=False)
        except:
            self.small_font = pygame.font.Font(None, 22)
        
        try:
            # For current player text - medium bold
            self.player_font = pygame.font.SysFont('segoeui', 24, bold=True)
            if not self.player_font:
                self.player_font = pygame.font.SysFont('arial', 24, bold=True)
        except:
            self.player_font = pygame.font.Font(None, 32)
        
        # Game state
        self.board = [[Cell() for _ in range(COLS)] for _ in range(ROWS)]
        self.current_player = Player.RED
        self.game_over = False
        self.winner = None
        self.waiting_for_ai = False
        
        # Detect game mode from environment variable or file
        self.ai_vs_ai_mode = self.detect_game_mode()
        
        # File path
        self.game_file = "../game-info/gamestate.txt"
        
        # UI elements
        self.selected_cell = None
        
        # Track last file modification time for AI vs AI mode
        self.last_file_time = 0
        
        # Initialize empty board file
        # self.save_game_state("Human Move")
        
        # # Set initial state for AI vs AI mode
        # if self.ai_vs_ai_mode:
        #     self.waiting_for_ai = True
        #     print("AI vs AI mode detected - watching for moves...")
        
        # Initialize board file based on mode
        if self.ai_vs_ai_mode:
            self.save_game_state("AI Move")  # This will trigger Red AI
            self.waiting_for_ai = True
            print("AI vs AI mode detected - watching for moves...")
        else:
            self.save_game_state("Human Move")
    
    def detect_game_mode(self):
        """Detect if we're in AI vs AI mode"""
        # Check for environment variable
        if os.getenv('AI_VS_AI_MODE') == '1':
            return True
        
        # Check for mode file
        mode_file = "../game-info/game_mode.txt"
        if os.path.exists(mode_file):
            try:
                with open(mode_file, 'r') as f:
                    mode = f.read().strip()
                    return mode == "AI_VS_AI"
            except:
                pass
        
        return False
    
    def load_background_gif(self):
        """Load and scale the background GIF for the sidebar"""
        try:
            # Try to load the GIF using pygame
            # Note: pygame doesn't natively support animated GIFs, so we'll try to load as static first
            # For animated GIFs, you might need the Pillow library
            
            # Check if file exists
            gif_path = "chain_reaction.gif"  # Assuming it's in the same directory
            if os.path.exists(gif_path):
                try:
                    # Try using Pillow for proper GIF support
                    from PIL import Image, ImageSequence
                    
                    # Load the GIF
                    gif = Image.open(gif_path)
                    
                    # Calculate scaled dimensions to fit sidebar width while maintaining aspect ratio
                    original_width, original_height = gif.size
                    scale_factor = SIDEBAR_WIDTH / original_width
                    self.scaled_gif_height = int(original_height * scale_factor)
                    
                    # Extract all frames and scale them
                    for frame in ImageSequence.Iterator(gif):
                        # Convert PIL image to pygame surface
                        frame_rgba = frame.convert('RGBA')
                        frame_scaled = frame_rgba.resize((SIDEBAR_WIDTH, self.scaled_gif_height), Image.Resampling.LANCZOS)
                        
                        # Convert to pygame surface
                        frame_string = frame_scaled.tobytes()
                        frame_surface = pygame.image.fromstring(frame_string, frame_scaled.size, 'RGBA')
                        self.background_frames.append(frame_surface)
                    
                    print(f"Loaded {len(self.background_frames)} frames from GIF")
                    
                except ImportError:
                    print("Pillow not available, trying pygame image load...")
                    # Fallback to static image loading
                    try:
                        static_image = pygame.image.load(gif_path)
                        # Scale to fit sidebar width
                        original_width = static_image.get_width()
                        original_height = static_image.get_height()
                        scale_factor = SIDEBAR_WIDTH / original_width
                        self.scaled_gif_height = int(original_height * scale_factor)
                        
                        scaled_image = pygame.transform.scale(static_image, (SIDEBAR_WIDTH, self.scaled_gif_height))
                        self.background_frames.append(scaled_image)
                        print("Loaded GIF as static image")
                    except:
                        print("Could not load GIF file")
                        self.create_fallback_background()
                        
            else:
                print(f"GIF file not found: {gif_path}")
                self.create_fallback_background()
                
        except Exception as e:
            print(f"Error loading background GIF: {e}")
            self.create_fallback_background()
    
    def create_fallback_background(self):
        """Create a simple fallback background if GIF loading fails"""
        # Create a simple gradient background as fallback
        fallback_surface = pygame.Surface((SIDEBAR_WIDTH, SIDEBAR_WIDTH))
        for y in range(SIDEBAR_WIDTH):
            color_value = int(200 + (55 * y / SIDEBAR_WIDTH))
            color = (color_value, color_value, color_value)
            pygame.draw.line(fallback_surface, color, (0, y), (SIDEBAR_WIDTH, y))
        
        self.background_frames.append(fallback_surface)
        self.scaled_gif_height = SIDEBAR_WIDTH
    
    def get_critical_mass(self, row, col):
        """Get critical mass for a cell based on its position"""
        if (row == 0 or row == ROWS - 1) and (col == 0 or col == COLS - 1):
            return 2  # Corner
        elif row == 0 or row == ROWS - 1 or col == 0 or col == COLS - 1:
            return 3  # Edge
        else:
            return 4  # Center
    
    def is_valid_move(self, row, col):
        """Check if a move is valid"""
        if row < 0 or row >= ROWS or col < 0 or col >= COLS:
            return False
        
        cell = self.board[row][col]
        return cell.orbs == 0 or cell.owner == self.current_player
    
    def make_move(self, row, col):
        """Make a move and handle explosions"""
        if not self.is_valid_move(row, col) or self.game_over:
            return False
        
        # Place orb
        self.board[row][col].orbs += 1
        self.board[row][col].owner = self.current_player
        
        # Handle explosions
        self.handle_explosions()
        
        # Check game end
        self.check_game_end()
        
        # Switch players
        self.current_player = Player.BLUE if self.current_player == Player.RED else Player.RED
        
        return True
    
    def handle_explosions(self):
        """Handle chain explosions"""
        explosion_queue = []
        
        # Find initial explosions
        for i in range(ROWS):
            for j in range(COLS):
                if self.board[i][j].orbs >= self.get_critical_mass(i, j):
                    explosion_queue.append((i, j))
        
        # Process explosions
        while explosion_queue:
            row, col = explosion_queue.pop(0)
            
            if self.board[row][col].orbs < self.get_critical_mass(row, col):
                continue
            
            exploding_player = self.board[row][col].owner
            
            # Clear exploding cell
            self.board[row][col].orbs = 0
            
            # Get neighbors
            neighbors = self.get_neighbors(row, col)
            
            # Distribute orbs to neighbors
            for nr, nc in neighbors:
                self.board[nr][nc].orbs += 1
                self.board[nr][nc].owner = exploding_player
                
                # Check if neighbor should explode
                if self.board[nr][nc].orbs >= self.get_critical_mass(nr, nc):
                    if (nr, nc) not in explosion_queue:
                        explosion_queue.append((nr, nc))
    
    def get_neighbors(self, row, col):
        """Get valid neighbors of a cell"""
        neighbors = []
        directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
        
        for dr, dc in directions:
            nr, nc = row + dr, col + dc
            if 0 <= nr < ROWS and 0 <= nc < COLS:
                neighbors.append((nr, nc))
        
        return neighbors
    
    def check_game_end(self):
        """Check if game has ended"""
        red_orbs = 0
        blue_orbs = 0
        total_moves = 0
        
        # Count orbs for each player and total moves
        for i in range(ROWS):
            for j in range(COLS):
                if self.board[i][j].orbs > 0:
                    total_moves += self.board[i][j].orbs
                    if self.board[i][j].owner == Player.RED:
                        red_orbs += 1
                    else:
                        blue_orbs += 1
        
        # Only check for winner if both players have made moves (at least 2 total moves)
        if total_moves >= 2:
            if red_orbs == 0 and blue_orbs > 0:
                self.game_over = True
                self.winner = Player.BLUE
            elif blue_orbs == 0 and red_orbs > 0:
                self.game_over = True
                self.winner = Player.RED
    
    def has_file_changed(self):
        """Check if the game file has been modified"""
        try:
            if os.path.exists(self.game_file):
                current_time = os.path.getmtime(self.game_file)
                if current_time > self.last_file_time:
                    self.last_file_time = current_time
                    return True
            return False
        except:
            return False
    
    def load_game_state(self):
        """Load game state from file"""
        try:
            with open(self.game_file, 'r') as f:
                lines = f.readlines()
            
            if not lines:
                return False
            
            header = lines[0].strip()
            
            # Clear board
            for i in range(ROWS):
                for j in range(COLS):
                    self.board[i][j] = Cell()
            
            # Parse board
            for i in range(1, min(len(lines), ROWS + 1)):
                cells = lines[i].strip().split()
                for j in range(min(len(cells), COLS)):
                    cell_str = cells[j]
                    if cell_str == "0":
                        self.board[i-1][j] = Cell(0, Player.RED)
                    else:
                        orbs = int(cell_str[0])
                        owner = Player.RED if cell_str[1] == 'R' else Player.BLUE
                        self.board[i-1][j] = Cell(orbs, owner)
            
            # Check for game end after loading
            self.check_game_end()
            
            # Set current player and waiting state based on header and mode
            if self.ai_vs_ai_mode:
                # In AI vs AI mode, determine who should move next
                if "AI Move:" in header:
                    # Blue AI just moved, Red AI's turn next
                    self.current_player = Player.RED
                elif "Human Move:" in header:
                    # Red AI just moved (saved as "Human Move"), Blue AI's turn next
                    self.current_player = Player.BLUE
                # Always waiting for AI in AI vs AI mode (unless game is over)
                self.waiting_for_ai = not self.game_over
            else:
                # Original Human vs AI mode
                if "AI Move:" in header:
                    self.current_player = Player.RED
                    self.waiting_for_ai = False
                elif "Human Move:" in header:
                    self.current_player = Player.BLUE
                    self.waiting_for_ai = True
            
            return True
            
        except Exception as e:
            print(f"Error loading game state: {e}")
            return False
    
    def save_game_state(self, move_type):
        """Save game state to file"""
        try:
            # Create directory if it doesn't exist
            os.makedirs(os.path.dirname(self.game_file), exist_ok=True)
            
            with open(self.game_file, 'w') as f:
                f.write(f"{move_type}:\n")
                
                for i in range(ROWS):
                    row_str = []
                    for j in range(COLS):
                        cell = self.board[i][j]
                        if cell.orbs == 0:
                            row_str.append("0")
                        else:
                            owner_char = 'R' if cell.owner == Player.RED else 'B'
                            row_str.append(f"{cell.orbs}{owner_char}")
                    f.write(" ".join(row_str) + "\n")
            
            # Update our file time tracker
            self.last_file_time = os.path.getmtime(self.game_file)
            
            return True
            
        except Exception as e:
            print(f"Error saving game state: {e}")
            return False
    
    def draw_board(self):
        """Draw the game board"""
        # Draw grid
        for i in range(ROWS + 1):
            pygame.draw.line(self.screen, BLACK, 
                           (0, i * CELL_SIZE), (GRID_WIDTH, i * CELL_SIZE), 2)
        
        for j in range(COLS + 1):
            pygame.draw.line(self.screen, BLACK, 
                           (j * CELL_SIZE, 0), (j * CELL_SIZE, GRID_HEIGHT), 2)
        
        # Draw cells
        for i in range(ROWS):
            for j in range(COLS):
                cell = self.board[i][j]
                x = j * CELL_SIZE
                y = i * CELL_SIZE
                
                # Highlight selected cell (only in human vs AI mode)
                if not self.ai_vs_ai_mode and self.selected_cell == (i, j):
                    pygame.draw.rect(self.screen, GREEN, 
                                   (x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4), 3)
                
                # Draw orbs
                if cell.orbs > 0:
                    color = RED if cell.owner == Player.RED else BLUE
                    light_color = LIGHT_RED if cell.owner == Player.RED else LIGHT_BLUE
                    
                    # Draw orb representation
                    center_x = x + CELL_SIZE // 2
                    center_y = y + CELL_SIZE // 2
                    
                    if cell.orbs == 1:
                        pygame.draw.circle(self.screen, color, (center_x, center_y), 15)
                        pygame.draw.circle(self.screen, light_color, (center_x, center_y), 12)
                    elif cell.orbs == 2:
                        pygame.draw.circle(self.screen, color, (center_x - 10, center_y), 12)
                        pygame.draw.circle(self.screen, light_color, (center_x - 10, center_y), 9)
                        pygame.draw.circle(self.screen, color, (center_x + 10, center_y), 12)
                        pygame.draw.circle(self.screen, light_color, (center_x + 10, center_y), 9)
                    elif cell.orbs == 3:
                        pygame.draw.circle(self.screen, color, (center_x - 12, center_y - 8), 10)
                        pygame.draw.circle(self.screen, light_color, (center_x - 12, center_y - 8), 7)
                        pygame.draw.circle(self.screen, color, (center_x + 12, center_y - 8), 10)
                        pygame.draw.circle(self.screen, light_color, (center_x + 12, center_y - 8), 7)
                        pygame.draw.circle(self.screen, color, (center_x, center_y + 10), 10)
                        pygame.draw.circle(self.screen, light_color, (center_x, center_y + 10), 7)
                    else:  # 4 or more orbs
                        positions = [(-10, -10), (10, -10), (-10, 10), (10, 10)]
                        for px, py in positions:
                            pygame.draw.circle(self.screen, color, 
                                             (center_x + px, center_y + py), 8)
                            pygame.draw.circle(self.screen, light_color, 
                                             (center_x + px, center_y + py), 6)
                        
                        # Show orb count for more than 4 - using WHITE for maximum contrast
                        if cell.orbs > 4:
                            text = self.small_font.render(str(cell.orbs), True, WHITE)
                            text_rect = text.get_rect(center=(center_x, center_y))
                            self.screen.blit(text, text_rect)
    
    def draw_sidebar(self):
        """Draw the sidebar with game information"""
        sidebar_x = GRID_WIDTH
        
        # Draw tiled background GIF
        if self.background_frames:
            current_surface = self.background_frames[self.current_frame]
            
            # Calculate how many tiles we need vertically
            tiles_needed = (WINDOW_HEIGHT + self.scaled_gif_height - 1) // self.scaled_gif_height
            
            # Draw tiles vertically
            for i in range(tiles_needed):
                y_pos = i * self.scaled_gif_height
                # Only draw if the tile is visible
                if y_pos < WINDOW_HEIGHT:
                    # Clip the surface if it extends beyond window height
                    if y_pos + self.scaled_gif_height > WINDOW_HEIGHT:
                        # Need to clip the bottom
                        clip_height = WINDOW_HEIGHT - y_pos
                        clipped_surface = pygame.Surface((SIDEBAR_WIDTH, clip_height))
                        clipped_surface.blit(current_surface, (0, 0), (0, 0, SIDEBAR_WIDTH, clip_height))
                        self.screen.blit(clipped_surface, (sidebar_x, y_pos))
                    else:
                        self.screen.blit(current_surface, (sidebar_x, y_pos))
        else:
            # Fallback solid background
            pygame.draw.rect(self.screen, LIGHT_GRAY, 
                            (sidebar_x, 0, SIDEBAR_WIDTH, WINDOW_HEIGHT))
        
        # Create semi-transparent overlay for better text readability
        overlay = pygame.Surface((SIDEBAR_WIDTH, WINDOW_HEIGHT))
        overlay.set_alpha(20)  # Reduced opacity for better color visibility
        overlay.fill((255, 255, 255))  # White overlay
        self.screen.blit(overlay, (sidebar_x, 0))
        
        # Title - using WHITE for maximum contrast
        title_lines = ["Chain", "Reaction"]
        y_offset = 20
        for line in title_lines:
            title = self.title_font.render(line, True, WHITE)
            # Subtle shadow for depth
            shadow = self.title_font.render(line, True, (200, 200, 200))
            self.screen.blit(shadow, (sidebar_x + 16, y_offset + 1))
            self.screen.blit(title, (sidebar_x + 15, y_offset))
            y_offset += 50
        
        # Current player - modified for AI vs AI mode
        y_offset += 40
        if not self.game_over:
            if self.ai_vs_ai_mode:
                # AI vs AI mode - show which AI is thinking/playing
                if self.waiting_for_ai:
                    if self.current_player == Player.RED:
                        player_text = "AI (Red)\nThinking..."
                        color = LIGHT_PURPLE
                    else:
                        player_text = "AI (Blue)\nThinking..."
                        color = PASTE
                else:
                    if self.current_player == Player.RED:
                        player_text = "AI's (Red)\nTurn"
                        color = LIGHT_PURPLE
                    else:
                        player_text = "AI's (Blue)\nTurn"
                        color = PASTE
                shadow_color = MEDIUM_GRAY
            else:
                # Original Human vs AI mode
                if self.waiting_for_ai:
                    player_text = "AI Thinking..."
                    color = PASTE
                    shadow_color = MEDIUM_GRAY
                else:
                    player_name = "Red" if self.current_player == Player.RED else "Blue"
                    player_text = f"{player_name}'s Turn"
                    color = LIGHT_PURPLE if self.current_player == Player.RED else PASTE
                    shadow_color = MEDIUM_GRAY
        else:
            # Game over - show winner
            if self.ai_vs_ai_mode:
                winner_name = "AI (Red)" if self.winner == Player.RED else "AI (Blue)"
            else:
                winner_name = "Red" if self.winner == Player.RED else "Blue"
            player_text = f"{winner_name} Wins!"
            color = LIGHT_PURPLE if self.winner == Player.RED else PASTE
            shadow_color = BODY_TEXT
        
        # Add shadow for player text
        # shadow = self.player_font.render(player_text, True, shadow_color)
        # text = self.player_font.render(player_text, True, color)
        # self.screen.blit(shadow, (sidebar_x + 17, y_offset + 2))
        # self.screen.blit(text, (sidebar_x + 15, y_offset))
        # Replace the single line rendering with:
        lines = player_text.split('\n')
        for i, line in enumerate(lines):
            shadow = self.player_font.render(line, True, shadow_color)
            text = self.player_font.render(line, True, color)
            line_y = y_offset + (i * 30)  # 30px spacing between lines
            self.screen.blit(shadow, (sidebar_x + 17, line_y + 2))
            self.screen.blit(text, (sidebar_x + 15, line_y))
        
        # Instructions - using professional color hierarchy
        y_offset += 100
        
        if self.ai_vs_ai_mode:
            instructions = [
                "Watch AI vs AI",
                "competition",
                "",
                "Orbs explode when",
                "they reach critical",
                "mass:",
                "",
                "• Corner: 2 orbs",
                "• Edge: 3 orbs", 
                "• Center: 4 orbs",
                "",
                "Red AI vs Blue AI"
            ]
        else:
            instructions = [
                "Click a cell to",
                "place an orb",
                "",
                "Orbs explode when",
                "they reach critical",
                "mass:",
                "",
                "• Corner: 2 orbs",
                "• Edge: 3 orbs", 
                "• Center: 4 orbs",
                "",
                "Win by eliminating",
                "all opponent orbs"
            ]
        
        for line in instructions:
            if line == "":
                y_offset += 15  # Smaller gap for empty lines
            else:
                if line.startswith("•"):
                    # Bullet points - using white for contrast
                    text = self.small_font.render(line, True, WHITE)
                    self.screen.blit(text, (sidebar_x + 15, y_offset))
                elif line in ["Click a cell to", "place an orb", "Watch AI vs AI", "competition",
                             "Orbs explode when", "they reach critical", "mass:", 
                             "Win by eliminating", "all opponent orbs", "Red AI vs Blue AI"]:
                    # Specific text lines - using very light yellowish color
                    shadow = self.font.render(line, True, MEDIUM_GRAY)
                    text = self.font.render(line, True, STRONG_YELLOW)
                    self.screen.blit(shadow, (sidebar_x + 17, y_offset + 1))
                    self.screen.blit(text, (sidebar_x + 15, y_offset))
                else:
                    # Regular text - using BODY_TEXT
                    shadow = self.font.render(line, True, MEDIUM_GRAY)
                    text = self.font.render(line, True, BODY_TEXT)
                    self.screen.blit(shadow, (sidebar_x + 17, y_offset + 1))
                    self.screen.blit(text, (sidebar_x + 15, y_offset))
                
                y_offset += 25
    
    def handle_click(self, pos):
        """Handle mouse click"""
        # Only allow clicks in Human vs AI mode
        if self.ai_vs_ai_mode or self.game_over or self.waiting_for_ai:
            return
        
        x, y = pos
        if x < GRID_WIDTH:  # Click on board
            col = x // CELL_SIZE
            row = y // CELL_SIZE
            
            if 0 <= row < ROWS and 0 <= col < COLS:
                if self.is_valid_move(row, col):
                    if self.make_move(row, col):
                        # Save state after human move
                        self.save_game_state("Human Move")
                        if not self.game_over:
                            self.waiting_for_ai = True
    
    def update(self):
        """Update game state"""
        if self.ai_vs_ai_mode:
            # In AI vs AI mode, constantly check for file changes
            if not self.game_over and self.has_file_changed():
                print("File changed, loading new state...")
                self.load_game_state()
        else:
            # Original Human vs AI mode
            if self.waiting_for_ai and not self.game_over:
                # Check if AI has made a move
                if self.load_game_state():
                    pass  # State updated
        
        # Update GIF animation
        if len(self.background_frames) > 1:
            self.frame_timer += 16  # Approximately 60 FPS
            if self.frame_timer >= self.frame_delay:
                self.current_frame = (self.current_frame + 1) % len(self.background_frames)
                self.frame_timer = 0
    
    def run(self):
        """Main game loop"""
        clock = pygame.time.Clock()
        
        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit()
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    self.handle_click(event.pos)
                elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_r and self.game_over:
                        # Reset game
                        self.__init__()
            
            self.update()
            
            # Draw everything
            self.screen.fill(WHITE)
            self.draw_board()
            self.draw_sidebar()
            
            pygame.display.flip()
            clock.tick(60)

if __name__ == "__main__":
    game = ChainReactionUI()
    game.run()