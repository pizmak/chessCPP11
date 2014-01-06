#pragma once

#include "NativeArray.h"
#include <functional>
#include <random>

template <int BoardSize, int...AdditionalSizes>
struct ZobristHash {
    using Array = NativeArray<uint64_t, BoardSize, AdditionalSizes...>;
    ZobristHash();
    template <typename ...IntTypes> void update(int field, IntTypes ...sizes);
    void clearHash() {
        hash = 0;
    }
    inline uint64_t getHash() const {
        return hash;
    }
protected:
    uint64_t hash = 0;
private:
    static typename Array::type randomHash;
    void initializeRandomHash();
};

template <int BoardSize, int...AdditionalSizes>
typename NativeArray<uint64_t, BoardSize, AdditionalSizes...>::type ZobristHash<BoardSize, AdditionalSizes...>::randomHash;

template <int BoardSize, int...AdditionalSizes>
ZobristHash<BoardSize, AdditionalSizes...>::ZobristHash() {
    static bool inited = false;
    if (!inited) {
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

template <int BoardSize, int...AdditionalSizes>
struct ZobristHashWithPlayerInfo : ZobristHash<BoardSize, AdditionalSizes...> {
    static uint64_t playerHash;
    void initializePlayerHash();
    ZobristHashWithPlayerInfo();
    void switchPlayer() {
        this->hash ^= playerHash;
    }
};

template <int BoardSize, int...AdditionalSizes>
uint64_t ZobristHashWithPlayerInfo<BoardSize, AdditionalSizes...>::playerHash;

template <int BoardSize, int...AdditionalSizes>
ZobristHashWithPlayerInfo<BoardSize, AdditionalSizes...>::ZobristHashWithPlayerInfo() {
    static bool inited = false;
    if (!inited) {
        initializePlayerHash();
        inited = true;
    }
}

template <int BoardSize, int...AdditionalSizes>
void ZobristHashWithPlayerInfo<BoardSize, AdditionalSizes...>::initializePlayerHash() {
    std::mt19937 engine;
    std::random_device rand;
    engine.seed(rand());
    std::uniform_int_distribution<uint64_t> distribution;
    playerHash = distribution(engine);
}
