#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include "2105009_SymbolTable.h"
#include "2105009_hash.h"
#include "2105009_utils.h"

using namespace std;

// Function to process commands with a given hash function and return collision count
int testHashFunction(unsigned int (*hashFunc)(string, unsigned int), const string& hashName, const string& inputFile, int numBuckets, ofstream& reportStream) {
    // Open the input file
    ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        cerr << "Error opening input file: " << inputFile << endl;
        return -1;
    }
    
    // Create a null output stream to discard output
    ofstream nullStream("/dev/null");
    if (!nullStream.is_open()) {
        cerr << "Error opening /dev/null" << endl;
        inFile.close();
        return -1;
    }
    
    // Create a symbol table with the specified hash function
    SymbolTable symbolTable(numBuckets, nullStream, hashFunc);
    
    // Process all commands in the input file
    string command;
    while (inFile >> command) {
        // Skip the rest of the line
        string line;
        getline(inFile, line);
        string trimmedLine = trim(line);
        
        // Process the command
        istringstream iss(trimmedLine);
        string name, type, errorMsg;
        
        if (command == "I") {
            if (validateCommand('I', iss, name, type, errorMsg)) {
                string formattedType = formatType(type);
                if (formattedType != "ERROR" && formattedType != "DUPLICATE ERROR") {
                    symbolTable.insert(name, formattedType);
                }
            }
        }
        else if (command == "L") {
            if (validateCommand('L', iss, name, type, errorMsg)) {
                symbolTable.lookUp(name);
            }
        }
        else if (command == "D") {
            if (validateCommand('D', iss, name, type, errorMsg)) {
                symbolTable.remove(name);
            }
        }
        else if (command == "P") {
            if (validateCommand('P', iss, name, type, errorMsg)) {
                if (name[0] == 'A') {
                    symbolTable.printAllScopeTables();
                } else if (name[0] == 'C') {
                    symbolTable.printCurrentScopeTable();
                }
            }
        }
        else if (command == "S") {
            if (validateCommand('S', iss, name, type, errorMsg)) {
                symbolTable.enterScope();
            }
        }
        else if (command == "E") {
            if (validateCommand('E', iss, name, type, errorMsg)) {
                symbolTable.exitScope();
            }
        }
        else if (command == "Q") {
            break;
        }
    }
    
    // Get the collision count
    int collisions = symbolTable.getTotalCollisionCount();
    
    // Close the files
    inFile.close();
    nullStream.close();
    
    // Write the result to the report
    reportStream << left << setw(20) << hashName + " Hash" << right << setw(15) << collisions << endl;
    
    return collisions;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <input_file> <output_file> <number_of_buckets>" << endl;
        return 1;
    }
    
    string inputFile = argv[1];
    string outputFile = argv[2];
    
    // Parse the number of buckets from command line
    int numBuckets;
    try {
        numBuckets = stoi(argv[3]);
        if (numBuckets <= 0) {
            throw invalid_argument("Number of buckets must be positive");
        }
    } catch (const exception& e) {
        cerr << "Error: Invalid number of buckets. " << e.what() << endl;
        return 1;
    }
    
    // Open the report file
    ofstream reportFile(outputFile);
    if (!reportFile.is_open()) {
        cerr << "Error opening output file: " << outputFile << endl;
        return 1;
    }
    
    // Write the header of the report
    reportFile << "Symbol Table Hash Function Performance Report" << endl;
    reportFile << "===========================================" << endl;
    reportFile << "Input File: " << inputFile << endl;
    reportFile << "Number of Buckets: " << numBuckets << endl;
    reportFile << "-------------------------------------------" << endl;
    reportFile << endl;
    
    reportFile << "Collision Counts:" << endl;
    reportFile << "----------------" << endl;
    reportFile << left << setw(20) << "Hash Function" << right << setw(15) << "Collisions" << endl;
    reportFile << "-------------------------------------" << endl;
    
    // Test each hash function
    int sdbmCollisions = testHashFunction(SDBMHash, "SDBM", inputFile, numBuckets, reportFile);
    int djb2Collisions = testHashFunction(DJB2Hash, "DJB2", inputFile, numBuckets, reportFile);
    int fnv1aCollisions = testHashFunction(FNV1aHash, "FNV1a", inputFile, numBuckets, reportFile);
    
    reportFile << "-------------------------------------" << endl;
    reportFile << endl;
    
    // Determine the best hash function
    string bestHash;
    int minCollisions = INT_MAX;
    
    if (sdbmCollisions >= 0 && sdbmCollisions < minCollisions) {
        minCollisions = sdbmCollisions;
        bestHash = "SDBM";
    }
    
    if (djb2Collisions >= 0 && djb2Collisions < minCollisions) {
        minCollisions = djb2Collisions;
        bestHash = "DJB2";
    }
    
    if (fnv1aCollisions >= 0 && fnv1aCollisions < minCollisions) {
        minCollisions = fnv1aCollisions;
        bestHash = "FNV1a";
    }
    
    // Write the summary
    reportFile << "Summary:" << endl;
    reportFile << "--------" << endl;
    
    if (minCollisions == INT_MAX) {
        reportFile << "Error: Could not determine the best hash function." << endl;
    } else {
        reportFile << "Best Performing Hash Function: " << bestHash << " Hash with " << minCollisions << " collisions." << endl;
    }
    
    // Close the report file
    reportFile.close();
    
    cout << "Results written to " << outputFile << endl;
    
    return 0;
}