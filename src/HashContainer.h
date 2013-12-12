#pragma once
#include <string.h>

template <typename HashedValue, uint64_t hashSizeInBits>
struct HashContainer {
    static const uint64_t hashMask = (2 << hashSizeInBits) - 1;
    HashedValue *hashValues;
    HashContainer() : hashValues(new HashedValue[hashMask + 1]) {
        memset(hashValues, 0, sizeof(HashedValue) * hashMask);
        std::cerr << "hash mask: " << hashMask << std::endl;
    }
    ~HashContainer() {
        delete [] hashValues;
    }
    inline void insert(uint64_t hash, const HashedValue &value) {
        if (hashValues[hash & hashMask].depth > value.depth) {
            return;
        }
        hashValues[hash & hashMask] = value;
    }
    inline HashedValue &get(uint64_t hash) {
        return hashValues[hash & hashMask];
    }
};
