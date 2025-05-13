#include "grasp.h"
#include "semi_greedy.h"
#include "local_search.h"
#include <limits>

using namespace std;

GraspResult grasp(const Graph& graph, int maxIterations, double alpha) {
    int bestValue = numeric_limits<int>::min();
    pair<set<int>, set<int>> bestSolution;
    
    for (int i = 0; i < maxIterations; ++i) {
        // Construction phase: Build a greedy randomized solution
        SemiGreedyResult constructionResult = semiGreedyMaxCut(graph, alpha);
        auto initialPartition = constructionResult.partition;
        
        // Local search phase: Improve the solution
        LocalSearchResult searchResult = localSearch(graph, &initialPartition);
        int currentValue = searchResult.value;
        auto currentPartition = searchResult.partition;
        
        // Update best solution
        if (i == 0 || currentValue > bestValue) {
            bestValue = currentValue;
            bestSolution = currentPartition;
        }
    }
    
    return {
        bestValue,
        bestSolution,
        maxIterations,
        alpha
    };
}