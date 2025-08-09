#include "game.h"
#include "minimax.h"
#include "heuristics.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

const string GAME_FILE = "../game-info/gamestate.txt";

struct AIConfig {
    Player player;
    int depth;
    int heuristic;
    int timeLimit;
    
    AIConfig() : player(BLUE), depth(3), heuristic(5), timeLimit(60000) {}
};

AIConfig parseArguments(int argc, char* argv[]) {
    AIConfig config;
    
    // Parse arguments: ./ai_engine [PLAYER] [DEPTH] [HEURISTIC] [TIME_LIMIT]
    if (argc >= 2) {
        string playerArg = argv[1];
        if (playerArg == "RED" || playerArg == "red") {
            config.player = RED;
        } else if (playerArg == "BLUE" || playerArg == "blue") {
            config.player = BLUE;
        }
    }
    
    if (argc >= 3) {
        config.depth = max(1, min(6, atoi(argv[2]))); // Clamp between 1-6
    }
    
    if (argc >= 4) {
        config.heuristic = max(1, min(7, atoi(argv[3]))); // Clamp between 1-7
    }
    
    if (argc >= 5) {
        config.timeLimit = max(1000, atoi(argv[4])); // Minimum 1 second
    }
    
    return config;
}

string getPlayerName(Player player) {
    return (player == RED) ? "Red" : "Blue";
}

string getWaitingFor(Player player) {
    if (player == RED) {
        return "AI Move:"; // Red AI waits for Blue AI's move
    } else {
        return "Human Move:"; // Blue AI waits for Human/Red AI's move  
    }
}

string getSaveAs(Player player) {
    if (player == RED) {
        return "Human Move"; // Red AI saves as "Human Move" so Blue responds
    } else {
        return "AI Move"; // Blue AI saves as "AI Move" so Red/Human responds
    }
}

int main(int argc, char* argv[]) {
    // Parse configuration from command line
    AIConfig config = parseArguments(argc, argv);
    string playerName = getPlayerName(config.player);
    
    GameState gameState;
    MinimaxAgent aiAgent(config.depth, config.heuristic, config.player, config.timeLimit);
    
    cout << playerName << " AI Engine started. Using depth " << config.depth 
         << ", heuristic " << config.heuristic 
         << ", time limit " << config.timeLimit << "ms" << endl;
    cout << "Waiting for moves..." << endl;
    
    string waitingFor = getWaitingFor(config.player);
    string saveAs = getSaveAs(config.player);
    
    while (true) {
        // Try to load game state from file
        if (gameState.loadFromFile(GAME_FILE)) {
            
            // Check if it's this AI's turn
            ifstream file(GAME_FILE);
            string header;
            getline(file, header);
            file.close();
            
            if (header.find(waitingFor) != string::npos) {
                cout << playerName << " AI processing move..." << endl;
                
                // Check if game is over
                if (gameState.isGameOver()) {
                    cout << "Game Over! Winner: " 
                              << (gameState.getWinner() == RED ? "Red" : "Blue") 
                              << endl;
                    return 0;
                    // break;
                }
                
                // Set current player
                gameState.setCurrentPlayer(config.player);
                
                // Check if there are any valid moves
                vector<Move> legalMoves = gameState.getLegalMoves();
                if (legalMoves.empty()) {
                    cout << "No valid moves available for " << playerName << " AI!" << endl;
                    break;
                }
                
                // Get best move from AI with timing
                auto startTime = chrono::high_resolution_clock::now();
                Move aiMove = aiAgent.getBestMove(gameState);
                auto endTime = chrono::high_resolution_clock::now();
                
                auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
                
                // Check if time limit was hit during search
                bool hitTimeLimit = (duration.count() >= config.timeLimit);
                if (hitTimeLimit) {
                    cout << playerName << " AI hit time limit!" << endl;
                }
                
                if (aiMove.isValid()) {
                    cout << playerName << " AI making move at (" << aiMove.row << ", " << aiMove.col << ") ";
                    cout << "in " << duration.count() << "ms" << endl;
                    
                    // Make the move
                    gameState.makeMove(aiMove);
                    
                    // Save the updated state
                    gameState.saveToFile(GAME_FILE, saveAs);
                    
                    cout << playerName << " AI move saved to file." << endl;
                    
                    // Check if game ended after AI move
                    if (gameState.isGameOver()) {
                        cout << "Game Over! Winner: " 
                                  << (gameState.getWinner() == RED ? "Red" : "Blue") 
                                  << endl;
                        return 0;
                        // break;
                    }
                } else {
                    cout << playerName << " AI could not find a valid move!" << endl;
                    break;
                }
            }
        }
        
        // Reduced wait time for faster response
        this_thread::sleep_for(chrono::milliseconds(250));
    }
    
    return 0;
}