#pragma once

#include "ZobristHash.h"
#include "ChessBoard.h"

using BoardType = ChessBoard<ZobristHashWithPlayerInfo<64, 2, 8>>;
