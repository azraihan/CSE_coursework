#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include <string>

class SymbolInfo {
private:
    std::string name;
    std::string type;
    SymbolInfo* next;

public:
    SymbolInfo(const std::string& name, const std::string& type);
    ~SymbolInfo();

    const std::string& getName() const;
    const std::string& getType() const;
    SymbolInfo* getNext() const;

    void setNext(SymbolInfo* next);
};

#endif