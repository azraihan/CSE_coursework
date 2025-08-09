#ifndef SCOPETABLE_H
#define SCOPETABLE_H

#include "2105009_SymbolInfo.h"
#include <string>
#include <fstream>
#include <vector>

using namespace std;

// ScopeTable implementation
class ScopeTable
{
private:
    SymbolInfo **hashTable;
    ScopeTable *parentScope;
    int id;
    int numBuckets;

    int collisionCount;

    unsigned int (*hashFunction)(std::string, unsigned int);

    std::ofstream *out;

    ///////////////////////////////////////////////////////////////////////////
    // to keep track of the most recent scope number
    int recentScope = 0;
    // To get the id-string in the format "parent.child.granchild. - - -"
    string getConcatenatedIdString (ScopeTable *scopeTable);
    ///////////////////////////////////////////////////////////////////////////

public:
    ScopeTable(int numBuckets, int id, std::ofstream *out, unsigned int (*hashFunc)(std::string, unsigned int));
    ~ScopeTable();

    bool insert(const std::string &name, const std::string &type, const std::string &varDataType = "NULL", const std::string &functionReturnType="NULL", int functionArgumentNumber=-1, std::vector<std::string>parameterTypes={}, bool isGlobal = false, int stackBasePointerOffset=0);
    SymbolInfo *lookUp(const std::string &name, bool printMessage = true);
    bool deleteSymbol(const std::string &name);

    void print();

    ScopeTable *getParentScope() const;
    int getId() const;
    int getNumBuckets() const;
    int getCollisionCount() const;

    void setParentScope(ScopeTable *parent);

    ///////////////////////////////////////////////////////////////////////////
    int getRecentScope();
    void setRecentScope(int scopeCount);
    ///////////////////////////////////////////////////////////////////////////
};

#endif