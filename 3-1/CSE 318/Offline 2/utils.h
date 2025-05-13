#ifndef UTILS_H
#define UTILS_H

#include "graph.h"
#include <string>
#include <map>
#include <vector>

using namespace std;

extern const map<string, int> KNOWN_BEST_SOLUTIONS;

struct GraphInfo {
    int numVertices;
    int numEdges;
};

Graph loadGraphFromFile(const string& filePath);

map<string, Graph> loadAllGraphs(const string& directory);

void writeResultsToCsv(
    const map<string, map<string, int>>& results,
    const string& outputFile = "results.csv",
    int n_runs = 10,
    int maxIterations = 10
);

void writeGraphResultToCsv(
    const string& graphName,
    const map<string, int>& graphResults,
    int n_runs = 10,
    double localSearchAvg = 0.0,
    int maxIterations = 10,
    const string& outputFile = "results.csv",
    bool createHeader = false
);

GraphInfo getGraphInfo(const Graph& graph);

#endif 