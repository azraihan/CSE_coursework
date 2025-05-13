#ifndef GREEDY_H
#define GREEDY_H

#include "graph.h"
#include <set>
#include <utility>

using namespace std;

struct GreedyResult {
    int value;               // Cut weight
    pair<set<int>, set<int>> partition; // Partition (X, Y)
};

GreedyResult greedyMaxCut(const Graph& graph);

#endif // GREEDY_H