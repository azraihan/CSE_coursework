#include "game.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <sstream>
#include <set>

using namespace std;

GameState::GameState()
{
    initializeBoard();
    currentPlayer = RED;
    gameOver = false;
    winner = RED;
}

void GameState::initializeBoard()
{
    board.resize(ROWS, vector<Cell>(COLS));
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            board[i][j] = Cell(0, RED);
        }
    }
}

int GameState::getCriticalMass(int row, int col) const
{
    int mass = 4; // Default for inner cells

    // Corner cells have critical mass of 2
    if ((row == 0 || row == ROWS - 1) && (col == 0 || col == COLS - 1))
    {
        mass = 2;
    }
    // Edge cells have critical mass of 3
    else if (row == 0 || row == ROWS - 1 || col == 0 || col == COLS - 1)
    {
        mass = 3;
    }

    return mass;
}

vector<pair<int, int>> GameState::getNeighbors(int row, int col) const
{
    vector<pair<int, int>> neighbors;

    // Check all four directions
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++)
    {
        int newRow = row + dr[i];
        int newCol = col + dc[i];

        if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS)
        {
            neighbors.push_back({newRow, newCol});
        }
    }

    return neighbors;
}

bool GameState::isValidMove(const Move &move) const
{
    if (!move.isValid())
        return false;

    const Cell &cell = board[move.row][move.col];

    // Can place in empty cell or cell with same color orbs
    return cell.orbs == 0 || cell.owner == move.player;
}

vector<Move> GameState::getLegalMoves() const
{
    vector<Move> moves;

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            Move move(i, j, currentPlayer);
            if (isValidMove(move))
            {
                moves.push_back(move);
            }
        }
    }

    return moves;
}

bool GameState::makeMove(const Move &move)
{
    if (!isValidMove(move))
        return false;

    // Place the orb
    board[move.row][move.col].orbs++;
    board[move.row][move.col].owner = move.player;

    // Handle explosions
    explodeCell(move.row, move.col);

    // Check if game ended
    checkGameEnd();

    // Switch players
    currentPlayer = (currentPlayer == RED) ? BLUE : RED;

    return true;
}

bool GameState::checkGameEndDuringExplosion()
{
    int redCells = 0;
    int blueCells = 0;
    int totalCells = 0;

    // Count cells by owner
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (board[i][j].orbs > 0)
            {
                totalCells++;
                if (board[i][j].owner == RED)
                {
                    redCells++;
                }
                else
                {
                    blueCells++;
                }
            }
        }
    }

    // Only check for winner if both players have made moves (at least 2 total cells)
    if (totalCells >= 2)
    {
        if (redCells == 0 && blueCells > 0)
        {
            gameOver = true;
            winner = BLUE;
            return true;
        }
        else if (blueCells == 0 && redCells > 0)
        {
            gameOver = true;
            winner = RED;
            return true;
        }
    }

    return false;
}

void GameState::explodeCell(int row, int col)
{
    queue<pair<int, int>> explosionQueue;
    bool inQueue[ROWS][COLS] = {false};

    // Initial cell
    if (board[row][col].orbs >= getCriticalMass(row, col))
    {
        explosionQueue.push({row, col});
        inQueue[row][col] = true;
    }

    bool gameEndedDuringExplosion = false;
    int stabilizingExplosions = 0;
    const int MAX_STABILIZING_EXPLOSIONS = 100; // Threshold for stabilization

    while (!explosionQueue.empty())
    {
        auto [r, c] = explosionQueue.front();
        explosionQueue.pop();
        inQueue[r][c] = false;

        // Skip if cell no longer critical
        if (board[r][c].orbs < getCriticalMass(r, c))
        {
            continue;
        }

        Player explodingPlayer = board[r][c].owner;
        int criticalMass = getCriticalMass(r, c);
        
        // Explode: remove critical mass orbs and distribute
        board[r][c].orbs -= criticalMass;

        // Distribute to neighbors
        if (r > 0)
        {
            board[r - 1][c].orbs++;
            board[r - 1][c].owner = explodingPlayer;
            if (board[r - 1][c].orbs >= getCriticalMass(r - 1, c) && !inQueue[r - 1][c])
            {
                explosionQueue.push({r - 1, c});
                inQueue[r - 1][c] = true;
            }
        }
        if (r < ROWS - 1)
        {
            board[r + 1][c].orbs++;
            board[r + 1][c].owner = explodingPlayer;
            if (board[r + 1][c].orbs >= getCriticalMass(r + 1, c) && !inQueue[r + 1][c])
            {
                explosionQueue.push({r + 1, c});
                inQueue[r + 1][c] = true;
            }
        }
        if (c > 0)
        {
            board[r][c - 1].orbs++;
            board[r][c - 1].owner = explodingPlayer;
            if (board[r][c - 1].orbs >= getCriticalMass(r, c - 1) && !inQueue[r][c - 1])
            {
                explosionQueue.push({r, c - 1});
                inQueue[r][c - 1] = true;
            }
        }
        if (c < COLS - 1)
        {
            board[r][c + 1].orbs++;
            board[r][c + 1].owner = explodingPlayer;
            if (board[r][c + 1].orbs >= getCriticalMass(r, c + 1) && !inQueue[r][c + 1])
            {
                explosionQueue.push({r, c + 1});
                inQueue[r][c + 1] = true;
            }
        }

        // Check if the current cell is still critical after explosion
        if (board[r][c].orbs >= getCriticalMass(r, c) && !inQueue[r][c])
        {
            explosionQueue.push({r, c});
            inQueue[r][c] = true;
        }

        // Check if game ended during explosion
        if (!gameEndedDuringExplosion && checkGameEndDuringExplosion())
        {
            gameEndedDuringExplosion = true;
            stabilizingExplosions = 0; // Start counting stabilizing explosions
        }

        // If game has ended, continue explosions for stabilization but with a limit
        if (gameEndedDuringExplosion)
        {
            stabilizingExplosions++;
            if (stabilizingExplosions >= MAX_STABILIZING_EXPLOSIONS)
            {
                // Force stabilization by reducing all super-critical cells
                for (int i = 0; i < ROWS; i++)
                {
                    for (int j = 0; j < COLS; j++)
                    {
                        int critMass = getCriticalMass(i, j);
                        if (board[i][j].orbs >= critMass)
                        {
                            board[i][j].orbs = critMass - 1;
                        }
                    }
                }
                return; // Stop explosions
            }
        }
    }
}

void GameState::checkGameEnd()
{
    bool redHasOrbs = false;
    bool blueHasOrbs = false;
    int totalMoves = 0;

    // Count orbs and total moves
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (board[i][j].orbs > 0)
            {
                totalMoves += board[i][j].orbs;
                if (board[i][j].owner == RED)
                {
                    redHasOrbs = true;
                }
                else
                {
                    blueHasOrbs = true;
                }
            }
        }
    }

    // Only check for winner if both players have made moves (at least 2 total moves)
    if (totalMoves >= 2)
    {
        if (!redHasOrbs && blueHasOrbs)
        {
            gameOver = true;
            winner = BLUE;
        }
        else if (!blueHasOrbs && redHasOrbs)
        {
            gameOver = true;
            winner = RED;
        }
    }
}

bool GameState::loadFromFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
        return false;

    string header;
    getline(file, header);

    // Parse header to determine current player
    if (header.find("Human Move:") != string::npos)
    {
        currentPlayer = BLUE; // AI's turn after human move
    }
    else if (header.find("AI Move:") != string::npos)
    {
        currentPlayer = RED; // Human's turn after AI move
    }

    // Read board state
    for (int i = 0; i < ROWS; i++)
    {
        string line;
        getline(file, line);
        istringstream iss(line);

        for (int j = 0; j < COLS; j++)
        {
            string cellStr;
            iss >> cellStr;

            if (cellStr == "0")
            {
                board[i][j] = Cell(0, RED);
            }
            else
            {
                int orbs = cellStr[0] - '0';
                Player owner = (cellStr[1] == 'R') ? RED : BLUE;
                board[i][j] = Cell(orbs, owner);
            }
        }
    }

    file.close();
    return true;
}

bool GameState::saveToFile(const string &filename, const string &moveType) const
{
    ofstream file(filename);
    if (!file.is_open())
        return false;

    // Write header
    file << moveType << ":\n";

    // Write board state
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (board[i][j].orbs == 0)
            {
                file << "0";
            }
            else
            {
                file << board[i][j].orbs;
                file << (board[i][j].owner == RED ? 'R' : 'B');
            }

            if (j < COLS - 1)
                file << " ";
        }
        file << "\n";
    }

    file.close();
    return true;
}

GameState GameState::copy() const
{
    GameState newState;
    newState.board = this->board;
    newState.currentPlayer = this->currentPlayer;
    newState.gameOver = this->gameOver;
    newState.winner = this->winner;
    return newState;
}

// Getter and setter methods
bool GameState::isGameOver() const { return gameOver; }
Player GameState::getWinner() const { return winner; }
Player GameState::getCurrentPlayer() const { return currentPlayer; }
void GameState::setCurrentPlayer(Player player) { currentPlayer = player; }

const Cell &GameState::getCell(int row, int col) const
{
    return board[row][col];
}

void GameState::setCell(int row, int col, int orbs, Player owner)
{
    board[row][col].orbs = orbs;
    board[row][col].owner = owner;
}

void GameState::printBoard() const
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (board[i][j].orbs == 0)
            {
                cout << "0 ";
            }
            else
            {
                cout << board[i][j].orbs;
                cout << (board[i][j].owner == RED ? 'R' : 'B') << " ";
            }
        }
        cout << endl;
    }
}