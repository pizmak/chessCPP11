#pragma once
#include <list>

#include "Board.h"
#include "Move.h"

class Engine {
public:
    using PairList = std::list<std::pair<uint_fast8_t, uint_fast8_t>>;
    Board board;
    Move moves[1024];
    Move *generateMoves(Move *startMove);
    Move *movesOfShortDistancePiece(uint_fast8_t square, uint64_t mask, Move *startMove);
    Move *movesOfLongDistancePiece(uint_fast8_t square, uint64_t mask[64][4], Move *startMove);
    template <Color color>
    Move *generatePawnMoves(uint_fast8_t square, Move *startMove);
    Move *generateKnightMoves(uint_fast8_t square, Move *startMove);
    Move *generateBishopMoves(uint_fast8_t square, Move *startMove);
    Move *generateRookMoves(uint_fast8_t square, Move *startMove);
    Move *generateQueenMoves(uint_fast8_t square, Move *startMove);
    Move *generateKingMoves(uint_fast8_t square, Move *startMove);

    void fillMoveFlags(Move &m);

    static uint64_t knightBitmask[64];
    static uint64_t bishopBitmask[64][4];
    static uint64_t rookBitmask[64][4];
    static uint64_t kingBitmask[64];
    static uint64_t maskOfShortDistancePiece(uint_fast8_t square, const PairList &list);
    static void maskOfLongDistancePiece(uint_fast8_t square, uint64_t array[4], const PairList &list);
    static uint64_t knightMask(uint_fast8_t square);
    static void bishopMask(uint_fast8_t square, uint64_t array[4]);
    static void rookMask(uint_fast8_t square, uint64_t array[4]);
    static uint64_t kingMask(uint_fast8_t square);
public:
    static void initBitmasks();
    void reset();
    // assume that from, to are set, rest fields will be set by engine
    void move(const std::string &move);
    void move(const std::list<std::string> &moves);
    Move go();
};
