#pragma once

#include <cstdint>
#include "enums.h"
#include "asserts.h"

// values of this enum are used as indices in arrays and cannot be changed
ENUM(Color, uint_fast8_t,
    white, 0,
    black, 1,
    empty, 2
)

inline Color opponent(Color color) {
    ASSERT(color == Color::white || color == Color::black, color);
    return Color(toInt(color) ^ 1);
}
