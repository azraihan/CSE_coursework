#include "2105009_ScopeTable.h"
#include <iostream>
#include <string>

using namespace std;

ScopeTable::ScopeTable(int numBuckets, int id, ofstream *out, unsigned int (*hashFunction)(std::string, unsigned int))
{
    this->numBuckets = numBuckets;
    this->id = id;
    this->out = out;
    this->collisionCount = 0;
    this->hashFunction = hashFunction;

    hashTable = new SymbolInfo *[numBuckets];

    // Initialize all buckets to nullptr
    for (int i = 0; i < numBuckets; i++)
    {
        hashTable[i] = nullptr;
    }

    parentScope = nullptr;

    ///////////////////////////////////////////////////////////////////////////
    // // for offlline-2
    this->setRecentScope(this->getRecentScope() + 1);
    ///////////////////////////////////////////////////////////////////////////

    // *out << "\tScopeTable# " << id << " created" << endl;
}

ScopeTable::~ScopeTable()
{
    // *out << "\tScopeTable# " << id << " removed" << endl;

    for (int i = 0; i < numBuckets; i++)
    {
        SymbolInfo *current = hashTable[i];
        while (current != nullptr)
        {
            SymbolInfo *temp = current;
            current = current->getNext();

            delete temp;
        }
    }

    delete[] hashTable;
}

bool ScopeTable::insert(const string &name, const string &type, const std::string &varDataType, const std::string &functionReturnType, int functionArgumentNumber, std::vector<std::string>parameterTypes)
{
    int hashValue = hashFunction(name, numBuckets) % numBuckets;

    // Check if symbol already exists
    SymbolInfo *current = hashTable[hashValue];
    SymbolInfo *prev = nullptr;
    int position = 0;

    while (current != nullptr)
    {
        if (current->getName() == name)
        {
            // Symbol already exists
            // *out << "\t'" << name << "' already exists in the current ScopeTable" << endl;
            return false;
        }
        prev = current;
        current = current->getNext();
        position++;
    }

    // Create new symbol
    SymbolInfo *newSymbol = new SymbolInfo(name, type, varDataType, functionReturnType, functionArgumentNumber, parameterTypes);

    // Insert at head if bucket is empty
    if (prev == nullptr)
    {
        hashTable[hashValue] = newSymbol;
    }
    else
    {
        // Insert at end of chain
        prev->setNext(newSymbol);
        this->collisionCount++;
    }

    // *out << "\tInserted in ScopeTable# " << id << " at position " << hashValue + 1 << ", " << position + 1 << endl;
    return true;
}

SymbolInfo *ScopeTable::lookUp(const string &name, bool printMessage)
{
    int hashValue = hashFunction(name, numBuckets) % numBuckets;

    SymbolInfo *current = hashTable[hashValue];
    int position = 0;

    while (current != nullptr)
    {
        if (current->getName() == name)
        {
            if (printMessage)
            {
                // *out << "\t\'" << name << "\' found in ScopeTable# " << id << " at position " << hashValue + 1 << ", " << position + 1 << endl;
                // *out << "< " << current->getName() << " : " << current->getType() << " >" << " already exists in ScopeTable# " << getConcatenatedIdString(this) << " at position " << hashValue<< ", " << position<< endl;
            }
            return current;
        }
        current = current->getNext();
        position++;
    }

    return nullptr;
}

bool ScopeTable::deleteSymbol(const string &name)
{
    int hashValue = hashFunction(name, numBuckets) % numBuckets;

    SymbolInfo *current = hashTable[hashValue];
    SymbolInfo *prev = nullptr;
    int position = 0;

    while (current != nullptr)
    {
        if (current->getName() == name)
        {
            // Found the symbol to delete
            if (prev == nullptr)
            {
                // It's the head of the list
                hashTable[hashValue] = current->getNext();
            }
            else
            {
                // It's in the middle or end of the list
                prev->setNext(current->getNext());
            }

            // *out << "\tDeleted \'" << name << "\' from ScopeTable# " << id << " at position " << hashValue + 1 << ", " << position + 1 << endl;
            delete current;
            return true;
        }

        prev = current;
        current = current->getNext();
        position++;
    }

    // Symbol not found
    // *out << "\tNot found" << endl;
    return false;
}

///////////////////////////////////////////////////////////////////////////

// // Print the scope table with specified indentation (for offline-1)
// void ScopeTable::print(int indentLevel)
// {
//     // Print scope header with indentation
//     for (int i = 0; i < indentLevel; i++)
//     {
//         *out << "\t";
//     }
//     *out << "\tScopeTable# " << id << endl;

//     // Print each bucket
//     for (int i = 0; i < numBuckets; i++)
//     {
//         SymbolInfo *current = hashTable[i];
//         int currentIndex = i + 1;  // for 1 indexed

//         // Indentation + tab
//         for (int j = 0; j < indentLevel; j++)
//         {
//             *out << "\t";
//         }
//         *out << "\t" << currentIndex << "--> ";

//         // Print bucket contents
//         while (current != nullptr)
//         {
//             *out << "<" << current->getName() << "," << current->getType() << "> ";
//             current = current->getNext();
//         }
//         *out << endl;
//     }
// }

///////////////////////////////////////////////////////////////////////////

// printing according to the spec of offline-2 (lexical analysis)

string ScopeTable::getConcatenatedIdString(ScopeTable *scopeTable){
    if(scopeTable->parentScope==nullptr){
        return to_string(scopeTable->id);
    }

    int convertedIdWithRespectToOriginalScopeTable = scopeTable->id - 1;

    int parentID = scopeTable->parentScope->getId();

    string concatenatedStringId = getConcatenatedIdString(scopeTable->getParentScope()) + "." + to_string(convertedIdWithRespectToOriginalScopeTable);

    return concatenatedStringId;

}

void ScopeTable::print()
{
    *out << "ScopeTable # " << getConcatenatedIdString(this) << endl;

    // Print each bucket
    for (int i = 0; i < numBuckets; i++)
    {
        SymbolInfo *current = hashTable[i];

        //print only the nonempty rows
        if(current == nullptr){
            continue;
        }

        int currentIndex = i;  // for 0 indexed
       
        *out << currentIndex << " --> ";

        // Print bucket contents
        while (current != nullptr)
        {
            *out << "< " << current->getName() << " : " << current->getType() << " >";
            current = current->getNext();
        }
        *out << endl;
    }
}
///////////////////////////////////////////////////////////////////////////

ScopeTable *ScopeTable::getParentScope() const
{
    return parentScope;
}

int ScopeTable::getId() const
{
    return id;
}

int ScopeTable::getNumBuckets() const
{
    return numBuckets;
}

void ScopeTable::setParentScope(ScopeTable *parent)
{
    parentScope = parent;
}

int ScopeTable::getCollisionCount() const
{
    return collisionCount;
}

///////////////////////////////////////////////////////////////////////////
int ScopeTable::getRecentScope()
{
    return recentScope;
}
void ScopeTable::setRecentScope(int scopeCount)
{
    this->recentScope=scopeCount;
}
///////////////////////////////////////////////////////////////////////////