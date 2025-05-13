#include "utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <filesystem>
namespace fs = filesystem;

using namespace std;

// // Check if we need to use experimental/filesystem
// #if __has_include(<filesystem>)
//     #include <filesystem>
//     namespace fs = filesystem;
// #elif __has_include(<experimental/filesystem>)
//     #include <experimental/filesystem>
//     namespace fs = experimental::filesystem;
// #else
//     #error "No filesystem support available"
// #endif

// Known best solutions for selected benchmark instances
const map<string, int> KNOWN_BEST_SOLUTIONS = {
    {"g1", 12078}, {"g2", 12084}, {"g3", 12077}, {"g11", 627}, {"g12", 621}, {"g13", 645},
    {"g14", 3187}, {"g15", 3169}, {"g16", 3172}, {"g22", 14123}, {"g23", 14129}, {"g24", 14131},
    {"g32", 1560}, {"g33", 1537}, {"g34", 1541}, {"g35", 8000}, {"g36", 7996}, {"g37", 8009},
    {"g43", 7027}, {"g44", 7022}, {"g45", 7020}, {"g48", 6000}, {"g49", 6000}, {"g50", 5988}
};

Graph loadGraphFromFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filePath);
    }
    
    // Read the first line to get n and m
    int n, m;
    file >> n >> m;
    
    // Create a graph with n vertices
    Graph graph(n);
    
    // Read the edges
    for (int i = 0; i < m; ++i) {
        int u, v, weight;
        file >> u >> v >> weight;
        graph.addEdge(u, v, weight);
    }
    
    file.close();
    return graph;
}

map<string, Graph> loadAllGraphs(const string& directory) {
    map<string, Graph> graphs;
    
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.path().extension() == ".rud") {
                string fileName = entry.path().stem().string();
                try {
                    Graph graph = loadGraphFromFile(entry.path().string());
                    graphs.insert({fileName, graph});
                } catch (const exception& e) {
                    cerr << "Error loading graph " << fileName << ": " << e.what() << endl;
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
    }
    
    return graphs;
}

void writeResultsToCsv(
    const map<string, map<string, int>>& results,
    const string& outputFile,
    int n_runs,
    int maxIterations) {
    
    ofstream file(outputFile);
    if (!file.is_open()) {
        throw runtime_error("Failed to open output file: " + outputFile);
    }
    
    // Write the header in the format matching the image
    file << "Name,|V| or n,|E| or m,Simple Randomized or Randomized-1,Simple Greedy or Greedy-1,"
         << "Semi-greedy-1,Simple local or local-1,,,GRASP-1,,,Known best solution or upper bound" << endl;
    file << ",,,,,,No. of iterations,Average value,,No. of iterations,Best value," << endl;
    
    // Write results for each graph
    for (const auto& [graphName, graphResults] : results) {
        file << graphName << ",";
        
        // Write vertex and edge counts
        auto verticesIt = graphResults.find("num_vertices");
        auto edgesIt = graphResults.find("num_edges");
        
        file << (verticesIt != graphResults.end() ? to_string(verticesIt->second) : "") << ",";
        file << (edgesIt != graphResults.end() ? to_string(edgesIt->second) : "") << ",";
        
        // Write algorithm results
        auto randomizedIt = graphResults.find("randomized");
        auto greedyIt = graphResults.find("greedy");
        auto semiGreedyIt = graphResults.find("semi_greedy");
        auto localSearchIt = graphResults.find("local_search");
        auto graspIt = graphResults.find("grasp");
        
        // Write the simple algorithms
        file << (randomizedIt != graphResults.end() ? to_string(randomizedIt->second) : "") << ",";
        file << (greedyIt != graphResults.end() ? to_string(greedyIt->second) : "") << ",";
        file << (semiGreedyIt != graphResults.end() ? to_string(semiGreedyIt->second) : "") << ",";
        
        // Write local search with iterations and average
        double localSearchAvg = 0.0;

        file << (localSearchIt != graphResults.end() ? to_string(n_runs) : "") << ",";
        file << (localSearchIt != graphResults.end() ? to_string(localSearchAvg) : "") << ",,";
        
        // Write GRASP with iterations and best value
        file << (graspIt != graphResults.end() ? to_string(maxIterations) : "") << ",";
        file << (graspIt != graphResults.end() ? to_string(graspIt->second) : "") << ",,";
        
        // Write known best solution if available, otherwise write "N/A"
        auto knownBestIt = KNOWN_BEST_SOLUTIONS.find(graphName);
        file << (knownBestIt != KNOWN_BEST_SOLUTIONS.end() ? to_string(knownBestIt->second) : "N/A");
        
        file << endl;
    }
    
    file.close();
}

// function to write a single graph result to CSV
void writeGraphResultToCsv(
    const string& graphName,
    const map<string, int>& graphResults,
    int n_runs,
    double localSearchAvg,
    int maxIterations,
    const string& outputFile,
    bool createHeader) {
    
    // Check if file exists to determine if header needs to be written
    bool fileExists = false;
    try {
        fileExists = fs::exists(outputFile);
    } catch (const fs::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
    }
    
    // Open file in append mode unless needed to be created
    ofstream file;
    if (!fileExists || createHeader) {
        file.open(outputFile);
    } else {
        file.open(outputFile, ios_base::app);
    }
    
    if (!file.is_open()) {
        throw runtime_error("Failed to open output file: " + outputFile);
    }
    
    // Write header if this is a new file or explicitly requested
    if (!fileExists || createHeader) {
        file << "Name,|V| or n,|E| or m,Simple Randomized or Randomized-1,Simple Greedy or Greedy-1,"
             << "Semi-greedy-1,Simple local or local-1,,,GRASP-1,,,Known best solution or upper bound" << endl;
        file << ",,,,,,No. of iterations,Average value,,No. of iterations,Best value," << endl;
    }
    
    // Write graph's results
    file << graphName << ",";
    
    // Write vertex and edge counts
    auto verticesIt = graphResults.find("num_vertices");
    auto edgesIt = graphResults.find("num_edges");
    
    file << (verticesIt != graphResults.end() ? to_string(verticesIt->second) : "") << ",";
    file << (edgesIt != graphResults.end() ? to_string(edgesIt->second) : "") << ",";
    
    // Write algorithm results
    auto randomizedIt = graphResults.find("randomized");
    auto greedyIt = graphResults.find("greedy");
    auto semiGreedyIt = graphResults.find("semi_greedy");
    auto localSearchIt = graphResults.find("local_search");
    auto graspIt = graphResults.find("grasp");
    
    // Write the simple algorithms
    file << (randomizedIt != graphResults.end() ? to_string(randomizedIt->second) : "") << ",";
    file << (greedyIt != graphResults.end() ? to_string(greedyIt->second) : "") << ",";
    file << (semiGreedyIt != graphResults.end() ? to_string(semiGreedyIt->second) : "") << ",";
    
    // Write local search with iterations and average
    file << (localSearchIt != graphResults.end() ? to_string(n_runs) : "") << ",";
    file << (localSearchIt != graphResults.end() ? to_string(localSearchAvg) : "") << ",,";
    
    // Write GRASP with iterations and best value
    file << (graspIt != graphResults.end() ? to_string(maxIterations) : "") << ",";
    file << (graspIt != graphResults.end() ? to_string(graspIt->second) : "") << ",,";
    
    // Write known best solution if available, otherwise write "N/A"
    auto knownBestIt = KNOWN_BEST_SOLUTIONS.find(graphName);
    file << (knownBestIt != KNOWN_BEST_SOLUTIONS.end() ? to_string(knownBestIt->second) : "N/A");
    
    file << endl;
    file.close();
}

GraphInfo getGraphInfo(const Graph& graph) {
    return {
        graph.getNumVertices(),
        static_cast<int>(graph.getEdges().size())
    };
}