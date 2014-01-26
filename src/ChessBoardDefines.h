#pragma once

#include "ChessZobristHash.h"
#include "NoHash.h"
#include "ChessBoard.h"

using BoardType = ChessBoard<ChessZobristHash>;
//using BoardType = ChessBoard<NoHash>;
