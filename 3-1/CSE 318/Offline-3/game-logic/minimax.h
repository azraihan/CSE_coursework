#ifndef MINIMAX_H
#define MINIMAX_H

#include "game.h"
#include "heuristics.h"
#include <climits>

// Structure to hold minimax result
struct MinimaxResult {
    int value;
    Move bestMove;
    int nodesEvaluated;
    
    MinimaxResult() : value(0), bestMove(), nodesEvaluated(0) {}
    MinimaxResult(int v, Move m) : value(v), bestMove(m), nodesEvaluated(0) {}
};

class MinimaxAgent {
private:
    int maxDepth;
    int heuristicLevel;
    int timeLimitMs;
    HeuristicFunction heuristicFunc;
    Player aiPlayer;
    int nodesEvaluated;
    
    // Move ordering for better pruning
    std::vector<Move> orderMoves(const std::vector<Move>& moves, const GameState& state);
    int evaluateMove(const Move& move, const GameState& state);
    
public:
    MinimaxAgent(int depth = 6, int heuristicLevel = 6, Player player = BLUE, int timeLimitMs = 60000);
    
    // Main minimax search with alpha-beta pruning
    MinimaxResult minimaxSearch(GameState& state, int depthLimit);
    
    // Set configuration
    void setHeuristic(int heuristicLevel);
    void setDepth(int depth);
    void setPlayer(Player player);
    void setTimeLimit(int timeLimitMs);
    
    // Get best move for current state
    Move getBestMove(GameState& state);
    
    // Performance monitoring
    int getNodesEvaluated() const { return nodesEvaluated; }
    void resetStats() { nodesEvaluated = 0; }
    
private:
    // Recursive minimax with alpha-beta pruning
    int minimax(GameState& state, int depth, int alpha, int beta, bool maximizing);
    
    // Evaluate terminal states
    int evaluateTerminal(const GameState& state) const;
    
    // Quiescence search for explosive positions
    int quiescenceSearch(GameState& state, int alpha, int beta, bool maximizing, int qDepth = 3);
    
    // Check if position is "quiet" (no immediate explosions)
    bool isQuietPosition(const GameState& state);
};

#endif // MINIMAX_H