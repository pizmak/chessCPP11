#pragma once

#include <cstdint>
#include "EnumFlags.h"
#include "Piece.h"

ENUM(MoveFlags, uint16_t,
    empty,            0x0000,
    castling,         0x1000,
    enPassantCapture, 0x2000,
    queenPromotion,   0x0100,
    knightPromotion,  0x0200,
    rookPromotion,    0x0400,
    bishopPromotion,  0x0800,
    w_k_rook_first,   0x0001,// = BoardFlags::w_k_rook
    w_q_rook_first,   0x0002,// = BoardFlags::w_q_rook
    b_k_rook_first,   0x0004,// = BoardFlags::b_k_rook
    b_q_rook_first,   0x0008,// = BoardFlags::b_q_rook
    w_king_first,     0x0010,// = BoardFlags::w_king
    b_king_first,     0x0020 // = BoardFlags::b_king
)

static const EnumFlags<MoveFlags> promotions = MoveFlags::queenPromotion | MoveFlags::knightPromotion
        | MoveFlags::rookPromotion | MoveFlags::bishopPromotion;

struct Move {
    uint_fast8_t from;
    uint_fast8_t to;
    Piece captured; // empty in case of en passant capture
    EnumFlags<MoveFlags> flags;
    // always rewritten from Board enPassantSquare - unmakeMove should properly set boards enPassantSqare to previous value
    uint_fast8_t enPassantSquare;
    uint_fast16_t score;
};

std::ostream &operator<<(std::ostream &stream, const Move &move);

Piece promotionPiece(EnumFlags<MoveFlags> promotionType);
