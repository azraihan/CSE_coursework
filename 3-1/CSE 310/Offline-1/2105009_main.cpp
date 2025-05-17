#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "2105009_SymbolTable.h"
#include "2105009_hash.h"
#include "2105009_utils.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    ifstream inFile(argv[1]);
    ofstream outFile(argv[2]);

    if (!inFile.is_open())
    {
        cout << "Error opening input file: " << argv[1] << endl;
        return 1;
    }

    if (!outFile.is_open())
    {
        cout << "Error opening output file: " << argv[2] << endl;
        inFile.close();
        return 1;
    }

    int numBuckets;
    inFile >> numBuckets;

    {
        SymbolTable symbolTable(numBuckets, outFile, SDBMHash);

        string command;
        string name, type;
        int commandCount = 1;

        while (inFile >> command)
        {

            outFile << "Cmd " << commandCount << ": ";

            // Read the rest of the line
            string line;
            getline(inFile, line);

            if (trim(command).empty())
            {
                continue;
            }

            // Create a trimmed version for parsing
            string trimmedLine = trim(line);

            // Make sure command line is properly formatted with a space only if there are arguments
            string outputLine = command + (trimmedLine.empty() ? "" : " " + trimmedLine);
            outFile << outputLine << endl;

            // Create a string stream for parsing the arguments using the trimmed line
            istringstream iss(trimmedLine);

            // Validate the command
            string errorMsg;
            if (command == "I")
            {
                // Insert command
                if (!validateCommand('I', iss, name, type, errorMsg))
                {
                    outFile << "\t" << errorMsg << endl;
                }
                else
                {
                    // Format the type string before inserting
                    string formattedType = formatType(type);
                    if (formattedType == "ERROR")
                    {
                        outFile << "\tNumber of parameters mismatch for the command I\n";
                        commandCount++;
                        continue;
                    }
                    else if (formattedType == "DUPLICATE ERROR")
                    {
                        outFile << "\tDuplicate Parameters are not allowed\n";
                        commandCount++;
                        continue;
                    }
                    symbolTable.insert(name, formattedType);
                }
            }

            else if (command == "L")
            {
                // Lookup command
                if (!validateCommand('L', iss, name, type, errorMsg))
                {
                    outFile << "\t" << errorMsg << endl;
                }
                else
                {
                    symbolTable.lookUp(name);
                }
            }

            else if (command == "D")
            {
                // Delete command
                if (!validateCommand('D', iss, name, type, errorMsg))
                {
                    outFile << "\t" << errorMsg << endl;
                }
                else
                {
                    symbolTable.remove(name);
                }
            }
            else if (command == "P")
            {
                // Print command
                if (!validateCommand('P', iss, name, type, errorMsg))
                {
                    outFile << "\t" << errorMsg << endl;
                }
                else
                {
                    char printType = name[0]; // Get the print type from name

                    if (printType == 'A')
                    {
                        symbolTable.printAllScopeTables();
                    }
                    else if (printType == 'C')
                    {
                        symbolTable.printCurrentScopeTable();
                    }
                }
            }
            else if (command == "S")
            {
                // Enter scope command
                if (!validateCommand('S', iss, name, type, errorMsg))
                {
                    outFile << "\t" << errorMsg << endl;
                }
                else
                {
                    symbolTable.enterScope();
                }
            }
            else if (command == "E")
            {
                // Exit scope command
                if (!validateCommand('E', iss, name, type, errorMsg))
                {
                    outFile << "\t" << errorMsg << endl;
                }
                else
                {
                    symbolTable.exitScope();
                }
            }
            else if (command == "Q")
            {
                // Quit command
                if (!validateCommand('Q', iss, name, type, errorMsg))
                {
                    outFile << "\t" << errorMsg << endl;
                }
                else
                {
                    break;
                }
            }
            else
            {
                outFile << "\tInvalid command" << endl;
            }
            commandCount++;
        }
    }
    inFile.close();
    outFile.close();

    return 0;
}