#pragma once

#include "Piece.h"
#include "Color.h"
#include "EnumFlags.h"
#include <cstdint>
#include <iosfwd>

struct Move;

ENUM(BoardFlags, uint16_t,
    empty,    0x0000,
    w_k_rook, 0x0001,// = MoveFlags::w_k_rook_first
    w_q_rook, 0x0002,// = MoveFlags::w_q_rook_first
    b_k_rook, 0x0004,// = MoveFlags::b_k_rook_first
    b_q_rook, 0x0008,// = MoveFlags::b_q_rook_first
    w_king,   0x0010,// = MoveFlags::w_king_first
    b_king,   0x0020// = MoveFlags::b_king_first
)

static const EnumFlags<BoardFlags> white_castling_kingside = BoardFlags::w_k_rook | BoardFlags::w_king;
static const EnumFlags<BoardFlags> white_castling_queenside = BoardFlags::w_q_rook | BoardFlags::w_king;
static const EnumFlags<BoardFlags> black_castling_kingside = BoardFlags::b_k_rook | BoardFlags::b_king;
static const EnumFlags<BoardFlags> black_castling_queenside = BoardFlags::b_q_rook | BoardFlags::b_king;
static const EnumFlags<BoardFlags> castling = white_castling_kingside | white_castling_queenside | black_castling_kingside | black_castling_queenside;

#define START_BOARD \
    Piece::rook,  Piece::knight, Piece::bishop, Piece::queen, Piece::king,  Piece::bishop, Piece::knight, Piece::rook,\
    Piece::pawn,  Piece::pawn,   Piece::pawn,   Piece::pawn,  Piece::pawn,  Piece::pawn,   Piece::pawn,   Piece::pawn,\
    Piece::empty, Piece::empty,  Piece::empty,  Piece::empty, Piece::empty, Piece::empty,  Piece::empty,  Piece::empty,\
    Piece::empty, Piece::empty,  Piece::empty,  Piece::empty, Piece::empty, Piece::empty,  Piece::empty,  Piece::empty,\
    Piece::empty, Piece::empty,  Piece::empty,  Piece::empty, Piece::empty, Piece::empty,  Piece::empty,  Piece::empty,\
    Piece::empty, Piece::empty,  Piece::empty,  Piece::empty, Piece::empty, Piece::empty,  Piece::empty,  Piece::empty,\
    Piece::pawn,  Piece::pawn,   Piece::pawn,   Piece::pawn,  Piece::pawn,  Piece::pawn,   Piece::pawn,   Piece::pawn,\
    Piece::rook,  Piece::knight, Piece::bishop, Piece::queen, Piece::king,  Piece::bishop, Piece::knight, Piece::rook

#define START_COLORS \
    Color::white, Color::white, Color::white, Color::white, Color::white, Color::white, Color::white, Color::white,\
    Color::white, Color::white, Color::white, Color::white, Color::white, Color::white, Color::white, Color::white, \
    Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty,\
    Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty,\
    Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty,\
    Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty,\
    Color::black, Color::black, Color::black, Color::black, Color::black, Color::black, Color::black, Color::black,\
    Color::black, Color::black, Color::black, Color::black, Color::black, Color::black, Color::black, Color::black

struct Board {
    Piece pieces[64] = {START_BOARD};
    Color piecesColors[64] = {START_COLORS};
    uint_fast64_t bitmask[2][6] = {
        {0x000000000000FF00ULL, 0x0000000000000042ULL, 0x0000000000000024ULL, 0x0000000000000081ULL, 0x0000000000000008ULL, 0x0000000000000010ULL},
        {0x00FF000000000000ULL, 0x4200000000000000ULL, 0x2400000000000000ULL, 0x8100000000000000ULL, 0x0800000000000000ULL, 0x1000000000000000ULL}
    };
    EnumFlags<BoardFlags> flags = castling;
    Color toMove = Color::white;
    uint_fast8_t enPassantSquare = 0;
    void makeMove(const Move &r); // only move pieces around, no check for move validity
    void unmakeMove(const Move &r);
    void checkIntegrity();
    void dump(std::ostream &stream);
    uint64_t piecesOf(Color color);
    uint64_t allPieces();
private:
    void dumpRank(std::ostream &stream, uint_fast8_t rank);
    void dumpMask(std::ostream &stream, Piece piece);
    static void movePiece(Board &board, Piece piece, Color color, uint_fast8_t from, uint_fast8_t to);
    static void takePiece(Board &board, Piece piece, Color color, Piece opponentPiece, uint_fast8_t from, uint_fast8_t to);
    static void untakePiece(Board &board, Piece piece, Color color, Piece opponentPiece, uint_fast8_t from, uint_fast8_t to);
    static void disappearPiece(Board &board, Piece piece, Color color, uint_fast8_t from);
    static void appearPiece(Board &board, Piece piece, Color color, uint_fast8_t to);
};
