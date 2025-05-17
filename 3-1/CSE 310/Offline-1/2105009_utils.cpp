#include "2105009_utils.h"
#include <sstream>

using namespace std;

// Function to trim leading and trailing whitespace from a string
string trim(const string &str)
{
    // Find the first non-whitespace character
    size_t start = str.find_first_not_of(" \t\r\n");

    // If the string is all whitespace, return an empty string
    if (start == string::npos)
    {
        return "";
    }

    // Find the last non-whitespace character
    size_t end = str.find_last_not_of(" \t\r\n");

    // Return the trimmed substring
    return str.substr(start, end - start + 1);
}



// Function to format the type string according to the required format
string formatType(const string &rawType)
{
    istringstream iss(rawType);
    string typeIdentifier;
    iss >> typeIdentifier;

    // Simple types like VAR, NUMBER, RELOP, or BOOL don't need formatting
    if (typeIdentifier != "FUNCTION" && typeIdentifier != "STRUCT" && typeIdentifier != "UNION")
    {
        return rawType;
    }

    string formattedType = typeIdentifier + ",";

    if (typeIdentifier == "FUNCTION")
    {
        // Format: FUNCTION,ReturnType<==(Param1,Param2,...)
        string returnType;
        iss >> returnType;

        // check to ensure returnType is not empty
        if (returnType.empty())
        {
            // Handle the error case, perhaps by returning an error message or using a default type
            return "ERROR";
        }
        formattedType += returnType + "<==";

        // Read remaining parameters and add them with commas
        string param;
        bool firstParam = true;
        formattedType += "(";
        while (iss >> param)
        {
            if (!firstParam)
            {
                formattedType += ",";
            }
            formattedType += param;
            firstParam = false;
        }
        formattedType += ")";
    }
    else if (typeIdentifier == "STRUCT" || typeIdentifier == "UNION")
    {
        // Format: STRUCT/UNION,{(Type1,name1),(Type2,name2),...}
        formattedType += "{";

        string fieldType, fieldName;
        bool firstField = true;
        
        // Store field names to check for duplicates
        const int MAX_FIELDS = 100; // Define a reasonable maximum number of fields
        string fieldNames[MAX_FIELDS];
        int fieldCount = 0;

        while (true)
        {
            // Try to read a pair of field type and field name
            string nextFieldType, nextFieldName;
            if (!(iss >> nextFieldType))
                break; // No more fields to read
            
            if (!(iss >> nextFieldName))
            {
                // Field type without field name is an error
                return "ERROR";
            }
            
            // Check if either field type or name is empty
            if (nextFieldType.empty() || nextFieldName.empty())
            {
                return "ERROR";
            }
            
            // Check for duplicate field names
            for (int i = 0; i < fieldCount; i++)
            {
                if (fieldNames[i] == nextFieldName)
                {
                    // Duplicate field name found
                    return "DUPLICATE ERROR";
                }
            }
            
            // Store the field name to check for future duplicates
            if (fieldCount < MAX_FIELDS)
            {
                fieldNames[fieldCount] = nextFieldName;
                fieldCount++;
            }
            // If too many fields, we can't check for duplicates anymore
            // We could return an error, but let's just continue processing

            if (!firstField)
            {
                formattedType += ",";
            }
            formattedType += "(" + nextFieldType + "," + nextFieldName + ")";
            firstField = false;
        }

        // If we couldn't read any pairs, it's an error
        if (firstField)
        {
            return "ERROR";
        }

        formattedType += "}";
    }

    return formattedType;
}

bool validateCommand(char command, istringstream &iss, string &name, string &type, string &errorMsg)
{
    switch (command)
    {
    case 'I':
    {
        // Insert command requires a name and a type
        if (!(iss >> name))
        {
            errorMsg = "Number of parameters mismatch for the command I";
            return false;
        }
        name = trim(name);

        // Read the type identifier (second argument)
        if (!(iss >> type))
        {
            errorMsg = "Number of parameters mismatch for the command I";
            return false;
        }
        type = trim(type);

        // Read the rest of the line to analyze for STRUCT, UNION, and FUNCTION validation
        string remaining;
        getline(iss, remaining);
        remaining = trim(remaining);

        if (type == "STRUCT" || type == "UNION")
        {
            // For STRUCT or UNION, count the number of arguments
            istringstream remainingStream(remaining);
            string token;
            int argCount = 0;

            while (remainingStream >> token)
            {
                token = trim(token);
                if (!token.empty())
                {
                    argCount++;
                }
            }

            // Must have at least 2 arguments after STRUCT/UNION
            if (argCount < 2)
            {
                errorMsg = "Number of parameters mismatch for the command I";
                return false;
            }

            // Number of arguments must be even (type-name pairs)
            if (argCount % 2 != 0)
            {
                errorMsg = "Number of parameters mismatch for the command I";
                return false;
            }
        }
        else if (type == "FUNCTION")
        {
            // For FUNCTION, must have at least one argument after FUNCTION
            istringstream remainingStream(remaining);
            string token;
            int argCount = 0;

            while (remainingStream >> token)
            {
                token = trim(token);
                if (!token.empty())
                {
                    argCount++;
                }
            }

            if (argCount < 1)
            {
                errorMsg = "Number of parameters mismatch for the command I";
                return false;
            }
        }
        else
        {
            // For any other type (not STRUCT, UNION, FUNCTION), there should be no additional arguments
            if (!remaining.empty())
            {
                errorMsg = "Number of parameters mismatch for the command I";
                return false;
            }
        }

        // Reconstruct the full type string (type + remaining)
        type = type + (remaining.empty() ? "" : " " + remaining);
        return true;
    }
    case 'L':
    {
        // Lookup command requires exactly one name
        if (!(iss >> name))
        {
            errorMsg = "Number of parameters mismatch for the command L";
            return false;
        }
        name = trim(name);

        // Check for extra parameters
        string extraParam;
        if (iss >> extraParam)
        {
            extraParam = trim(extraParam);
            errorMsg = "Number of parameters mismatch for the command L";
            return false;
        }
        return true;
    }
    case 'D':
    {
        // Delete command requires exactly one name
        if (!(iss >> name))
        {
            errorMsg = "Number of parameters mismatch for the command D";
            return false;
        }
        name = trim(name);

        // Check for extra parameters
        string extraParam;
        if (iss >> extraParam)
        {
            extraParam = trim(extraParam);
            errorMsg = "Number of parameters mismatch for the command D";
            return false;
        }
        return true;
    }
    case 'P':
    {
        // Print command requires exactly one argument (A or C)
        string printType;
        if (!(iss >> printType))
        {
            errorMsg = "Number of parameters mismatch for the command P";
            return false;
        }
        printType = trim(printType);

        // Check if it's exactly one character and either A or C
        if (printType.length() != 1 || (printType[0] != 'A' && printType[0] != 'C'))
        {
            errorMsg = "Invalid parameters";
            return false;
        }

        // Check for any extra parameters
        string extraParam;
        if (iss >> extraParam)
        {
            extraParam = trim(extraParam);
            if (!extraParam.empty())
            {
                errorMsg = "Number of parameters mismatch for the command P";
                return false;
            }
        }

        // Store the print type in the name variable for convenience
        name = printType;
        return true;
    }
    case 'S':
    case 'E':
    case 'Q':
    {
        // These commands don't require parameters
        // Check that there are no extra parameters
        string extraParam;
        if (iss >> extraParam)
        {
            extraParam = trim(extraParam);
            // If after trimming extraParam is empty, there's still no extra parameter
            if (!extraParam.empty())
            {
                errorMsg = "Number of parameters mismatch for the command " + string(1, command);
                return false;
            }
        }
        return true;
    }
    default:
    {
        errorMsg = "Invalid command";
        return false;
    }
    }
}
