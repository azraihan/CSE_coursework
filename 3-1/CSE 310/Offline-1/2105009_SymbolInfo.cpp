#include "2105009_SymbolInfo.h"

SymbolInfo::SymbolInfo(const std::string &name, const std::string &type)
{
    this->name = name;
    this->type = type;
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