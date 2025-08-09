#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>

// Constants for the game
const int ROWS = 9;
const int COLS = 6;
const int EMPTY = 0;

// Player colors
enum Player {
    RED = 1,
    BLUE = 2
};

// Structure to represent a cell
struct Cell {
    int orbs;
    Player owner;
    
    Cell() : orbs(0), owner(RED) {}
    Cell(int o, Player p) : orbs(o), owner(p) {}
};

// Structure to represent a move
struct Move {
    int row, col;
    Player player;
    
    Move() : row(-1), col(-1), player(RED) {}
    Move(int r, int c, Player p) : row(r), col(c), player(p) {}
    
    bool isValid() const { return row >= 0 && row < ROWS && col >= 0 && col < COLS; }
};

// Game state class
class GameState {
private:
    std::vector<std::vector<Cell>> board;
    Player currentPlayer;
    bool gameOver;
    Player winner;

public:
    GameState();
    
    // Board operations
    void initializeBoard();
    bool makeMove(const Move& move);
    void explodeCell(int row, int col);
    bool isValidMove(const Move& move) const;
    std::vector<Move> getLegalMoves() const;
    
    // Game state queries
    int getCriticalMass(int row, int col) const;
    bool isGameOver() const;
    Player getWinner() const;
    Player getCurrentPlayer() const;
    void setCurrentPlayer(Player player);
    
    // Board access
    const Cell& getCell(int row, int col) const;
    void setCell(int row, int col, int orbs, Player owner);
    
    // File I/O
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename, const std::string& moveType) const;
    
    // Utility functions
    GameState copy() const;
    void printBoard() const;
    
private:
    void checkGameEnd();
    bool checkGameEndDuringExplosion(); // New method
    bool hasPlayerLost(Player player) const;
    std::vector<std::pair<int, int>> getNeighbors(int row, int col) const;
};

#endif // GAME_H