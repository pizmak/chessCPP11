#pragma once

#include "ZobristHash.h"
#include "ChessBoardWithHash.h"

using BoardType = ChessBoardWithHash<ZobristHashWithPlayerInfo<64, 2, 8>>;
