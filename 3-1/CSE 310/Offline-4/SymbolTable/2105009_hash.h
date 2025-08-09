#ifndef HASH_H
#define HASH_H

#include <string>

unsigned int SDBMHash(std::string str, unsigned int num_buckets);
unsigned int DJB2Hash(std::string str, unsigned int num_buckets);
unsigned int FNV1aHash(std::string str, unsigned int num_buckets);

#endif