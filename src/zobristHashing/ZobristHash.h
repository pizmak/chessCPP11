#pragma once

#include "utils/NativeArray.h"
#include <functional>
#include <random>

template <int...Sizes>
struct ZobristArray {
    using Array = NativeArray<uint64_t, Sizes...>;
    static const int DIMENSIONS = sizeof...(Sizes);
    template <typename ...IntTypes>
    static inline uint64_t getRandomHash(IntTypes...indices);
    static typename Array::type randomHash;
    static void initializeRandomHash();
};

template <int...Sizes>
typename NativeArray<uint64_t, Sizes...>::type ZobristArray<Sizes...>::randomHash;

template <int...Sizes>
template <typename ...IntTypes> inline uint64_t ZobristArray<Sizes...>::getRandomHash(IntTypes...indices) {
    return Array::get(randomHash, indices...);
}

template <int...Sizes>
void ZobristArray<Sizes...>::initializeRandomHash() {
    static bool inited = false;
    if (inited) {
        return;
    }
    inited = true;
    std::mt19937 engine;
    std::random_device rand;
    engine.seed(rand());
    std::uniform_int_distribution<uint64_t> distribution;
    auto dist = std::bind(distribution, engine);
    Array::initMulti(dist, randomHash);
}

template <int...Sizes>
struct ZobristHash: ZobristArray<Sizes...> {
    template <typename ...IntTypes>
    void update(int field, IntTypes ...sizes);
    void clearHash();
    inline uint64_t getHash() const;
protected:
    uint64_t hash = 0;
};

template <int...Sizes>
void ZobristHash<Sizes...>::clearHash() {
    hash = 0;
}

template <int...Sizes>
inline uint64_t ZobristHash<Sizes...>::getHash() const {
    return hash;
}

template <int...Sizes>
template <typename ...IntTypes>
void ZobristHash<Sizes...>::update(int field, IntTypes ...sizes) {
    static_assert(sizeof...(Sizes) == sizeof...(sizes) + 1, "invalid call to update functions");
    hash ^= ZobristArray<Sizes...>::Array::get(this->randomHash, field, sizes...);
}
