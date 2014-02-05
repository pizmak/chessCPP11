#pragma once

#include "utils/enums.h"
#include <cstdint>

ENUM(ScoreAccuracy, uint8_t,
    exact, 1,
    lowerBound, 2,
    upperBound, 3
)

struct AlphaBetaGenericHashElement {
    uint64_t hash;
    union {
        struct {
            int16_t score;
            uint8_t depth;
            ScoreAccuracy accuracy;
        } data;
        uint32_t intData;
    };
    explicit operator bool() const {
        return hash != 0;
    }
};
