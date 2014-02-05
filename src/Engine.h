#pragma once

#include "ChessBoardDefines.h"
#include "Move.h"
#include "hashTables/SharedHashContainer.h"

#include <atomic>
#include <list>

class Engine : public SharedHashContainer<26> {
    BoardType board;
    Move moves[4096];
    uint8_t alphaBetaDepth = 5;
    template <bool isMin>
    int16_t callAlphaBeta(Move *moveStorage, int16_t alphaOrBeta, uint8_t depth);
    void fillMoveFlags(BoardType &board, Move &m);
    void setupFenPosition(BoardType &board, std::list<std::string> fenPosition);
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
    void setupFenPosition(std::list<std::string> fenPosition);
    uint8_t getAlphaBetaDepth();
    void setAlphaBetaDepth(uint8_t alphaBetaDepth);
    const BoardType &getBoard();
};
