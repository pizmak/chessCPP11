#pragma once

#include "zobristHashing/ChessZobristHash.h"
#include "NoHash.h"
#include "ChessBoard.h"
#include "hashTables/SharedHashContainer.h"
#include "hashTables/NoHashContainer.h"

using BoardType = ChessBoard<ChessZobristHash>;
//using BoardType = ChessBoard<NoHash>;

using HashContainerType = SharedHashContainer<26>;
//using HashContainerType = NoHashContainer<AlphaBetaGenericHashElement>;
//using HashContainerType = AlphaBetaHashContainer<26>;
