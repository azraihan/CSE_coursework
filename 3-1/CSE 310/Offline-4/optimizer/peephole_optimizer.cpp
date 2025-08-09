#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>

using namespace std;

void peepholeOptimize(const string& asmFilePath) {
    ifstream inputFile(asmFilePath);
    if (!inputFile) {
        cout << "Error: Cannot open file " << asmFilePath << endl;
        return;
    }
    
    vector<string> lines;
    string line;
    
    // reading all lines
    while (getline(inputFile, line)) {
        lines.push_back(line);
    }
    inputFile.close();
    
    // tracking all jump targets
    set<string> jumpTargets;
    
    // first pass: collecting all jump targets
    for (const string& line : lines) {
        string trimmed = line;
        // trim leading whitespace
        size_t start = trimmed.find_first_not_of(" \t");
        if (start != string::npos) {
            trimmed = trimmed.substr(start);
        }
        
        // checking for jump instructions
        if (trimmed.find("JMP ") == 0 || trimmed.find("JE ") == 0 || 
            trimmed.find("JNE ") == 0 || trimmed.find("JL ") == 0 || 
            trimmed.find("JG ") == 0 || trimmed.find("JLE ") == 0 || 
            trimmed.find("JGE ") == 0 || trimmed.find("CALL ") == 0) {
            
            size_t spacePos = trimmed.find(' ');
            if (spacePos != string::npos) {
                string target = trimmed.substr(spacePos + 1);
                jumpTargets.insert(target);
            }
        }
    }
    
    vector<string> optimized;
    
    for (int i = 0; i < lines.size(); i++) {
        string currentLine = lines[i];
        string trimmed = currentLine;
        
        // trimmimg leading whitespace for analysis
        size_t start = trimmed.find_first_not_of(" \t");
        if (start != string::npos) {
            trimmed = trimmed.substr(start);
        }
        
        // Remove PUSH/POP pairs (labels in between)
        if (trimmed.find("PUSH ") == 0) {
            string pushReg = trimmed.substr(5);
            
            // looking ahead for matching POP, collecting labels in between
            int j = i + 1;
            vector<string> allLabels;
            vector<string> referencedLabels;
            bool hasReferencedLabel = false;
            bool foundMatchingPop = false;
            int popIndex = -1;
            
            while (j < lines.size()) {
                string checkLine = lines[j];
                string checkTrimmed = checkLine;
                size_t checkStart = checkTrimmed.find_first_not_of(" \t");
                if (checkStart != string::npos) {
                    checkTrimmed = checkTrimmed.substr(checkStart);
                }
                
                // if it's a label
                if (checkTrimmed.find(':') != string::npos && checkTrimmed.find(':') == checkTrimmed.length() - 1) {
                    string labelName = checkTrimmed.substr(0, checkTrimmed.find(':'));
                    allLabels.push_back(checkLine);
                    // checking if this label is referenced
                    if (jumpTargets.find(labelName) != jumpTargets.end()) {
                        hasReferencedLabel = true;
                        referencedLabels.push_back(checkLine);
                    }
                    j++;
                    continue;
                }
                
                // if it's a matching POP
                if (checkTrimmed.find("POP ") == 0) {
                    string popReg = checkTrimmed.substr(4);
                    if (pushReg == popReg) {
                        foundMatchingPop = true;
                        popIndex = j;
                    }
                }
                break; // stopping at first non-label instruction
            }
            
            if (foundMatchingPop) {
                if (hasReferencedLabel) {
                    // keeping PUSH, referenced labels, and POP
                    optimized.push_back(currentLine); // PUSH
                    for (const string& label : referencedLabels) {
                        optimized.push_back(label);
                    }
                    // letting POP be processed normally
                } else {
                    // remoing PUSH and POP, apply label keeping logic
                    if (!allLabels.empty()) {
                        // keeping only the first label if none are referenced
                        optimized.push_back(allLabels[0]);
                    }
                    // skipping to after the POP
                    i = popIndex;
                }
                continue;
            }
        }
        
        // handling consecutive labels
        if (trimmed.find(':') != string::npos && trimmed.find(':') == trimmed.length() - 1) {
            // this is is a label
            vector<string> consecutiveLabels;
            consecutiveLabels.push_back(currentLine);
            
            // looking ahead for more consecutive labels
            int j = i + 1;
            while (j < lines.size()) {
                string nextLine = lines[j];
                string nextTrimmed = nextLine;
                size_t nextStart = nextTrimmed.find_first_not_of(" \t");
                if (nextStart != string::npos) {
                    nextTrimmed = nextTrimmed.substr(nextStart);
                }
                
                if (nextTrimmed.find(':') != string::npos && nextTrimmed.find(':') == nextTrimmed.length() - 1) {
                    consecutiveLabels.push_back(nextLine);
                    j++;
                } else {
                    break;
                }
            }
            
            if (consecutiveLabels.size() > 1) {
                // if we have consecutive labels, apply optimization
                vector<string> referencedLabels;
                
                // checking the labels that are referenced
                for (const string& labelLine : consecutiveLabels) {
                    string labelTrimmed = labelLine;
                    size_t labelStart = labelTrimmed.find_first_not_of(" \t");
                    if (labelStart != string::npos) {
                        labelTrimmed = labelTrimmed.substr(labelStart);
                    }
                    string labelName = labelTrimmed.substr(0, labelTrimmed.find(':'));
                    
                    if (jumpTargets.find(labelName) != jumpTargets.end()) {
                        referencedLabels.push_back(labelLine);
                    }
                }
                
                if (!referencedLabels.empty()) {
                    // keeping all referenced labels
                    for (const string& refLabel : referencedLabels) {
                        optimized.push_back(refLabel);
                    }
                } else {
                    // keeping only the first label
                    optimized.push_back(consecutiveLabels[0]);
                }
                
                // skipping the processed labels
                i = j - 1;
            } else {
                // if its a single label, keep it
                optimized.push_back(currentLine);
            }
        } else {
            // not a label, leeping the line
            optimized.push_back(currentLine);
        }
    }
    
    // writing optimized code
    size_t lastSlash = asmFilePath.find_last_of("/\\");
    string directory = (lastSlash != string::npos) ? asmFilePath.substr(0, lastSlash) : ".";
    string optimizedPath = directory + "/optcode.asm";
    
    ofstream outputFile(optimizedPath);
    if (!outputFile) {
        cout << "Error: Cannot create optimized file " << optimizedPath << endl;
        return;
    }
    
    for (const string& line : optimized) {
        outputFile << line << endl;
    }
    
    outputFile.close();
}