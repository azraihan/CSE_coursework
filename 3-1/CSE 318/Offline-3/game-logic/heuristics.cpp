#include "heuristics.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Heuristic function array
HeuristicFunction HEURISTIC_FUNCTIONS[] = {
    &HeuristicEvaluator::evaluateHeuristic1,
    &HeuristicEvaluator::evaluateHeuristic2,
    &HeuristicEvaluator::evaluateHeuristic3,
    &HeuristicEvaluator::evaluateHeuristic4,
    &HeuristicEvaluator::evaluateHeuristic5,
    &HeuristicEvaluator::evaluateAdvancedStrategy,
    &HeuristicEvaluator::evaluateRandomHeuristic
};

int HeuristicEvaluator::getCriticalMass(int row, int col) {
    // Corner cells
    if ((row == 0 || row == ROWS - 1) && (col == 0 || col == COLS - 1)) {
        return 2;
    }
    // Edge cells
    else if (row == 0 || row == ROWS - 1 || col == 0 || col == COLS - 1) {
        return 3;
    }
    // Center cells
    else {
        return 4;
    }
}

int HeuristicEvaluator::getPositionalWeight(int row, int col) {
    int criticalMass = getCriticalMass(row, col);
    
    if (criticalMass == 2) return CORNER_WEIGHT;      // Corners
    else if (criticalMass == 3) return EDGE_WEIGHT;   // Edges
    else return CENTER_WEIGHT;                        // Center
}

std::vector<std::pair<int, int>> HeuristicEvaluator::getNeighbors(int row, int col) {
    std::vector<std::pair<int, int>> neighbors;
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    
    for (int i = 0; i < 4; i++) {
        int newRow = row + dr[i];
        int newCol = col + dc[i];
        
        if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS) {
            neighbors.push_back({newRow, newCol});
        }
    }
    
    return neighbors;
}

bool HeuristicEvaluator::isCriticalCell(const GameState& state, int row, int col) {
    const Cell& cell = state.getCell(row, col);
    if (cell.orbs == 0) return false;
    
    return cell.orbs >= getCriticalMass(row, col) - 1;
}

bool HeuristicEvaluator::isNearCritical(const GameState& state, int row, int col) {
    const Cell& cell = state.getCell(row, col);
    if (cell.orbs == 0) return false;
    
    return cell.orbs >= getCriticalMass(row, col) - 2;
}

bool HeuristicEvaluator::isEarlyGame(const GameState& state) {
    int totalOrbs = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            totalOrbs += state.getCell(i, j).orbs;
        }
    }
    return totalOrbs < 8;
}

bool HeuristicEvaluator::isEndGame(const GameState& state) {
    int totalOrbs = 0;
    int occupiedCells = 0;
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int orbs = state.getCell(i, j).orbs;
            if (orbs > 0) {
                totalOrbs += orbs;
                occupiedCells++;
            }
        }
    }
    
    return totalOrbs > 20 || occupiedCells > 15;
}

// Heuristic 1: Simple Orb Difference (Optimized)
int HeuristicEvaluator::evaluateHeuristic1(const GameState& state, Player maximizingPlayer) {
    int maxOrbs = 0, minOrbs = 0;
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            const Cell& cell = state.getCell(i, j);
            if (cell.orbs > 0) {
                if (cell.owner == maximizingPlayer) {
                    maxOrbs += cell.orbs;
                } else {
                    minOrbs += cell.orbs;
                }
            }
        }
    }
    
    return maxOrbs - minOrbs;
}

// Heuristic 2: Positional Advantage (Optimized)
int HeuristicEvaluator::evaluateHeuristic2(const GameState& state, Player maximizingPlayer) {
    int maxScore = 0, minScore = 0;
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            const Cell& cell = state.getCell(i, j);
            if (cell.orbs > 0) {
                int weight = getPositionalWeight(i, j);
                int cellValue = weight * cell.orbs;
                
                if (cell.owner == maximizingPlayer) {
                    maxScore += cellValue;
                } else {
                    minScore += cellValue;
                }
            }
        }
    }
    
    return maxScore - minScore;
}

// Heuristic 3: Critical Cell Count (Optimized)
int HeuristicEvaluator::evaluateHeuristic3(const GameState& state, Player maximizingPlayer) {
    int maxCritical = 0, minCritical = 0;
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            const Cell& cell = state.getCell(i, j);
            if (cell.orbs > 0) {
                int criticalMass = getCriticalMass(i, j);
                
                if (cell.orbs == criticalMass - 1) {
                    if (cell.owner == maximizingPlayer) {
                        maxCritical++;
                    } else {
                        minCritical++;
                    }
                }
            }
        }
    }
    
    return (maxCritical - minCritical) * CRITICAL_BONUS;
}

// Heuristic 4: Advanced Threat Analysis (Simplified for speed)
int HeuristicEvaluator::evaluateHeuristic4(const GameState& state, Player maximizingPlayer) {
    int maxScore = 0, minScore = 0;
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            const Cell& cell = state.getCell(i, j);
            if (cell.orbs > 0) {
                int criticalMass = getCriticalMass(i, j);
                
                if (cell.orbs == criticalMass - 1) {
                    int cellScore = CRITICAL_BONUS;
                    
                    // Quick threat check - count adjacent opponent orbs
                    int threatCount = 0;
                    if (i > 0 && state.getCell(i-1, j).orbs > 0 && state.getCell(i-1, j).owner != cell.owner) threatCount++;
                    if (i < ROWS-1 && state.getCell(i+1, j).orbs > 0 && state.getCell(i+1, j).owner != cell.owner) threatCount++;
                    if (j > 0 && state.getCell(i, j-1).orbs > 0 && state.getCell(i, j-1).owner != cell.owner) threatCount++;
                    if (j < COLS-1 && state.getCell(i, j+1).orbs > 0 && state.getCell(i, j+1).owner != cell.owner) threatCount++;
                    
                    cellScore -= threatCount * THREAT_PENALTY;
                    
                    if (cell.owner == maximizingPlayer) {
                        maxScore += cellScore;
                    } else {
                        minScore += cellScore;
                    }
                }
            }
        }
    }
    
    return maxScore - minScore;
}

// Heuristic 5: Combined Strategy (Optimized weights)
int HeuristicEvaluator::evaluateHeuristic5(const GameState& state, Player maximizingPlayer) {
    const double wPos = 0.4;    // Weight for positional advantage
    const double wCell = 0.3;   // Weight for cell count
    const double wCrit = 0.3;   // Weight for critical analysis
    
    int positionalScore = evaluateHeuristic2(state, maximizingPlayer);
    int cellCount = evaluateHeuristic1(state, maximizingPlayer);
    int criticalScore = evaluateHeuristic4(state, maximizingPlayer);
    
    return (int)(wPos * positionalScore + wCell * cellCount + wCrit * criticalScore);
}

// Simplified Advanced Strategy for better performance
int HeuristicEvaluator::evaluateAdvancedStrategy(const GameState& state, Player maximizingPlayer) {
    int score = 0;
    
    // Base evaluation
    score += evaluateHeuristic2(state, maximizingPlayer) * 0.4;
    score += evaluateHeuristic1(state, maximizingPlayer) * 0.3;
    
    // Quick territory control bonus
    int territoryBonus = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            const Cell& cell = state.getCell(i, j);
            if (cell.orbs > 0 && cell.owner == maximizingPlayer) {
                // Count friendly neighbors for connectivity
                int friendlyNeighbors = 0;
                if (i > 0 && state.getCell(i-1, j).orbs > 0 && state.getCell(i-1, j).owner == maximizingPlayer) friendlyNeighbors++;
                if (i < ROWS-1 && state.getCell(i+1, j).orbs > 0 && state.getCell(i+1, j).owner == maximizingPlayer) friendlyNeighbors++;
                if (j > 0 && state.getCell(i, j-1).orbs > 0 && state.getCell(i, j-1).owner == maximizingPlayer) friendlyNeighbors++;
                if (j < COLS-1 && state.getCell(i, j+1).orbs > 0 && state.getCell(i, j+1).owner == maximizingPlayer) friendlyNeighbors++;
                
                territoryBonus += friendlyNeighbors;
            }
        }
    }
    
    score += territoryBonus * CONNECTIVITY_BONUS * 0.3;
    
    // Corner control bonus (simplified)
    if (isEarlyGame(state)) {
        for (int i = 0; i < ROWS; i += ROWS - 1) {
            for (int j = 0; j < COLS; j += COLS - 1) {
                const Cell& cell = state.getCell(i, j);
                if (cell.orbs > 0) {
                    int multiplier = (cell.owner == maximizingPlayer) ? 1 : -1;
                    score += multiplier * 10;
                }
            }
        }
    }
    
    return score;
}

// Random Heuristic for experimentation
int HeuristicEvaluator::evaluateRandomHeuristic(const GameState& state, Player maximizingPlayer) {
    // Initialize random seed if not already done
    static bool seedInitialized = false;
    if (!seedInitialized) {
        srand(static_cast<unsigned int>(time(nullptr)));
        seedInitialized = true;
    }
    
    // Return a random value between -1000 and 1000
    // This effectively makes the AI choose moves randomly
    return (rand() % 2001) - 1000;
}

// Simplified utility functions for performance
int HeuristicEvaluator::analyzeThreatLevel(const GameState& state, int row, int col, Player player) {
    int threatLevel = 0;
    
    // Quick neighbor check
    if (row > 0) {
        const Cell& cell = state.getCell(row-1, col);
        if (cell.orbs > 0 && cell.owner != player && isCriticalCell(state, row-1, col)) threatLevel += 2;
    }
    if (row < ROWS-1) {
        const Cell& cell = state.getCell(row+1, col);
        if (cell.orbs > 0 && cell.owner != player && isCriticalCell(state, row+1, col)) threatLevel += 2;
    }
    if (col > 0) {
        const Cell& cell = state.getCell(row, col-1);
        if (cell.orbs > 0 && cell.owner != player && isCriticalCell(state, row, col-1)) threatLevel += 2;
    }
    if (col < COLS-1) {
        const Cell& cell = state.getCell(row, col+1);
        if (cell.orbs > 0 && cell.owner != player && isCriticalCell(state, row, col+1)) threatLevel += 2;
    }
    
    return threatLevel;
}

int HeuristicEvaluator::countChainPotential(const GameState& state, int row, int col, Player player) {
    int potential = 0;
    
    // Simplified chain potential calculation
    if (row > 0) {
        const Cell& cell = state.getCell(row-1, col);
        if (cell.orbs > 0) {
            potential += (cell.owner == player) ? 1 : cell.orbs;
        }
    }
    if (row < ROWS-1) {
        const Cell& cell = state.getCell(row+1, col);
        if (cell.orbs > 0) {
            potential += (cell.owner == player) ? 1 : cell.orbs;
        }
    }
    if (col > 0) {
        const Cell& cell = state.getCell(row, col-1);
        if (cell.orbs > 0) {
            potential += (cell.owner == player) ? 1 : cell.orbs;
        }
    }
    if (col < COLS-1) {
        const Cell& cell = state.getCell(row, col+1);
        if (cell.orbs > 0) {
            potential += (cell.owner == player) ? 1 : cell.orbs;
        }
    }
    
    return potential;
}

int HeuristicEvaluator::evaluateTerritoryControl(const GameState& state, Player player) {
    int territoryScore = 0;
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            const Cell& cell = state.getCell(i, j);
            if (cell.orbs > 0 && cell.owner == player) {
                // Quick friendly neighbor count
                int friendlyNeighbors = 0;
                if (i > 0 && state.getCell(i-1, j).orbs > 0 && state.getCell(i-1, j).owner == player) friendlyNeighbors++;
                if (i < ROWS-1 && state.getCell(i+1, j).orbs > 0 && state.getCell(i+1, j).owner == player) friendlyNeighbors++;
                if (j > 0 && state.getCell(i, j-1).orbs > 0 && state.getCell(i, j-1).owner == player) friendlyNeighbors++;
                if (j < COLS-1 && state.getCell(i, j+1).orbs > 0 && state.getCell(i, j+1).owner == player) friendlyNeighbors++;
                
                territoryScore += friendlyNeighbors;
            }
        }
    }
    
    return territoryScore;
}

int HeuristicEvaluator::evaluateConnectivity(const GameState& state, Player player) {
    return evaluateTerritoryControl(state, player) / 2; // Simplified
}