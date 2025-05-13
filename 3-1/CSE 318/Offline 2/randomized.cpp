#include "randomized.h"
#include <random>
#include <chrono>

using namespace std;

RandomizedResult randomizedMaxCut(const Graph& graph, int n) {
    int totalCutWeight = 0;
    int bestCutWeight = numeric_limits<int>::min();
    pair<set<int>, set<int>> bestPartition;
    
    // Create random number generator
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_real_distribution<double> distribution(0.0, 1.0);
    
    for (int i = 0; i < n; ++i) {
        // Initialize partitions
        set<int> setX;
        set<int> setY;
        
        // Assign each vertex to either X or Y with probability 1/2
        for (int v = 1; v <= graph.getNumVertices(); ++v) {
            if (distribution(generator) >= 0.5) {
                setX.insert(v);
            } else {
                setY.insert(v);
            }
        }
        
        // Calculate cut weight
        int cutWeight = 0;
        for (int u : setX) {
            for (int v : setY) {
                cutWeight += graph.getEdgeWeight(u, v);
            }
        }
        
        // Update total and best cut weight
        totalCutWeight += cutWeight;
        
        if (i == 0 || cutWeight > bestCutWeight) {
            bestCutWeight = cutWeight;
            bestPartition = {setX, setY};
        }
    }
    
    double averageCutWeight = static_cast<double>(totalCutWeight) / n;
    
    return {
        bestCutWeight,
        averageCutWeight,
        bestPartition
    };
}