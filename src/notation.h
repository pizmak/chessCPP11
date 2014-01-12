#pragma once

#include <iostream>
#include <string>

#include "asserts.h"
#include "bit.h"
#include "logging.h"
#include "Move.h"
#include "Color.h"

inline bool inRange(uint8_t _rank, uint8_t _file) {
    return _rank < 8 && _file < 8;
}

inline uint8_t notation2Number(const std::string &s) {
    return (s[0] - 'a') + (s[1] - '1') * 8;
}

constexpr uint8_t n2N(const char* c) {
    return (*(c) - 'a') + (*(c+1) - '1') * 8;
}
// kolumny od 0 do 7
inline uint8_t file(uint8_t number) {
    ASSERT(/*number >= 0 && */number < 64, number);
    return number & 7;
}

// wiersze od 0 do 7 (0 -> 'a', 1 -> 'b' ...)
inline uint8_t rank(uint8_t number) {
    ASSERT(/*number >= 0 && */number < 64, number);
    return number >> 3;
}

inline uint8_t number(uint8_t rank, uint8_t file) {
    ASSERT(/*rank >= 0 && */rank < 8/* && file >= 0 */&& file < 8, (int)rank, (int)file);
    return (rank << 3) + file;
}

inline Color color(uint8_t number) {
    ASSERT(/*number >= 0 && */number < 64, number);
    return number & 1 ? Color::white : Color::black;
}

inline Color color(uint8_t rank, uint8_t file) {
    ASSERT(/*rank >= 0 && */rank < 8/* && file >= 0 */&& file < 8, rank, file);
    return rank + file & 1 ? Color::white : Color::black;
}

inline bool isWhite(uint8_t rank, uint8_t file) {
    ASSERT(/*rank >= 0 &&*/ rank < 8 && /*file >= 0 && */file < 8, rank, file);
    return color(rank, file) == Color::white;
}

inline std::string number2Notation(uint8_t pos) {
    char ans[3] = "\0\0";
    ans[0] = 'a' + file(pos);
    ans[1] = '1' + rank(pos);
    return ans;
}

inline char piece2Notation(Piece piece, Color color) {
    char ret;
    switch (piece) {
    case Piece::pawn:
        ret = 'P';
        break;
    case Piece::knight:
        ret = 'N';
        break;
    case Piece::bishop:
        ret = 'B';
        break;
    case Piece::rook:
        ret = 'R';
        break;
    case Piece::queen:
        ret = 'Q';
        break;
    case Piece::king:
        ret = 'K';
        break;
    default:
        ASSERT(false, piece);
        ret = ' ';
    }
    if (color == Color::black) {
        ret += 'a' - 'A';
    }
    return ret;
}

inline Piece notation2Piece(char notation) {
    switch (notation) {
    case 'p':
    case 'P':
        return Piece::pawn;
    case 'n':
    case 'N':
        return Piece::knight;
    case 'b':
    case 'B':
        return Piece::bishop;
    case 'r':
    case 'R':
        return Piece::rook;
    case 'q':
    case 'Q':
        return Piece::queen;
    case 'k':
    case 'K':
        return Piece::king;
    }
    ASSERT(false, notation);
    return Piece::empty;
}

inline Move parseMove(const std::string &move) {
    Move m{0, 0, 0, Piece::empty, MoveFlags::empty, 0};
    m.from = notation2Number(move.substr(0, 2));
    m.to = notation2Number(move.substr(2, 2));
    if (move.length() == 5) {
        ASSERT(m.to > 0x37 && m.to < 0x40 || m.to < 0x8, move);
        m.flags = piece2promotion(notation2Piece(move[4]));
    }
    return m;
}

inline std::string move2String(const Move &move) {
    std::string ret;
    TRACELN((int)move.from << ", " << (int)move.to);
    ret += number2Notation(move.from);
    ret += number2Notation(move.to);
    TRACELN(ret);
    if (move.flags & promotions) {
        ret += piece2Notation(promotionPiece(move.flags), Color::white);
    }
    return ret;
}

inline void printBitmaskAsBoard(uint64_t bitmask, std::ostream &stream) {
    for (uint8_t rank = 7; rank < 8; --rank) {
        for (uint8_t file = 0; file < 8; ++file) {
            stream << (bit::isSet(bitmask, number(rank, file)) ? "1" : "0");
        }
        stream << std::endl;
    }
}

template <Color color> constexpr uint8_t  forwardSquare(uint8_t square) {
    static_assert(color == Color::white || color == Color::black, "invalid color");
    return color == Color::white ? square + 8 : square - 8;
}
