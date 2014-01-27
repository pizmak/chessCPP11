#pragma once

#include <cstdint>

#include "common.h"
#include "utils/EnumFlags.h"
#include "Piece.h"

ENUM(MoveFlags, uint16_t,
    empty,            0x0000,
    castling,         0x1000,
    enPassantCapture, 0x2000,
    queenPromotion,   0x0100,
    knightPromotion,  0x0200,
    rookPromotion,    0x0400,
    bishopPromotion,  0x0800,
    K_castling,       K_CASTLING,
    Q_castling,       Q_CASTLING,
    k_castling,       k_CASTLING,
    q_castling,       q_CASTLING
)

static const EnumFlags<MoveFlags> promotions = MoveFlags::queenPromotion | MoveFlags::knightPromotion
        | MoveFlags::rookPromotion | MoveFlags::bishopPromotion;

struct Move {
    uint8_t from;
    uint8_t to;
    uint8_t enPassantSquare;
    Piece captured; // empty in case of en passant capture
    EnumFlags<MoveFlags> flags;
    // always rewritten from Board enPassantSquare - unmakeMove should properly set boards enPassantSqare to previous value
    int16_t score;
};

std::ostream &operator<<(std::ostream &stream, const Move &move);

Piece promotionPiece(EnumFlags<MoveFlags> promotionType);
MoveFlags piece2promotion(Piece piece);
