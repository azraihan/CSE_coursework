#include "graph.h"
#include "randomized.h"
#include "greedy.h"
#include "semi_greedy.h"
#include "local_search.h"
#include "grasp.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>

using namespace std;

#include <filesystem>
namespace fs = filesystem;

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

map<string, int> runAllAlgorithmsOnGraph(
    const Graph& graph, 
    const string& graphName,
    int n = 10,
    double alpha = 0.5,
    int n_runs = 10,
    int maxIterations = 10) {
    
    map<string, int> graphResults;
    LocalSearchResult localSearchResult; 
    double localSearchAvg = 0.0;
    
    // Get graph info
    GraphInfo graphInfo = getGraphInfo(graph);
    graphResults["num_vertices"] = graphInfo.numVertices;
    graphResults["num_edges"] = graphInfo.numEdges;
    
    // Run Randomized Algorithm
    cout << "  Running Randomized algorithm on " << graphName << " with n=" << n << "..." << endl;
    auto startTime = chrono::high_resolution_clock::now();
    RandomizedResult randomizedResult = randomizedMaxCut(graph, n);
    auto endTime = chrono::high_resolution_clock::now();
    auto randomizedTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    cout << "    Cut weight: " << randomizedResult.value << ", Time: " 
              << fixed << setprecision(4) << randomizedTime << "s" << endl;
    graphResults["randomized"] = randomizedResult.value;
    
    // Run Greedy Algorithm
    cout << "  Running Greedy algorithm on " << graphName << "..." << endl;
    startTime = chrono::high_resolution_clock::now();
    GreedyResult greedyResult = greedyMaxCut(graph);
    endTime = chrono::high_resolution_clock::now();
    auto greedyTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    cout << "    Cut weight: " << greedyResult.value << ", Time: " 
              << fixed << setprecision(4) << greedyTime << "s" << endl;
    graphResults["greedy"] = greedyResult.value;
    
    // Run Semi-Greedy Algorithm
    cout << "  Running Semi-Greedy algorithm on " << graphName << " with alpha=" << alpha << "..." << endl;
    startTime = chrono::high_resolution_clock::now();
    SemiGreedyResult semiGreedyResult = semiGreedyMaxCut(graph, alpha);
    endTime = chrono::high_resolution_clock::now();
    auto semiGreedyTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    cout << "    Cut weight: " << semiGreedyResult.value << ", Time: " 
              << fixed << setprecision(4) << semiGreedyTime << "s" << endl;
    graphResults["semi_greedy"] = semiGreedyResult.value;
    
    // Run Local Search Algorithm with random initial solution
    cout << "  Running Local Search algorithm on " << graphName << " with n_runs=" << n_runs << "..." << endl;
    startTime = chrono::high_resolution_clock::now();
    localSearchResult = localSearch(graph, nullptr, n_runs);
    endTime = chrono::high_resolution_clock::now();
    auto localSearchTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    cout << "    Cut weight: " << localSearchResult.value << ", Average: " << localSearchResult.averageValue 
          << ", Time: " << fixed << setprecision(4) << localSearchTime << "s" << endl;
    graphResults["local_search"] = localSearchResult.value;
    localSearchAvg = localSearchResult.averageValue;
    
    // Run GRASP Algorithm
    cout << "  Running GRASP algorithm on " << graphName << " with max_iterations=" << maxIterations 
         << ", alpha=" << alpha << "..." << endl;
    startTime = chrono::high_resolution_clock::now();
    GraspResult graspResult = grasp(graph, maxIterations, alpha);
    endTime = chrono::high_resolution_clock::now();
    auto graspTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    cout << "    Cut weight: " << graspResult.value << ", Time: " 
              << fixed << setprecision(4) << graspTime << "s" << endl;
    graphResults["grasp"] = graspResult.value;
    
    // Display known best solution if available
    auto knownBestIt = KNOWN_BEST_SOLUTIONS.find(graphName);
    if (knownBestIt != KNOWN_BEST_SOLUTIONS.end()) {
        cout << "  Known best solution: " << knownBestIt->second << endl;
    } else {
        cout << "  Known best solution: N/A" << endl;
    }
    
    // Store average local search value for CSV output
    graphResults["local_search_avg"] = static_cast<int>(localSearchAvg);
    
    return graphResults;
}

pair<int, pair<set<int>, set<int>>> runSpecificAlgorithm(
    const Graph& graph, 
    const string& graphName, 
    const string& algorithm,
    int n = 10,
    double alpha = 0.5,
    int maxIterations = 10,
    int n_runs = 10) {
    
    auto startTime = chrono::high_resolution_clock::now();
    pair<int, pair<set<int>, set<int>>> result;
    double localSearchAvg = 0.0;
    
    if (algorithm == "randomized") {
        cout << "Running Randomized algorithm on " << graphName << " with n=" << n << "..." << endl;
        RandomizedResult randomizedResult = randomizedMaxCut(graph, n);
        result = {randomizedResult.value, randomizedResult.partition};
    }
    else if (algorithm == "greedy") {
        cout << "Running Greedy algorithm on " << graphName << "..." << endl;
        GreedyResult greedyResult = greedyMaxCut(graph);
        result = {greedyResult.value, greedyResult.partition};
    }
    else if (algorithm == "semi_greedy") {
        cout << "Running Semi-Greedy algorithm on " << graphName << " with alpha=" << alpha << "..." << endl;
        SemiGreedyResult semiGreedyResult = semiGreedyMaxCut(graph, alpha);
        result = {semiGreedyResult.value, semiGreedyResult.partition};
    }
    else if (algorithm == "local_search") {
        cout << "Running Local Search algorithm on " << graphName << " with n_runs=" << n_runs << "..." << endl;
        LocalSearchResult localSearchResult = localSearch(graph, nullptr, n_runs);
        result = {localSearchResult.value, localSearchResult.partition};
        localSearchAvg = localSearchResult.averageValue;
        cout << "    Average cut weight across " << n_runs << " runs: " << localSearchResult.averageValue << endl;
    }
    else if (algorithm == "grasp") {
        cout << "Running GRASP algorithm on " << graphName 
                 << " with max_iterations=" << maxIterations 
                 << ", alpha=" << alpha << "..." << endl;
        GraspResult graspResult = grasp(graph, maxIterations, alpha);
        result = {graspResult.value, graspResult.partition};
    }
    else {
        cerr << "Unknown algorithm: " << algorithm << endl;
        return {0, {{}, {}}};
    }
    
    auto endTime = chrono::high_resolution_clock::now();
    auto executionTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    cout << "Result: Cut weight = " << result.first 
              << ", Time: " << fixed << setprecision(4) << executionTime << "s" << endl;
    
    // Display known best solution if available
    auto knownBestIt = KNOWN_BEST_SOLUTIONS.find(graphName);
    if (knownBestIt != KNOWN_BEST_SOLUTIONS.end()) {
        cout << "Known best solution: " << knownBestIt->second << endl;
    } else {
        cout << "Known best solution: N/A" << endl;
    }
    
    return result;
}

void printUsage(const string& programName) {
    cout << "Usage: " << programName << " [OPTIONS]" << endl;
    cout << "Options:" << endl;
    cout << "  --all                  Run all algorithms on all graphs" << endl;
    cout << "  --graph NAME           Specify a single graph to run (without .rud extension)" << endl;
    cout << "  --algorithm ALG        Specify a single algorithm to run" << endl;
    cout << "                          ALG can be: randomized, greedy, semi_greedy, local_search, grasp" << endl;
    cout << "  --n N                  Number of iterations for randomized algorithm (default: 10)" << endl;
    cout << "  --alpha A              Alpha parameter for semi-greedy and GRASP algorithms (default: 0.5)" << endl;
    cout << "  --max_iterations M     Maximum iterations for GRASP algorithm (default: 10)" << endl;
    cout << "  --n_runs R             Number of runs for local search algorithm (default: 10)" << endl;
    cout << "  --help                 Display this help message" << endl;
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    string graphName;
    string algorithm;
    bool runAll = false;
    int n = 10;
    double alpha = 0.5;
    int maxIterations = 10;
    int n_runs = 10;
    
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        
        if (arg == "--all") {
            runAll = true;
        }
        else if (arg == "--graph" && i + 1 < argc) {
            graphName = argv[++i];
        }
        else if (arg == "--algorithm" && i + 1 < argc) {
            algorithm = argv[++i];
        }
        else if (arg == "--n" && i + 1 < argc) {
            n = stoi(argv[++i]);
        }
        else if (arg == "--alpha" && i + 1 < argc) {
            alpha = stod(argv[++i]);
        }
        else if (arg == "--max_iterations" && i + 1 < argc) {
            maxIterations = stoi(argv[++i]);
        }
        else if (arg == "--n_runs" && i + 1 < argc) {
            n_runs = stoi(argv[++i]);
        }
        else if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }
    
    // Create output directory if it doesn't exist
    try {
        if (!fs::exists("results")) {
            fs::create_directory("results");
        }
    } catch (const exception& e) {
        cerr << "Error creating results directory: " << e.what() << endl;
        cerr << "Will attempt to write results to current directory." << endl;
    }
    
    // Directory containing graph files
    string graphsDir = "graph_GRASP/set1";
    
    // Case 1: Run all algorithms on all graphs
    if (runAll || (graphName.empty() && algorithm.empty())) {
        cout << "Loading all graphs from " << graphsDir << "..." << endl;
        auto allGraphs = loadAllGraphs(graphsDir);
        
        if (allGraphs.empty()) {
            cerr << "No graph files found in " << graphsDir << ". Please check the directory path." << endl;
            return 1;
        }
        
        cout << "Loaded " << allGraphs.size() << " graph instances." << endl;
        cout << "Using parameters: n=" << n << ", alpha=" << alpha 
             << ", n_runs=" << n_runs << ", max_iterations=" << maxIterations << endl;
        
        // Process each graph
        string outputCsv = "results/results.csv";
        bool firstGraph = true;
        
        for (const auto& [name, graph] : allGraphs) {
            cout << "\nProcessing graph: " << name << endl;
            map<string, int> graphResults = runAllAlgorithmsOnGraph(
                graph, name, n, alpha, n_runs, maxIterations);
            
            // Extract local search average value for CSV
            double localSearchAvg = 0.0;
            auto avgIt = graphResults.find("local_search_avg");
            if (avgIt != graphResults.end()) {
                localSearchAvg = static_cast<double>(avgIt->second);
            }
                
            // Write this graph's results to CSV immediately
            writeGraphResultToCsv(
                name, 
                graphResults, 
                n_runs,
                localSearchAvg,
                maxIterations,
                outputCsv, 
                firstGraph
            );
            firstGraph = false;
            
            cout << "  Results for " << name << " written to " << outputCsv << endl;
        }
        
        cout << "\nAll results have been written to " << outputCsv << endl;
    }
    // Case 2: Run a specific algorithm on a specific graph
    else if (!graphName.empty() && !algorithm.empty()) {
        string graphFile = graphsDir + "/" + graphName + ".rud";
        
        try {
            if (!fs::exists(graphFile)) {
                cerr << "Error: Graph file " << graphFile << " not found." << endl;
                return 1;
            }
        } catch (const exception& e) {
            cerr << "Error checking if file exists: " << e.what() << endl;
            cerr << "Will attempt to load the file anyway." << endl;
        }
        
        cout << "Loading graph " << graphName << "..." << endl;
        Graph graph = loadGraphFromFile(graphFile);
        
        // Run the specified algorithm with custom parameters
        auto result = runSpecificAlgorithm(graph, graphName, algorithm, n, alpha, maxIterations, n_runs);
        
        if (result.first > 0) {
            cout << "\nDetailed Result:" << endl;
            cout << "  Cut weight: " << result.first << endl;
            
            cout << "  Partition S: {";
            bool first = true;
            for (int v : result.second.first) {
                if (!first) cout << ", ";
                cout << v;
                first = false;
            }
            cout << "}" << endl;
            
            cout << "  Partition S̄: {";
            first = true;
            for (int v : result.second.second) {
                if (!first) cout << ", ";
                cout << v;
                first = false;
            }
            cout << "}" << endl;
        }
    }
    // Case 3: Run all algorithms on a specific graph
    else if (!graphName.empty() && algorithm.empty()) {
        string graphFile = graphsDir + "/" + graphName + ".rud";
        
        try {
            if (!fs::exists(graphFile)) {
                cerr << "Error: Graph file " << graphFile << " not found." << endl;
                return 1;
            }
        } catch (const exception& e) {
            cerr << "Error checking if file exists: " << e.what() << endl;
            cerr << "Will attempt to load the file anyway." << endl;
        }
        
        cout << "Loading graph " << graphName << "..." << endl;
        Graph graph = loadGraphFromFile(graphFile);
        
        // Run all algorithms on this graph
        auto graphResults = runAllAlgorithmsOnGraph(graph, graphName, n, alpha, n_runs, maxIterations);
        
        // Extract local search average value for CSV
        double localSearchAvg = 0.0;
        auto avgIt = graphResults.find("local_search_avg");
        if (avgIt != graphResults.end()) {
            localSearchAvg = static_cast<double>(avgIt->second);
        }
        
        cout << "\nSummary Results:" << endl;
        cout << "Graph: " << graphName << endl;
        cout << "Vertices: " << graphResults["num_vertices"] 
                  << ", Edges: " << graphResults["num_edges"] << endl;
        cout << "Randomized: " << graphResults["randomized"] << endl;
        cout << "Greedy: " << graphResults["greedy"] << endl;
        cout << "Semi-Greedy: " << graphResults["semi_greedy"] << endl;
        cout << "Local Search: " << graphResults["local_search"] << endl;
        cout << "GRASP: " << graphResults["grasp"] << endl;
        
        // Display known best solution if available
        auto knownBestIt = KNOWN_BEST_SOLUTIONS.find(graphName);
        if (knownBestIt != KNOWN_BEST_SOLUTIONS.end()) {
            cout << "Known best solution: " << knownBestIt->second << endl;
        } else {
            cout << "Known best solution: N/A" << endl;
        }
        
        // // Write results to CSV
        // string outputCsv = "results/results.csv";
        // writeGraphResultToCsv(
        //     graphName, 
        //     graphResults, 
        //     n_runs,
        //     localSearchAvg,
        //     maxIterations,
        //     outputCsv, 
        //     true
        // );
        // cout << "\nResults have been written to " << outputCsv << endl;
    }
    // Case 4: Invalid combination
    else if (graphName.empty() && !algorithm.empty()) {
        cerr << "Error: You must specify a graph when specifying an algorithm." << endl;
        printUsage(argv[0]);
        return 1;
    }
    
    cout << "\nDone!" << endl;
    return 0;
}