#pragma once

#include "Piece.h"
#include "Color.h"
#include "utils/EnumFlags.h"
#include "common.h"
#include "History.h"

struct Move;

ENUM(BoardFlags, uint16_t,
    empty,    0x0000,
    K_castling, K_CASTLING,
    Q_castling, Q_CASTLING,
    k_castling, k_CASTLING,
    q_castling, q_CASTLING
)

static const EnumFlags<BoardFlags> castling = BoardFlags::K_castling | BoardFlags::Q_castling | BoardFlags::k_castling | BoardFlags::q_castling;

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

template <typename HashPolicy>
struct ChessBoard : HashPolicy {
    using ThisType = ChessBoard<HashPolicy>;
    static const int16_t piecesValues[];
    uint64_t materialDifference = 0;
    Piece pieces[64] = {START_BOARD};
    Color piecesColors[64] = {START_COLORS};
    uint_fast64_t bitmask[2][6] = {
        {0x000000000000FF00ULL, 0x0000000000000042ULL, 0x0000000000000024ULL, 0x0000000000000081ULL, 0x0000000000000008ULL, 0x0000000000000010ULL},
        {0x00FF000000000000ULL, 0x4200000000000000ULL, 0x2400000000000000ULL, 0x8100000000000000ULL, 0x0800000000000000ULL, 0x1000000000000000ULL}
    };
    EnumFlags<BoardFlags> flags = castling;
    Color toMove = Color::white;
    uint8_t enPassantSquare = 0;
    History<ThisType> history;
    bool isDraw(); // returns true if there were 50 moves without capture or pawn moves or or position was three times repeated
    void makeMove(const Move &r); // only move pieces around, no check for move validity
    void unmakeMove(const Move &r);
    void checkIntegrity() const;
    void dump(std::ostream &stream) const;
    uint64_t piecesOf(Color color) const;
    uint64_t allPieces() const;
    void disappearPiece(Piece piece, Color color, uint8_t from);
    void appearPiece(Piece piece, Color color, uint8_t to);
    void clear();
    ChessBoard();
    void print() const;
protected:
    void initHash();
    void dumpRank(std::ostream &stream, uint8_t rank) const;
    void dumpMask(std::ostream &stream, Piece piece) const;
    void movePiece(Piece piece, Color color, uint8_t from, uint8_t to);
    void takePiece(Piece piece, Color color, Piece opponentPiece, uint8_t from, uint8_t to);
    void untakePiece(Piece piece, Color color, Piece opponentPiece, uint8_t from, uint8_t to);
};

#include "ChessBoardImpl.h"
