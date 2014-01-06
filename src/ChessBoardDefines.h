#pragma once

#include "ZobristHash.h"
#include "NoHash.h"
#include "ChessBoard.h"

using BoardType = ChessBoard<ZobristHashWithPlayerInfo<64, 2, 8>>;
//using BoardType = ChessBoard<NoHash>;
