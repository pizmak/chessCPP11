#pragma once
#include <list>

#include "Board.h"
#include "Move.h"

class Engine {
public:
    using PairList = std::list<std::pair<uint8_t, uint8_t>>;
    Board board;
    Move moves[1024];
    Move *generateMoves(Move *startMove);
    Move *movesOfShortDistancePiece(uint8_t square, uint64_t mask, Move *startMove);
    Move *movesOfLongDistancePiece(uint8_t square, uint64_t mask[64][4], Move *startMove);
    template <Color color>
    Move *generatePawnMoves(uint8_t square, Move *startMove);
    Move *generateKnightMoves(uint8_t square, Move *startMove);
    Move *generateBishopMoves(uint8_t square, Move *startMove);
    Move *generateRookMoves(uint8_t square, Move *startMove);
    Move *generateQueenMoves(uint8_t square, Move *startMove);
    Move *generateKingMoves(uint8_t square, Move *startMove);

    void fillMoveFlags(Move &m);

    static uint64_t knightBitmask[64];
    static uint64_t bishopBitmask[64][4];
    static uint64_t rookBitmask[64][4];
    static uint64_t kingBitmask[64];
    static uint64_t maskOfShortDistancePiece(uint8_t square, const PairList &list);
    static void maskOfLongDistancePiece(uint8_t square, uint64_t array[4], const PairList &list);
    static uint64_t knightMask(uint8_t square);
    static void bishopMask(uint8_t square, uint64_t array[4]);
    static void rookMask(uint8_t square, uint64_t array[4]);
    static uint64_t kingMask(uint8_t square);
public:
    static void initBitmasks();
    void reset();
    // assume that from, to are set, rest fields will be set by engine
    void move(const std::string &move);
    void move(const std::list<std::string> &moves);
    Move go();
};
