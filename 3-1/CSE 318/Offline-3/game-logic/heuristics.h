#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "game.h"

// Forward declaration
class GameState;

class HeuristicEvaluator {
public:
    // Heuristic evaluation functions
    static int evaluateHeuristic1(const GameState& state, Player maximizingPlayer);
    static int evaluateHeuristic2(const GameState& state, Player maximizingPlayer);
    static int evaluateHeuristic3(const GameState& state, Player maximizingPlayer);
    static int evaluateHeuristic4(const GameState& state, Player maximizingPlayer);
    static int evaluateHeuristic5(const GameState& state, Player maximizingPlayer);
    
    // Advanced heuristics for better play
    static int evaluateAdvancedStrategy(const GameState& state, Player maximizingPlayer);
    
    // Random heuristic for experiments
    static int evaluateRandomHeuristic(const GameState& state, Player maximizingPlayer);
    
    // Utility functions
    static int getPositionalWeight(int row, int col);
    static int getCriticalMass(int row, int col);
    static std::vector<std::pair<int, int>> getNeighbors(int row, int col);
    static bool isCriticalCell(const GameState& state, int row, int col);
    static bool isNearCritical(const GameState& state, int row, int col);
    
    // Game phase detection
    static bool isEarlyGame(const GameState& state);
    static bool isEndGame(const GameState& state);
    
    // Threat analysis
    static int analyzeThreatLevel(const GameState& state, int row, int col, Player player);
    static int countChainPotential(const GameState& state, int row, int col, Player player);
    
    // Territory control
    static int evaluateTerritoryControl(const GameState& state, Player player);
    static int evaluateConnectivity(const GameState& state, Player player);

private:
    // Constants for tuning
    static const int CORNER_WEIGHT = 5;
    static const int EDGE_WEIGHT = 3;
    static const int CENTER_WEIGHT = 1;
    static const int CRITICAL_BONUS = 8;
    static const int THREAT_PENALTY = 4;
    static const int CHAIN_BONUS = 6;
    static const int CONNECTIVITY_BONUS = 2;
};

// Heuristic function pointer type
typedef int (*HeuristicFunction)(const GameState&, Player);

// Array of heuristic functions for easy access
extern HeuristicFunction HEURISTIC_FUNCTIONS[];

#endif // HEURISTICS_H