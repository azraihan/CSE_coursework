#include "minimax.h"
#include <iostream>
#include <algorithm>
#include <chrono>

using namespace std;

MinimaxAgent::MinimaxAgent(int depth, int heuristicLevel, Player player, int timeLimitMs)
    : maxDepth(depth), heuristicLevel(heuristicLevel), aiPlayer(player), timeLimitMs(timeLimitMs), nodesEvaluated(0) {
    // Reduce depth for faster response
    if (depth > 4) {
        maxDepth = 4; // Cap at depth 4 for reasonable response time
    }
    setHeuristic(heuristicLevel);
}

void MinimaxAgent::setHeuristic(int level) {
    this->heuristicLevel = level;
    if (level >= 1 && level <= 7) {
        heuristicFunc = HEURISTIC_FUNCTIONS[level - 1];
    } else {
        heuristicFunc = HEURISTIC_FUNCTIONS[4]; // Default to level 5 (faster than 6)
    }
}

void MinimaxAgent::setDepth(int depth) {
    // Cap depth for performance
    maxDepth = min(depth, 4);
}

void MinimaxAgent::setPlayer(Player player) {
    aiPlayer = player;
}

void MinimaxAgent::setTimeLimit(int timeLimitMs) {
    this->timeLimitMs = timeLimitMs;
}

vector<Move> MinimaxAgent::orderMoves(const vector<Move>& moves, const GameState& state) {
    // Simplified move ordering for speed
    vector<pair<Move, int>> scoredMoves;

    for (const Move& move : moves) {
        int score = 0;

        // Quick scoring based on position only
        int criticalMass = state.getCriticalMass(move.row, move.col);
        if (criticalMass == 2) score = 30;      // Corner
        else if (criticalMass == 3) score = 20; // Edge
        else score = 10;                        // Center

        // Small bonus for cells that will become critical
        const Cell& cell = state.getCell(move.row, move.col);
        if (cell.orbs == criticalMass - 2) {
            score += 15;
        }

        scoredMoves.push_back({move, score});
    }

    // Sort by score (highest first)
    sort(scoredMoves.begin(), scoredMoves.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    vector<Move> orderedMoves;
    orderedMoves.reserve(scoredMoves.size());
    for (const auto& scoredMove : scoredMoves) {
        orderedMoves.push_back(scoredMove.first);
    }

    return orderedMoves;
}

int MinimaxAgent::evaluateMove(const Move& move, const GameState& state) {
    // Simplified for speed
    int criticalMass = state.getCriticalMass(move.row, move.col);
    return (criticalMass == 2) ? 30 : ((criticalMass == 3) ? 20 : 10);
}

MinimaxResult MinimaxAgent::minimaxSearch(GameState& state, int depthLimit) {
    nodesEvaluated = 0;
    MinimaxResult result;

    auto startTime = chrono::high_resolution_clock::now();

    // Get all legal moves
    vector<Move> legalMoves = state.getLegalMoves();

    if (legalMoves.empty()) {
        return result;
    }

    // Limit move ordering to reduce overhead
    if (legalMoves.size() > 15) {
        // Only order top moves when there are many options
        legalMoves = orderMoves(legalMoves, state);
        // Consider only top 20 moves max
        if (legalMoves.size() > 20) {
            legalMoves.resize(20);
        }
    }

    int bestValue = INT_MIN;
    Move bestMove = legalMoves[0];

    // Use iterative deepening for time management
    for (int depth = 1; depth <= min(depthLimit, maxDepth); depth++) {
        int currentBest = INT_MIN;
        Move currentBestMove = legalMoves[0];

        for (const Move& move : legalMoves) {
            GameState newState = state.copy();
            newState.makeMove(move);

            int value = minimax(newState, depth - 1, INT_MIN, INT_MAX, false);

            if (value > currentBest) {
                currentBest = value;
                currentBestMove = move;
            }

            // Time check - use configurable time limit
            auto currentTime = chrono::high_resolution_clock::now();
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime);
            if (elapsed.count() > timeLimitMs) {
                cout << "Time limit (" << timeLimitMs << "ms) reached, using depth " << depth << endl;
                bestValue = currentBest;
                bestMove = currentBestMove;
                goto search_complete;
            }
        }

        bestValue = currentBest;
        bestMove = currentBestMove;

        // If we find a very good move early, stop searching
        if (bestValue >= 1000) break;
    }

    search_complete:
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

    cout << "Search completed in " << duration.count() << "ms, evaluated "
              << nodesEvaluated << " nodes" << endl;

    result.value = bestValue;
    result.bestMove = bestMove;
    result.nodesEvaluated = nodesEvaluated;
    return result;
}

int MinimaxAgent::minimax(GameState& state, int depth, int alpha, int beta, bool maximizing) {
    nodesEvaluated++;

    // Terminal conditions
    if (state.isGameOver()) {
        return evaluateTerminal(state);
    }

    if (depth <= 0) {
        return heuristicFunc(state, aiPlayer);
    }

    vector<Move> legalMoves = state.getLegalMoves();

    if (legalMoves.empty()) {
        return heuristicFunc(state, aiPlayer);
    }

    // Limit branching factor for performance
    if (legalMoves.size() > 12 && depth > 1) {
        // Quick sort and take best moves only
        sort(legalMoves.begin(), legalMoves.end(), [&](const Move& a, const Move& b) {
            return evaluateMove(a, state) > evaluateMove(b, state);
        });
        legalMoves.resize(12); // Consider only top 12 moves
    }

    if (maximizing) {
        int maxEval = INT_MIN;

        for (const Move& move : legalMoves) {
            GameState newState = state.copy();
            newState.makeMove(move);

            int eval = minimax(newState, depth - 1, alpha, beta, false);
            maxEval = max(maxEval, eval);

            alpha = max(alpha, eval);
            if (beta <= alpha) {
                break; // Beta cutoff
            }
        }

        return maxEval;
    } else {
        int minEval = INT_MAX;

        for (const Move& move : legalMoves) {
            GameState newState = state.copy();
            newState.makeMove(move);

            int eval = minimax(newState, depth - 1, alpha, beta, true);
            minEval = min(minEval, eval);

            beta = min(beta, eval);
            if (beta <= alpha) {
                break; // Alpha cutoff
            }
        }

        return minEval;
    }
}

int MinimaxAgent::quiescenceSearch(GameState& state, int alpha, int beta, bool maximizing, int qDepth) {
    // Simplified quiescence search
    nodesEvaluated++;

    if (qDepth <= 0) {
        return heuristicFunc(state, aiPlayer);
    }

    int standPat = heuristicFunc(state, aiPlayer);

    if (maximizing) {
        if (standPat >= beta) return beta;
        alpha = max(alpha, standPat);
    } else {
        if (standPat <= alpha) return alpha;
        beta = min(beta, standPat);
    }

    // Only check truly critical moves
    vector<Move> criticalMoves;
    vector<Move> allMoves = state.getLegalMoves();

    for (const Move& move : allMoves) {
        const Cell& cell = state.getCell(move.row, move.col);
        int criticalMass = state.getCriticalMass(move.row, move.col);

        if (cell.orbs == criticalMass - 1) {
            criticalMoves.push_back(move);
        }
    }

    if (criticalMoves.empty() || criticalMoves.size() > 5) {
        return standPat;
    }

    for (const Move& move : criticalMoves) {
        GameState newState = state.copy();
        newState.makeMove(move);

        int score = quiescenceSearch(newState, alpha, beta, !maximizing, qDepth - 1);

        if (maximizing) {
            standPat = max(standPat, score);
            alpha = max(alpha, score);
            if (beta <= alpha) break;
        } else {
            standPat = min(standPat, score);
            beta = min(beta, score);
            if (beta <= alpha) break;
        }
    }

    return standPat;
}

bool MinimaxAgent::isQuietPosition(const GameState& state) {
    // Quick check for critical cells
    int criticalCount = 0;
    for (int i = 0; i < ROWS && criticalCount < 3; i++) {
        for (int j = 0; j < COLS && criticalCount < 3; j++) {
            const Cell& cell = state.getCell(i, j);
            if (cell.orbs > 0 && cell.orbs >= state.getCriticalMass(i, j) - 1) {
                criticalCount++;
            }
        }
    }
    return criticalCount < 3;
}

int MinimaxAgent::evaluateTerminal(const GameState& state) const {
    if (!state.isGameOver()) {
        return 0;
    }

    Player winner = state.getWinner();

    if (winner == aiPlayer) {
        return 10000;
    } else {
        return -10000;
    }
}

Move MinimaxAgent::getBestMove(GameState& state) {
    MinimaxResult result = minimaxSearch(state, maxDepth);
    return result.bestMove;
}