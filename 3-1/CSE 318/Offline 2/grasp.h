#ifndef GRASP_H
#define GRASP_H

#include "graph.h"
#include <set>
#include <utility>

using namespace std;

struct GraspResult {
    int value;               // Cut weight
    pair<set<int>, set<int>> partition; // Partition (S, S̄)
    int iterations;          // Number of iterations performed
    double alpha;            // Alpha parameter used
};

GraspResult grasp(const Graph& graph, int maxIterations = 10, double alpha = 0.5);

#endif 