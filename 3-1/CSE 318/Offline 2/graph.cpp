#include "graph.h"
#include <sstream>

using namespace std;

Graph::Graph()
    : numVertices(0) {
    // Initialize with empty structures
}

Graph::Graph(int numVertices, const vector<tuple<int, int, int>>& edges)
    : numVertices(numVertices), edges(edges) {
    
    // Initialize adjacency list for each vertex
    for (int v = 1; v <= numVertices; ++v) {
        adjacencyList[v] = vector<int>();
    }
    
    // Build adjacency list and weights dict from edges
    if (!edges.empty()) {
        for (const auto& edge : edges) {
            int u = get<0>(edge);
            int v = get<1>(edge);
            int weight = get<2>(edge);
            
            weightMatrix[u][v] = weight;
            weightMatrix[v][u] = weight; // graph is undirected
            adjacencyList[u].push_back(v);
            adjacencyList[v].push_back(u);
        }
    }
}

void Graph::addEdge(int u, int v, int weight) {
    edges.push_back(make_tuple(u, v, weight));
    weightMatrix[u][v] = weight;
    weightMatrix[v][u] = weight; // graph is undirected
    adjacencyList[u].push_back(v);
    adjacencyList[v].push_back(u);
}

const vector<int>& Graph::getNeighbors(int vertex) const {
    static const vector<int> emptyVector;
    auto it = adjacencyList.find(vertex);
    if (it != adjacencyList.end()) {
        return it->second;
    }
    return emptyVector;
}

int Graph::getEdgeWeight(int u, int v) const {
    auto uIt = weightMatrix.find(u);
    if (uIt != weightMatrix.end()) {
        auto vIt = uIt->second.find(v);
        if (vIt != uIt->second.end()) {
            return vIt->second;
        }
    }
    return 0;
}

void Graph::precomputeEdgeContributions(const set<int>& setS, unordered_map<int, int>& contributions) const {
    // For each vertex, precompute contribution to cut
    for (int v = 1; v <= numVertices; ++v) {
        int contribution = 0;
        for (int neighbor : getNeighbors(v)) {
            if (setS.count(neighbor) > 0) {
                // Neighbor is in set S
                contribution += weightMatrix.at(v).at(neighbor);
            }
        }
        contributions[v] = contribution;
    }
}

int Graph::calculateCutWeight(const set<int>& setS) const {
    int cutWeight = 0;
    
    // only iterate through edges once
    for (const auto& edge : edges) {
        int u = get<0>(edge);
        int v = get<1>(edge);
        int weight = get<2>(edge);
        
        // Edge contributes to cut if its endpoints are in different partitions
        if ((setS.count(u) > 0 && setS.count(v) == 0) || 
            (setS.count(u) == 0 && setS.count(v) > 0)) {
            cutWeight += weight;
        }
    }
    
    return cutWeight;
}

pair<int, int> Graph::findMaxWeightEdge() const {
    int maxWeight = numeric_limits<int>::min();
    pair<int, int> maxEdge = {0, 0};
    
    for (const auto& edge : edges) {
        int u = get<0>(edge);
        int v = get<1>(edge);
        int weight = get<2>(edge);
        
        if (weight > maxWeight) {
            maxWeight = weight;
            maxEdge = {u, v};
        }
    }
    
    return maxEdge;
}

pair<int, int> Graph::calculateSigmaValues(
    const set<int>& setX, 
    const set<int>& setY, 
    int vertex) const {
    
    int sigmaX = 0;
    int sigmaY = 0;
    
    // Only iterate through neighbors, not all vertices in sets
    for (int neighbor : getNeighbors(vertex)) {
        int weight = weightMatrix.at(vertex).at(neighbor);
        
        if (setX.count(neighbor) > 0) {
            sigmaX += weight;
        } else if (setY.count(neighbor) > 0) {
            sigmaY += weight;
        }
    }
    
    return {sigmaX, sigmaY};
}

int Graph::calculateDelta(const set<int>& setS, const set<int>& sComplement, int vertex) const {
    int sigmaS = 0;
    int sigmaSComplement = 0;
    
    // Only iterate through neighbors, not all vertices in sets
    for (int neighbor : getNeighbors(vertex)) {
        int weight = weightMatrix.at(vertex).at(neighbor);
        
        if (setS.count(neighbor) > 0) {
            sigmaS += weight;
        } else if (sComplement.count(neighbor) > 0) {
            sigmaSComplement += weight;
        }
    }
    
    // If vertex is in S, calculate delta for moving to S̄
    if (setS.count(vertex) > 0) {
        return sigmaS - sigmaSComplement;
    } 
    // If vertex is in S̄, calculate delta for moving to S
    else {
        return sigmaSComplement - sigmaS;
    }
}

int Graph::getNumVertices() const {
    return numVertices;
}

const vector<tuple<int, int, int>>& Graph::getEdges() const {
    return edges;
}

string Graph::toString() const {
    stringstream ss;
    ss << "Graph with " << numVertices << " vertices and " << edges.size() << " edges";
    return ss.str();
}