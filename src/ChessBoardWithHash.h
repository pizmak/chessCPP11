#pragma once

#include "ChessBoard.h"

template <typename HashPolicy>
struct ChessBoardWithHash : ChessBoard, HashPolicy {
    ChessBoardWithHash();
    void initHash();
    void disappearPiece(Piece piece, Color color, uint8_t from) override;
    void appearPiece(Piece piece, Color color, uint8_t to) override;
    void takePiece(Piece piece, Color color, Piece capturedPiece, uint8_t from, uint8_t to) override;
    void untakePiece(Piece piece, Color color, Piece capturedPiece, uint8_t from, uint8_t to) override;
    void makeMove(const Move &r) override;
    void unmakeMove(const Move &r) override;
    void clear() override;
    void dump(std::ostream &stream) const override;
};

template <typename HashPolicy>
ChessBoardWithHash<HashPolicy>::ChessBoardWithHash() {
    initHash();
}

template <typename HashPolicy>
void ChessBoardWithHash<HashPolicy>::initHash() {
    HashPolicy::clearHash();
    for (int i = 0; i < 64; ++i) {
        if (pieces[i] != Piece::empty) {
            HashPolicy::update(i, toInt(piecesColors[i]), toInt(pieces[i]));
        }
    }
}

template <typename HashPolicy>
void ChessBoardWithHash<HashPolicy>::disappearPiece(Piece piece, Color color, uint8_t from) {
    HashPolicy::update(from, toInt(piecesColors[from]), toInt(pieces[from]));
    ChessBoard::disappearPiece(piece, color, from);
}

template <typename HashPolicy>
void ChessBoardWithHash<HashPolicy>::appearPiece(Piece piece, Color color, uint8_t to) {
    ChessBoard::appearPiece(piece, color, to);
    HashPolicy::update(to, toInt(piecesColors[to]), toInt(pieces[to]));
}

template <typename HashPolicy>
void ChessBoardWithHash<HashPolicy>::takePiece(Piece piece, Color color, Piece capturedPiece, uint8_t from, uint8_t to) {
    ChessBoard::takePiece(piece, color, capturedPiece, from, to);
    HashPolicy::update(to, toInt(opponent(color)), toInt(capturedPiece));
}

template <typename HashPolicy>
void ChessBoardWithHash<HashPolicy>::untakePiece(Piece piece, Color color, Piece capturedPiece, uint8_t from, uint8_t to) {
    HashPolicy::update(to, toInt(color), toInt(piece));
    ChessBoard::untakePiece(piece, color, capturedPiece, from, to);
}

template <typename HashPolicy>
void ChessBoardWithHash<HashPolicy>::makeMove(const Move &move) {
    ChessBoard::makeMove(move);
    HashPolicy::switchPlayer();
}

template <typename HashPolicy>
void ChessBoardWithHash<HashPolicy>::unmakeMove(const Move &move) {
    ChessBoard::unmakeMove(move);
    HashPolicy::switchPlayer();
}

template <typename HashPolicy>
void ChessBoardWithHash<HashPolicy>::clear() {
    ChessBoard::clear();
    HashPolicy::clearHash();
}

template <typename HashPolicy>
void ChessBoardWithHash<HashPolicy>::dump(std::ostream &stream) const {
    stream << std::hex << std::showbase << std::endl << "__________\tpiece\twhite\t\t\t\tblack" << std::endl;
    for (uint8_t rank = 7; /*rank >= 0 && */rank < 8; --rank) {
        stream << "|";
        dumpRank(stream, rank);
        stream << "|";
        switch (rank) {
        case 1:
            dumpMask(stream, Piece::pawn);
            break;
        case 2:
            dumpMask(stream, Piece::knight);
            break;
        case 3:
            dumpMask(stream, Piece::bishop);
            break;
        case 4:
            dumpMask(stream, Piece::rook);
            break;
        case 5:
            dumpMask(stream, Piece::queen);
            break;
        case 6:
            dumpMask(stream, Piece::king);
            break;
        case 0:
            stream << "\thash\t" << std::noshowbase << (uint64_t)this->hash << std::showbase;
            break;
        }
        stream << std::endl;
    }
    stream << std::noshowbase << "----------\ttoMove: " << toMove;
    if (flags != BoardFlags::empty) {
        stream << ", flags: " << flags;
    }
    if (enPassantSquare != 0) {
        stream << ", e.p. on: " << (int)enPassantSquare;
    }
    stream << std::dec << std::endl << std::endl;;
}
