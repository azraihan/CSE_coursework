#ifndef RANDOMIZED_H
#define RANDOMIZED_H

#include "graph.h"
#include <map>
#include <set>
#include <utility>

using namespace std;

struct RandomizedResult {
    int value;               // Best cut weight
    double averageValue;     // Average cut weight
    pair<set<int>, set<int>> partition; // Best partition (X, Y)
};

RandomizedResult randomizedMaxCut(const Graph& graph, int n = 10);

#endif // RANDOMIZED_H