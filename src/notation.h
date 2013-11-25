#pragma once
#include <string>

inline uint_fast8_t notation2Number(const std::string &s) {
    return (s[0] - 'a') + (s[1] - '1') * 8;
}

// kolumny od 0 do 7
inline uint_fast8_t file(uint_fast8_t number) {
    ASSERT(/*number >= 0 && */number < 64, number);
    return number & 7;
}

// wiersze od 0 do 7 (0 -> 'a', 1 -> 'b' ...)
inline uint_fast8_t rank(uint_fast8_t number) {
    ASSERT(/*number >= 0 && */number < 64, number);
    return number >> 3;
}

inline uint_fast8_t number(uint_fast8_t rank, uint_fast8_t file) {
    ASSERT(/*rank >= 0 && */rank < 8/* && file >= 0 */&& file < 8, rank, file);
    return (rank << 3) + file;
}

inline Color color(uint_fast8_t number) {
    ASSERT(/*number >= 0 && */number < 64, number);
    return number & 1 ? Color::white : Color::black;
}

inline Color color(uint_fast8_t rank, uint_fast8_t file) {
    ASSERT(/*rank >= 0 && */rank < 8/* && file >= 0 */&& file < 8, rank, file);
    return rank + file & 1 ? Color::white : Color::black;
}

inline bool isWhite(uint_fast8_t rank, uint_fast8_t file) {
    ASSERT(/*rank >= 0 &&*/ rank < 8 && /*file >= 0 && */file < 8, rank, file);
    return color(rank, file) == Color::white;
}

inline std::string number2Notation(uint_fast8_t pos) {
    char ans[3] = "\0\0";
    ans[0] = 'a' + rank(pos);
    ans[1] = '1' + file(pos);
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
    if (color == Color::white) {
        ret += 'a' - 'A';
    }
    return ret;
}
