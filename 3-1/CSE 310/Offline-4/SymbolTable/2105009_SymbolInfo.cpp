#include "2105009_SymbolInfo.h"

SymbolInfo::SymbolInfo(const std::string &name, const std::string &type, const std::string &varDataType, const std::string& functionReturnType, int functionArgumentNumber, std::vector<std::string>parameterTypes, bool isGlobal, int stackBasePointerOffset)
{
    this->name = name;
    this->type = type;
    this->varDataType = varDataType;
    this->functionReturnType = functionReturnType;
    this->functionArgumentNumber = functionArgumentNumber;
    this->parameterTypes = parameterTypes;
    this->stackBasePointerOffset = stackBasePointerOffset;
    this->isGlobal = isGlobal;
    this->next = nullptr;
}

SymbolInfo::~SymbolInfo()
{
}

const std::string &SymbolInfo::getName() const
{
    return name;
}

const std::string &SymbolInfo::getType() const
{
    return type;
}

SymbolInfo *SymbolInfo::getNext() const
{
    return next;
}

void SymbolInfo::setNext(SymbolInfo *next)
{
    this->next = next;
}

// added for antlr offline
////////////////////////////////////////////////////////////////////////////////////////////////
const std::string &SymbolInfo::getFunctionReturnType() const
{
    return functionReturnType;
}

const std::string &SymbolInfo::getVarDataType() const
{
    return varDataType;
}

const int SymbolInfo::getFunctionArgumentNumber() const
{
    return functionArgumentNumber;
}

const std::vector<std::string> SymbolInfo::getParameterTypes() const
{
    return parameterTypes;
}
////////////////////////////////////////////////////////////////////////////////////////////////


// added for ICG offline
////////////////////////////////////////////////////////////////////////////////////////////////
const int SymbolInfo::getStackBasePointerOffset() const
{
    return stackBasePointerOffset;
}

const bool SymbolInfo::getIsGlobal() const
{
    return isGlobal;
}
////////////////////////////////////////////////////////////////////////////////////////////////