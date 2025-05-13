#include "local_search.h"
#include <limits>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

// Helper function for a single local search run
pair<int, pair<set<int>, set<int>>> singleLocalSearch(
    const Graph& graph, 
    const pair<set<int>, set<int>>* initialPartition) {
    
    // If no initial partition is provided, create a random one
    set<int> setS;
    set<int> sComplement;
    
    if (initialPartition == nullptr) {
        // Create a random number generator for random initial partitioning
        unsigned seed = chrono::system_clock::now().time_since_epoch().count();
        default_random_engine generator(seed);
        uniform_real_distribution<double> distribution(0.0, 1.0);
        
        for (int v = 1; v <= graph.getNumVertices(); ++v) {
            if (distribution(generator) >= 0.5) {
                setS.insert(v);
            } else {
                sComplement.insert(v);
            }
        }
    } else {
        setS = initialPartition->first;
        sComplement = initialPartition->second;
    }
    
    // Calculate initial cut weight
    int cutWeight = graph.calculateCutWeight(setS);
    
    // Store deltas for each vertex to avoid recalculating
    vector<pair<int, int>> vertexDeltas;
    vertexDeltas.reserve(graph.getNumVertices());
    
    bool improved = true;
    while (improved) {
        improved = false;
        vertexDeltas.clear();
        
        // Evaluate all possible moves in a single pass
        for (int vertex = 1; vertex <= graph.getNumVertices(); ++vertex) {
            int delta = graph.calculateDelta(setS, sComplement, vertex);
            
            // Store vertex and its delta
            vertexDeltas.push_back({vertex, delta});
        }
        
        // Sort vertices by delta (descending)
        sort(vertexDeltas.begin(), vertexDeltas.end(), 
                 [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Find the best move
        for (const auto& [vertex, delta] : vertexDeltas) {
            if (delta > 0) {
                // Apply the move
                if (setS.count(vertex) > 0) {
                    setS.erase(vertex);
                    sComplement.insert(vertex);
                } else {
                    sComplement.erase(vertex);
                    setS.insert(vertex);
                }
                
                // Update cut weight
                cutWeight += delta;
                improved = true;
                
                // Break after the first improvement (first-improvement strategy)
                break;
            }
        }
    }
    
    return {cutWeight, {setS, sComplement}};
}

LocalSearchResult localSearch(
    const Graph& graph, 
    const pair<set<int>, set<int>>* initialPartition,
    int n_runs) {
    
    int totalCutWeight = 0;
    int bestCutWeight = numeric_limits<int>::min();
    pair<set<int>, set<int>> bestPartition;
    
    for (int i = 0; i < n_runs; ++i) {
        // Use the provided initial partition only for the first run
        const pair<set<int>, set<int>>* currentPartition = (i == 0) ? initialPartition : nullptr;
        
        // Run single local search
        auto [cutWeight, partition] = singleLocalSearch(graph, currentPartition);
        
        // Update total and check if this is the best solution
        totalCutWeight += cutWeight;
        
        if (cutWeight > bestCutWeight) {
            bestCutWeight = cutWeight;
            bestPartition = partition;
        }
    }
    
    double averageCutWeight = static_cast<double>(totalCutWeight) / n_runs;
    
    return {
        bestCutWeight,
        averageCutWeight,
        bestPartition
    };
}