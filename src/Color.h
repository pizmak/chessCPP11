#pragma once

#include <cstdint>
#include "utils/enums.h"
#include "utils/asserts.h"

// values of this enum are used as indices in arrays and cannot be changed
ENUM(Color, uint8_t,
    white, 0,
    black, 1,
    empty, 2
)

inline constexpr Color opponent(Color color) {
//    ASSERT(color == Color::white || color == Color::black, color);
    return Color(toInt(color) ^ 1);
}
