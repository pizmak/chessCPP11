#pragma once

#include "GeneralizedZobristHash.h"

using BoardZobristArray = ZobristArray<64, 2, 8>;
using PlayerZobristArray = ZobristArray<1>;
using CastlingsZobristArray = ZobristArray<16>;
using EnPassantZobristArray = ZobristArray<8>;
using RepetitionZobristArray = ZobristArray<2>;

struct ChessZobristHash: GeneralizedZobristHash<BoardZobristArray, PlayerZobristArray, CastlingsZobristArray, EnPassantZobristArray, RepetitionZobristArray> {
    using HashType = GeneralizedZobristHash<BoardZobristArray, PlayerZobristArray, CastlingsZobristArray, EnPassantZobristArray, RepetitionZobristArray>;
    void switchPlayer();
    void updateCastlingCapabilities(uint8_t castlingCapabilities);
    void updateEnPassantFile(uint8_t enPassantFile);
    void updatePiece(uint8_t field, uint8_t color, uint8_t piece);
    void setRepetition(bool repeted);
    void resetRepetition();
    void initRepetition();
private:
    bool repetition = true;
    constexpr static int BOARD = 0;
    constexpr static int PLAYER = 1;
    constexpr static int CASTLINGS = 2;
    constexpr static int EN_PASSANT = 3;
    constexpr static int REPETITION = 4;
};
