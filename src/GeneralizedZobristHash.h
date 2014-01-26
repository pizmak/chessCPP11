#pragma once

#include "ZobristHash.h"
#include <tuple>

template <typename...ZobristArrays>
struct GeneralizedZobristHash {
    using Arrays = std::tuple<ZobristArrays...>;
    template <int hashIndex, typename ...IntTypes> void update(IntTypes ...indices);
    void clearHash();
    inline uint64_t getHash() const;
    GeneralizedZobristHash();
protected:
    uint64_t hash = 0;
};

template <typename Array>
inline void InitializeRandomHashes() {
    Array::initializeRandomHash();
}

template <typename Array1, typename Array2, typename...Arrays>
inline void InitializeRandomHashes() {
        Array1::initializeRandomHash();
        InitializeRandomHashes<Array2, Arrays...>();
}

template <typename...ZobristArrays>
GeneralizedZobristHash<ZobristArrays...>::GeneralizedZobristHash() {
    InitializeRandomHashes<ZobristArrays...>();
}

template <typename...ZobristArrays>
template <int hashIndex, typename ...IntTypes> void GeneralizedZobristHash<ZobristArrays...>::update(IntTypes ...indices) {
    using Array = typename std::tuple_element<hashIndex, Arrays>::type;
    static_assert(Array::DIMENSIONS == sizeof...(IntTypes), "invalid call to update function");
    hash ^= Array::getRandomHash(indices...);
}

template <typename...ZobristArrays>
void GeneralizedZobristHash<ZobristArrays...>::clearHash() {
    hash = 0;
}

template <typename...ZobristArrays>
uint64_t GeneralizedZobristHash<ZobristArrays...>::getHash() const {
    return hash;
}
