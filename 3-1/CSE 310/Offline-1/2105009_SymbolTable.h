#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "2105009_ScopeTable.h"
#include "2105009_hash.h"
#include <string>
#include <fstream>

class SymbolTable
{
private:
    ScopeTable *currentScope;
    int numBuckets;
    int scopeCount;

    unsigned int (*hashFunction)(std::string, unsigned int);

    struct ScopeRecord
    {
        int scopeId;
        int collisionCount;
        bool isActive;
    };
    ScopeRecord *scopeRecords; // array of scope records
    int recordCapacity;        // Current capacity of the scope records
    int recordCount;           // Current number of records
    void expandRecordArray();
    void updateScopeRecord(int scopeId, int collisionCount, bool isActive);

    std::ofstream *out;

public:
    SymbolTable(int numBuckets, std::ofstream &out, unsigned int (*hashFunc)(std::string, unsigned int) = SDBMHash);
    ~SymbolTable();

    void enterScope();
    bool exitScope();

    bool insert(const std::string &name, const std::string &type);
    bool remove(const std::string &name);
    SymbolInfo *lookUp(const std::string &name);

    void printCurrentScopeTable();
    void printAllScopeTables();

    int getTotalCollisionCount() const;
};

#endif