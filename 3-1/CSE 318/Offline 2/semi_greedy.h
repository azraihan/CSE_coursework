#ifndef SEMI_GREEDY_H
#define SEMI_GREEDY_H

#include "graph.h"
#include <set>
#include <utility>

using namespace std;

struct SemiGreedyResult {
    int value;               // Cut weight
    pair<set<int>, set<int>> partition; // Partition (X, Y)
    double alpha;            // Alpha parameter used
};

SemiGreedyResult semiGreedyMaxCut(const Graph& graph, double alpha = 0.5);

#endif 