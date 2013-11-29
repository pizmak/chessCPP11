#pragma once

#include <cstdint>
#include "asserts.h"

namespace bit {

template <typename IntType = unsigned long long>
inline IntType single(uint_fast8_t bitNumber) {
    return IntType(1) << bitNumber;
}

template <typename IntType>
inline bool isSet(IntType number, uint_fast8_t bitNumber) {
    return number & single(bitNumber);
}

template <typename IntType>
inline void unset(IntType &number, uint_fast8_t bitNumber) {
    number &= ~single(bitNumber);
}

template <typename IntType>
inline void set(IntType &number, uint_fast8_t bitNumber) {
    number |= single(bitNumber);
}

template <typename IntType>
inline IntType sum(IntType &number1, IntType number2) {
    return number1 | number2;
}

template <typename IntType>
inline IntType sub(IntType &number1, IntType number2) {
    return number1 & ~number2;
}

// argument must not be 0
inline uint_fast8_t mostSignificantBit(uint64_t number) {
    ASSERT(number, number);
    return 63 - __builtin_clzll(number);
}

// argument must not be 0
inline uint_fast8_t leastSignificantBit(uint64_t number) {
    ASSERT(number, number);
    return __builtin_ctzll(number);
}

inline uint_fast8_t numberOfOnes(uint64_t number) {
    return __builtin_popcountll(number);
}

template <typename Function>
inline void foreach_bit(uint64_t bitSet, Function f) {
    uint_fast8_t bitNumber;
    while (bitSet) {
        bitNumber = bit::mostSignificantBit(bitSet);
        f(bitNumber);
        bit::unset(bitSet, bitNumber);
    }
}

}
