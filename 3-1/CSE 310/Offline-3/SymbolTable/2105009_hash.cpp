#include "2105009_hash.h"

using namespace std;

unsigned int SDBMHash(string str, unsigned int num_buckets)
{
    unsigned int hash = 0;
    unsigned int len = str.length();

    for (unsigned int i = 0; i < len; i++)
    {
        // hash = ((str[i]) + (hash << 6) + (hash << 16) - hash) % num_buckets;
        hash = ((str[i]) + (hash << 6) + (hash << 16) - hash);
    }

    return hash;
}

// unsigned int SDBMHash(string str, unsigned int num_buckets)
// {
//     // Convert std::string to const char*
//     const char *p = str.c_str();

//     unsigned int hash = 0;
//     auto *str_ptr = (unsigned char *)p;
//     int c{};

//     while ((c = *str_ptr++))
//     {
//         hash = c + (hash << 6) + (hash << 16) - hash;
//     }

//     return hash;
// }

unsigned int DJB2Hash(string str, unsigned int num_buckets)
{
    unsigned int hash = 5381;
    unsigned int len = str.length();

    for (unsigned int i = 0; i < len; i++)
    {
        hash = ((hash << 5) + hash) + str[i];
        hash = hash % num_buckets;
    }

    return hash;
}

unsigned int FNV1aHash(string str, unsigned int num_buckets)
{
    // FNV-1a hash parameters
    const unsigned int FNV_PRIME = 16777619;
    const unsigned int FNV_OFFSET_BASIS = 2166136261;

    unsigned int hash = FNV_OFFSET_BASIS;
    unsigned int len = str.length();

    for (unsigned int i = 0; i < len; i++)
    {
        hash = hash ^ str[i];    // XOR the byte with the hash
        hash = hash * FNV_PRIME; // Multiply by the FNV prime
        hash = hash % num_buckets;
    }

    return hash;
}