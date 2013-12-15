#pragma once

#include <cstring>

template <typename HashedValue, uint64_t hashSizeInBits>
struct HashContainer {
    static const uint64_t hashMask = (2 << hashSizeInBits) - 1;
    static const uint64_t hashSize = 2 << hashSizeInBits;
    HashedValue *hashValues;
    HashContainer() : hashValues(new HashedValue[hashSize]) {
        clear();
    }
    ~HashContainer() {
        delete [] hashValues;
    }
    inline void insert(uint64_t hash, const HashedValue &value) {
        hashValues[hash & hashMask] = value;
    }
    inline HashedValue &get(uint64_t hash) {
        return hashValues[hash & hashMask];
    }
    void clear() {
        // FIXME: for some reason use hashSize instead of hashMask crashes application!!!
        memset(hashValues, 0, sizeof(HashedValue) * hashMask);
    }
};
