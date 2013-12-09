#pragma once

#include <list>

#include "Board.h"
#include "Move.h"
#include "ChessHashElement.h"
#include "ZobristHash.h"

class Engine {
public:
    using PairList = std::list<std::pair<uint8_t, uint8_t>>;
    BoardType board;
    Move moves[1024];
    const static int shortHashMask = 0xFF;
//    const static int shortHashMask = 0x1FFFFFF;
    ChessHashElement hashes[shortHashMask + 1];
    static Move *generateMoves(BoardType &board, Move *startMove);
    static Move *movesOfShortDistancePiece(BoardType &board, uint8_t square, uint64_t mask, Move *startMove);
    static Move *movesOfLongDistancePiece(BoardType &board, uint8_t square, uint64_t mask[64][4], Move *startMove);
    template <Color color>
    static Move *generatePawnMoves(BoardType &board, uint8_t square, Move *startMove);
    static Move *generateKnightMoves(BoardType &board, uint8_t square, Move *startMove);
    static Move *generateBishopMoves(BoardType &board, uint8_t square, Move *startMove);
    static Move *generateRookMoves(BoardType &board, uint8_t square, Move *startMove);
    static Move *generateQueenMoves(BoardType &board, uint8_t square, Move *startMove);
    static Move *generateKingMoves(BoardType &board, uint8_t square, Move *startMove);

    static void fillMoveFlags(BoardType &board, Move &m);

    static uint64_t knightBitmask[64];
    static uint64_t bishopBitmask[64][4];
    static uint64_t rookBitmask[64][4];
    static uint64_t kingBitmask[64];
    static uint64_t pawnBitmask[2][64];
    static uint64_t maskOfShortDistancePiece(uint8_t square, const PairList &list);
    static void maskOfLongDistancePiece(uint8_t square, uint64_t array[4], const PairList &list);
    static uint64_t knightMask(uint8_t square);
    static uint64_t pawnMask(uint8_t square, Color pawnColor);
    static void bishopMask(uint8_t square, uint64_t array[4]);
    static void rookMask(uint8_t square, uint64_t array[4]);
    static uint64_t kingMask(uint8_t square);
    // do not check if king is attaking square
    static bool isSquareAttacked(BoardType &board, uint8_t square, Color color);
    static void setupFenPosition(BoardType &board, std::list<std::string> fenPosition);
    void setupFenPosition(std::list<std::string> fenPosition);
    static void initBitmasks();
public:
    static void init();
    void reset();
    // assume that from, to are set, rest fields will be set by engine
    void move(const std::string &move);
    void move(const std::list<std::string> &moves);
    static bool isMoveValid(BoardType &board, const Move &m);
    Move go();
};
