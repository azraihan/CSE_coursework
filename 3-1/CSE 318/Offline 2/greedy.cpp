#include "greedy.h"

using namespace std;

GreedyResult greedyMaxCut(const Graph& graph) {
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
    set<int> unassigned;
    for (int i = 1; i <= graph.getNumVertices(); ++i) {
        if (i != u && i != v) {
            unassigned.insert(i);
        }
    }
    
    // Assign each unassigned vertex to the partition that maximizes the cut
    for (int z : unassigned) {
        // Calculate sigma values for both partitions
        auto sigmas = graph.calculateSigmaValues(setX, setY, z);
        int sigmaX = sigmas.first;
        int sigmaY = sigmas.second;
        
        // Calculate contribution to cut for both partitions
        // If vertex is in X, edges to Y contribute positively to cut
        int cutContribX = sigmaY - sigmaX;
        // If vertex is in Y, edges to X contribute positively to cut
        int cutContribY = sigmaX - sigmaY;
        
        // Place z in the partition that maximizes the cut
        if (cutContribX > cutContribY) {
            setX.insert(z);
        } else {
            setY.insert(z);
        }
    }
    
    // Calculate final cut weight
    int cutWeight = graph.calculateCutWeight(setX);
    
    return {
        cutWeight,
        {setX, setY}
    };
}