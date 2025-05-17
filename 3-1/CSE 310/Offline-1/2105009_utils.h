#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string trim(const std::string& str);

std::string formatType(const std::string& rawType);

bool validateCommand (char command, std::istringstream &iss, std::string &name, std::string &type, std::string &errorMsg);

#endif // UTILS_H