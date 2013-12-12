#pragma once

#include "NativeArray.h"
#include <functional>
#include <random>

template <int BoardSize, int...AdditionalSizes>
struct ZobristHash {
    using Array = NativeArray<uint64_t, BoardSize, AdditionalSizes...>;
    static typename Array::type randomHash;
    void initializeRandomHash();
    uint64_t hash = 0;
    ZobristHash();
    template <typename ...IntTypes> void update(int field, IntTypes ...sizes);
};

template <int BoardSize, int...AdditionalSizes>
typename NativeArray<uint64_t, BoardSize, AdditionalSizes...>::type ZobristHash<BoardSize, AdditionalSizes...>::randomHash;

template <int BoardSize, int...AdditionalSizes>
ZobristHash<BoardSize, AdditionalSizes...>::ZobristHash() {
    static bool inited = false;
    if (!inited) {
        std::cerr << "init hashes" << std::endl;
        initializeRandomHash();
        inited = true;
    }
}

template <int BoardSize, int...AdditionalSizes>
void ZobristHash<BoardSize, AdditionalSizes...>::initializeRandomHash() {
    std::mt19937 engine;
    std::random_device rand;
    engine.seed(rand());
    std::uniform_int_distribution<uint64_t> distribution;
    auto dist = std::bind(distribution, engine);
    Array::initMulti(dist, randomHash);
}

template <int BoardSize, int...AdditionalSizes>
template <typename ...IntTypes>
void ZobristHash<BoardSize, AdditionalSizes...>::update(int field, IntTypes ...sizes) {
    static_assert(sizeof...(AdditionalSizes) == sizeof...(sizes), "invalid call to update functions");
    hash ^= Array::get(randomHash, field, sizes...);
}
