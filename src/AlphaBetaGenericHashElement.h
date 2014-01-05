#pragma once

#include <cstdint>

struct AlphaBetaGenericHashElement {
    uint64_t hash;
    int16_t score;
    uint8_t depth;
    explicit operator bool() {
        return hash != 0;
    }
};
