#pragma once

#include "GeneralizedZobristHash.h"

using BoardZobristArray = ZobristArray<64, 2, 8>;
using PlayerZobristArray = ZobristArray<1>;
using CastlingsZobristArray = ZobristArray<16>;
using EnPassantZobristArray = ZobristArray<8>;

struct ChessZobristHash: GeneralizedZobristHash<BoardZobristArray, PlayerZobristArray, CastlingsZobristArray, EnPassantZobristArray> {
    using HashType = GeneralizedZobristHash<BoardZobristArray, PlayerZobristArray, CastlingsZobristArray, EnPassantZobristArray>;
    void switchPlayer();
    void updateCastlingCapabilities(uint8_t castlingCapabilities);
    void updateEnPassantFile(uint8_t enPassantFile);
    void updatePiece(uint8_t field, uint8_t color, uint8_t piece);
private:
    constexpr static int BOARD = 0;
    constexpr static int PLAYER = 1;
    constexpr static int CASTLINGS = 2;
    constexpr static int EN_PASSANT = 3;
};
