#pragma once

#include <cstdint>
#include "enums.h"

// values of this enum are used as indices in arrays and cannot be changed
ENUM(Piece, uint_fast8_t,
    pawn,   0,
    knight, 1,
    bishop, 2,
    rook,   3,
    queen,  4,
    king,   5,
    empty,  6
)
