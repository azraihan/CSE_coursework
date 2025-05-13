#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <set>
#include <limits>
#include <utility>
#include <iostream>
#include <tuple>

using namespace std;

class Graph {
private:
    int numVertices;
    vector<tuple<int, int, int>> edges; // (u, v, weight)
    
    unordered_map<int, unordered_map<int, int>> weightMatrix; 
    unordered_map<int, vector<int>> adjacencyList; // Maps vertex -> list of neighbors

    void precomputeEdgeContributions(const set<int>& setS, unordered_map<int, int>& contributions) const;

public:
    Graph();
    Graph(int numVertices, const vector<tuple<int, int, int>>& edges = {});
    void addEdge(int u, int v, int weight);
    const vector<int>& getNeighbors(int vertex) const;
    int getEdgeWeight(int u, int v) const;
    int calculateCutWeight(const set<int>& setS) const;
    pair<int, int> findMaxWeightEdge() const;
    pair<int, int> calculateSigmaValues(const set<int>& setX, const set<int>& setY, int vertex) const;
    int calculateDelta(const set<int>& setS, const set<int>& sComplement, int vertex) const;
    int getNumVertices() const;
    const vector<tuple<int, int, int>>& getEdges() const;
    string toString() const;
};

#endif