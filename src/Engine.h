#pragma once

#include "Board.h"
#include "Move.h"
#include "ZobristHash.h"
#include "AlphaBetaHashContainer.h"

#include <atomic>
#include <list>

class Engine : public AlphaBetaHashContainer<26> {
public:
    BoardType board;
    Move moves[4096];
    uint8_t alphaBetaDepth = 5;
    template <bool isMin>
    int16_t callAlphaBeta(Move *moveStorage, int16_t alphaOrBeta);

    static void fillMoveFlags(BoardType &board, Move &m);

    static void setupFenPosition(BoardType &board, std::list<std::string> fenPosition);
    void setupFenPosition(std::list<std::string> fenPosition);
    std::atomic<bool> stopped;
public:
    void reset();
    // assume that from, to are set, rest fields will be set by engine
    void move(const Move &m);
    void move(const std::string &move);
    void move(const std::list<std::string> &moves);
    Move go();
    void clearHash();
    void stop();
};
