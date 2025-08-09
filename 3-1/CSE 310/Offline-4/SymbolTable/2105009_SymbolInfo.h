#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include <string>
#include <vector>

class SymbolInfo
{
private:
    std::string name;
    std::string type;
    SymbolInfo *next;

    //////////////////////////////////////////////////
    std::string functionReturnType; // newly added for antlr offline
    int functionArgumentNumber;
    std::vector<std::string> parameterTypes;
    std::string varDataType;
    //////////////////////////////////////////////////

    //////////////////////////////////////////////////
    int stackBasePointerOffset; // newly added for ICG offline
    bool isGlobal;
    //////////////////////////////////////////////////

public:
    SymbolInfo(const std::string &name, const std::string &type, const std::string &varDataType = "NULL", const std::string &functionReturnType = "NULL", int functionArgumentNumber = -1, std::vector<std::string> parameterTypes = {}, bool isGlobal = false, int stackBasePointerOffset=0);
    ~SymbolInfo();

    const std::string &getName() const;
    const std::string &getType() const;
    SymbolInfo *getNext() const;

    void setNext(SymbolInfo *next);

    //////////////////////////////////////////////////
    const std::string &getFunctionReturnType() const; // newly added for antlr offline
    const std::string &getVarDataType() const;
    const int getFunctionArgumentNumber() const;
    const std::vector<std::string> getParameterTypes() const;
    //////////////////////////////////////////////////

    //////////////////////////////////////////////////
    const int getStackBasePointerOffset() const;
    const bool getIsGlobal() const;
    //////////////////////////////////////////////////
};

#endif