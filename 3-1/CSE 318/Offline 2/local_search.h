#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "graph.h"
#include <set>
#include <utility>
#include <unordered_map>

using namespace std;

struct LocalSearchResult {
    int value;               // Best cut weight
    double averageValue;     // Average cut weight across all runs
    pair<set<int>, set<int>> partition; // Partition (S, S̄) with best cut weight
};

LocalSearchResult localSearch(
    const Graph& graph, 
    const pair<set<int>, set<int>>* initialPartition = nullptr,
    int n_runs = 1);

#endif 