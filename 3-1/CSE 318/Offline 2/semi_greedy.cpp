#include "semi_greedy.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <limits>
#include <vector>
#include <utility>

using namespace std;

SemiGreedyResult semiGreedyMaxCut(const Graph& graph, double alpha) {
    // Initialize partitions
    set<int> setX;
    set<int> setY;
    
    // Find edge with maximum weight and place its endpoints in different partitions
    auto maxEdge = graph.findMaxWeightEdge();
    int u = maxEdge.first;
    int v = maxEdge.second;
    
    setX.insert(u);
    setY.insert(v);
    
    // Create set of unassigned vertices
    vector<int> unassigned;
    unassigned.reserve(graph.getNumVertices() - 2);
    for (int i = 1; i <= graph.getNumVertices(); ++i) {
        if (i != u && i != v) {
            unassigned.push_back(i);
        }
    }
    
    // Create random number generator
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    
    // Pre-allocate vectors for improved performance
    vector<tuple<int, int, int, int>> vertexValues;
    vertexValues.reserve(graph.getNumVertices());
    vector<int> rcl;
    rcl.reserve(graph.getNumVertices());
    
    while (!unassigned.empty()) {
        vertexValues.clear();
        
        // Calculate the contribution to the cut if vertex is added to X or Y
        for (int vertex : unassigned) {
            auto sigmas = graph.calculateSigmaValues(setX, setY, vertex);
            int sigmaX = sigmas.first;
            int sigmaY = sigmas.second;
            
            // Calculate contribution to cut for both partitions
            // If vertex is in X, edges to Y contribute positively to cut
            int cutContribX = sigmaY - sigmaX;
            // If vertex is in Y, edges to X contribute positively to cut
            int cutContribY = sigmaX - sigmaY;
            
            // Choose the best partition (max contribution to cut)
            int bestContrib = max(cutContribX, cutContribY);
            
            // Store vertex with its values
            vertexValues.push_back(make_tuple(vertex, sigmaX, sigmaY, bestContrib));
        }
        
        // Find minimum and maximum contribution values
        int minValue = numeric_limits<int>::max();
        int maxValue = numeric_limits<int>::min();
        
        for (const auto& entry : vertexValues) {
            int value = get<3>(entry);
            minValue = min(minValue, value);
            maxValue = max(maxValue, value);
        }
        
        // Calculate cut-off value μ
        int mu = minValue + static_cast<int>(alpha * (maxValue - minValue));
        
        // Construct RCL (Restricted Candidate List)
        rcl.clear();
        
        for (const auto& entry : vertexValues) {
            int vertex = get<0>(entry);
            int value = get<3>(entry);
            
            if (value >= mu) {
                rcl.push_back(vertex);
            }
        }
        
        // If RCL is empty (can happen if all values are the same), include all vertices
        if (rcl.empty()) {
            for (int vertex : unassigned) {
                rcl.push_back(vertex);
            }
        }
        
        // Select a random vertex from RCL
        uniform_int_distribution<int> distribution(0, rcl.size() - 1);
        int randomIndex = distribution(generator);
        int selectedVertex = rcl[randomIndex];
        
        // Find the sigma values for the selected vertex
        int sigmaX = 0, sigmaY = 0;
        for (const auto& entry : vertexValues) {
            if (get<0>(entry) == selectedVertex) {
                sigmaX = get<1>(entry);
                sigmaY = get<2>(entry);
                break;
            }
        }
        
        // Decide which partition to place the selected vertex based on cut contribution
        // If adding to X gives better cut, add to X
        if (sigmaY - sigmaX > sigmaX - sigmaY) {
            setX.insert(selectedVertex);
        } else {
            setY.insert(selectedVertex);
        }
        
        // Remove the selected vertex from unassigned
        unassigned.erase(remove(unassigned.begin(), unassigned.end(), selectedVertex), unassigned.end());
    }
    
    // Calculate final cut weight
    int cutWeight = graph.calculateCutWeight(setX);
    
    return {
        cutWeight,
        {setX, setY},
        alpha
    };
}