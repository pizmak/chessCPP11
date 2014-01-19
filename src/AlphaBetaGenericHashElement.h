#pragma once

#include "enums.h"
#include <cstdint>

ENUM(ScoreAccuracy, uint8_t,
    exact, 1,
    lowerBound, 2,
    upperBound, 3
)

struct AlphaBetaGenericHashElement {
    uint64_t hash;
    int16_t score;
    uint8_t depth;
    ScoreAccuracy accuracy;
    explicit operator bool() {
        return hash != 0;
    }
};
