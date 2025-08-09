#include "2105009_SymbolTable.h"
#include <iostream>

using namespace std;

SymbolTable::SymbolTable(int numBuckets, ofstream& out, unsigned int (*hashFunction)(std::string, unsigned int)) {
    this->numBuckets = numBuckets;
    this->scopeCount = 1; // Start with scope #1
    this->hashFunction = hashFunction;
    this->currentScope = new ScopeTable(numBuckets, scopeCount, &out, hashFunction);

    this->out = &out; 

    // Initialize the scope records array
    this->recordCapacity = 10;  // Initial capacity
    this->recordCount = 0;
    this->scopeRecords = new ScopeRecord[recordCapacity];
    
    updateScopeRecord(scopeCount, 0, true); // Record initial scope
}

SymbolTable::~SymbolTable() {
    // Delete all scope tables
    while (currentScope != nullptr) {
        ScopeTable* temp = currentScope;
        currentScope = currentScope->getParentScope();

        // Update record to inactive before deleting
        updateScopeRecord(temp->getId(), temp->getCollisionCount(), false);

        delete temp;
    }
    delete[] scopeRecords;
}

// Helper method to expand the record array when needed
void SymbolTable::expandRecordArray() {
    int newCapacity = recordCapacity * 2;
    ScopeRecord* newArray = new ScopeRecord[newCapacity];
    
    // Copy existing records
    for (int i = 0; i < recordCount; i++) {
        newArray[i] = scopeRecords[i];
    }
    
    // Replace old array with new one
    delete[] scopeRecords;
    scopeRecords = newArray;
    recordCapacity = newCapacity;
}

// Helper method to update or add a scope record
void SymbolTable::updateScopeRecord(int scopeId, int collisionCount, bool isActive) {
    // Check if record already exists
    for (int i = 0; i < recordCount; i++) {
        if (scopeRecords[i].scopeId == scopeId) {
            scopeRecords[i].collisionCount = collisionCount;
            scopeRecords[i].isActive = isActive;
            return;
        }
    }
    
    // expand the array if needed
    if (recordCount >= recordCapacity) {
        expandRecordArray();
    }
    
    // Add new record
    scopeRecords[recordCount].scopeId = scopeId;
    scopeRecords[recordCount].collisionCount = collisionCount;
    scopeRecords[recordCount].isActive = isActive;
    recordCount++;
}

///////////////////////////////////////////////////////////////////////////
// // for offline-1
// void SymbolTable::enterScope() {
//     scopeCount++;
//     ScopeTable* newScope = new ScopeTable(numBuckets, scopeCount, out, hashFunction);
//     newScope->setParentScope(currentScope);
//     currentScope = newScope;

//     // Add record for new scope
//     updateScopeRecord(scopeCount, 0, true);
// }

///////////////////////////////////////////////////////////////////////////
// // for offline-2 :((
void SymbolTable::enterScope() {
    scopeCount++;

    int recentScope = currentScope->getRecentScope();
    ScopeTable* newScope = new ScopeTable(numBuckets, recentScope+1, out, hashFunction);
    newScope->setParentScope(currentScope);
    currentScope = newScope;

    // Add record for new scope
    updateScopeRecord(scopeCount, 0, true);
}
///////////////////////////////////////////////////////////////////////////

bool SymbolTable::exitScope() {
    if (currentScope == nullptr) {
        // *out << "\tNo current scope" << endl;
        return false; // No scope to exit
    }
    
    if (currentScope->getParentScope() == nullptr) {
        // *out << "\tCannot remove ScopeTable with id " << currentScope->getId() << endl;
        return false; // Cannot exit the root scope
    }

    // Update record to inactive before deleting
    updateScopeRecord(currentScope->getId(), currentScope->getCollisionCount(), false);
    
    ScopeTable* temp = currentScope;
    currentScope = currentScope->getParentScope();
    delete temp;
    
    return true;
}


bool SymbolTable::insert(const string& name, const string& type, const std::string &varDataType, const std::string &functionReturnType, int functionArgumentNumber, std::vector<std::string>parameterTypes, bool isGlobal, int stackBasePointerOffset) {
    if (currentScope == nullptr) {
        // *out << "\tNo current scope" << endl;
        return false;
    }
    
    return currentScope->insert(name, type, varDataType, functionReturnType, functionArgumentNumber, parameterTypes, isGlobal, stackBasePointerOffset);
}

bool SymbolTable::remove(const string& name) {
    if (currentScope == nullptr) {
        // *out << "\tNo current scope" << endl;
        return false;
    }
    
    SymbolInfo* symbol = currentScope->lookUp(name, false);
    if (symbol == nullptr) {
        // *out << "\tNot found in the current ScopeTable" << endl;
        return false;
    }
    
    return currentScope->deleteSymbol(name);
}

///////////////////////////////////////////////////////////////////////////
// // lookUp() for offline-1
// SymbolInfo* SymbolTable::lookUp(const string& name) {
//     if (currentScope == nullptr) {
//         // *out << "\tNo current scope" << endl;
//         return nullptr;
//     }
    
//     ScopeTable* temp = currentScope;
    
//     while (temp != nullptr) {
//         SymbolInfo* symbol = temp->lookUp(name);

//         if (symbol != nullptr) {
//             return symbol;
//         }

//         temp = temp->getParentScope();
//     }
    
//     // *out <<"\t\'"<<name<<"\' not found in any of the ScopeTables"<< endl;
//     return nullptr;
// }

///////////////////////////////////////////////////////////////////////////
// // lookUp() for offline-2 (lexical analyzer)
SymbolInfo* SymbolTable::lookUp(const string& name) {
    if (currentScope == nullptr) {
        // *out << "\tNo current scope" << endl;
        return nullptr;
    }
    
    ScopeTable* temp = currentScope;
    
    while (temp != nullptr) {
        SymbolInfo* symbol = temp->lookUp(name);

        if (symbol != nullptr) {
            return symbol;
        }
        
        return nullptr;
        // temp = temp->getParentScope();
    }
    
    // *out <<"\t\'"<<name<<"\' not found in any of the ScopeTables"<< endl;
    return nullptr;
}
// new for antlr
SymbolInfo* SymbolTable::lookUpAll(const string& name) {
    if (currentScope == nullptr) {
        // *out << "\tNo current scope" << endl;
        return nullptr;
    }
    
    ScopeTable* temp = currentScope;
    
    while (temp != nullptr) {
        SymbolInfo* symbol = temp->lookUp(name);

        if (symbol != nullptr) {
            return symbol;
        }

        temp = temp->getParentScope();
    }
    
    // *out <<"\t\'"<<name<<"\' not found in any of the ScopeTables"<< endl;
    return nullptr;
}
///////////////////////////////////////////////////////////////////////////

void SymbolTable::printCurrentScopeTable() {
    if (currentScope != nullptr) {
        currentScope->print();
    } else {
        *out << "\tNo current scope" << endl;
    }
}

///////////////////////////////////////////////////////////////////////////
// //for offline-1
// void SymbolTable::printAllScopeTables() {
//     if (currentScope == nullptr) {
//         *out << "\tNo current scope" << endl;
//         return;
//     }
    
//     ScopeTable* temp = currentScope;
//     int indentLevel = 0;
    
//     while (temp != nullptr) {
//         temp->print(indentLevel);
//         temp = temp->getParentScope();
//         indentLevel++;
//     }
// }

///////////////////////////////////////////////////////////////////////////

// modified for offline-2
void SymbolTable::printAllScopeTables() {
    if (currentScope == nullptr) {
        *out << "\tNo current scope" << endl;
        return;
    }
    
    ScopeTable* temp = currentScope;
    
    while (temp != nullptr) {
        temp->print();
        temp = temp->getParentScope();
    }
}

///////////////////////////////////////////////////////////////////////////

// Method to get total collision count
int SymbolTable::getTotalCollisionCount() const {
    int total = 0;
    
    // Sum all records
    for (int i = 0; i < recordCount; i++) {
        // For active scopes, get current value directly
        if (scopeRecords[i].isActive) {
            ScopeTable* temp = currentScope;
            while (temp != nullptr) {
                if (temp->getId() == scopeRecords[i].scopeId) {
                    total += temp->getCollisionCount();
                    break;
                }
                temp = temp->getParentScope();
            }
        } else {
            // For inactive scopes, use stored value
            total += scopeRecords[i].collisionCount;
        }
    }
    
    return total;
}

///////////////////////////////////////////////////////////////////////////
// newly added for ICG offline
int SymbolTable::getCurrentScope() const{ 
    return currentScope->getId();
}
///////////////////////////////////////////////////////////////////////////