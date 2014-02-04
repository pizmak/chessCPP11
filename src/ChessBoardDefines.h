#pragma once

#include "zobristHashing/ChessZobristHash.h"
#include "NoHash.h"
#include "ChessBoard.h"

using BoardType = ChessBoard<ChessZobristHash>;
//using BoardType = ChessBoard<NoHash>;
