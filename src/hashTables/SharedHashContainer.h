#pragma once

#include "AlphaBetaHashContainer.h"
#include "utils/Statistics.h"

template <uint64_t hashSizeInBits>
struct SharedHashContainer : AlphaBetaHashContainer<hashSizeInBits> {
    using BaseClass = AlphaBetaHashContainer<hashSizeInBits>;
    inline void insert(uint64_t hash, AlphaBetaGenericHashElement value) {
        value.hash ^= value.intData;
        BaseClass::insert(hash, value);
    }
    inline AlphaBetaGenericHashElement get(uint64_t hash) {
        static AlphaBetaGenericHashElement empty;
        AlphaBetaGenericHashElement ret = BaseClass::get(hash);
        ret.hash ^= ret.intData;
#ifdef DEBUG
        if (ret.hash != 0 && ret.hash & this->hashMask != hash & this->hashMask) {
            Statistics::globalStatistics().increment("hash.fatalFailure");
        }
#endif
        return ret.hash == hash ? ret : empty;
    }
};
