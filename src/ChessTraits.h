#pragma once

#include "Board.h"
#include "Move.h"
#include "Engine.h"
#include "MoveGenerator.h"
#include "bit.h"
#include <climits>

struct ChessTraits {
    using State = std::pair<BoardType &, Engine &>;
    using Move = ::Move;
    static int16_t scoreState(State &state) {
        int16_t score = ScorePosition::scorePosition(state.first);
        return score;
    }
    static int16_t simpleScoreState(State &state) {
        return state.first.materialDifference;
    }

    static void scoreState(State &state, int16_t score, uint8_t level) {
        state.second.insert(state.first.hash, {state.first.hash, score, level});
    }
    static void scoreMove(Move &move, int16_t score) {
        move.score = score;
    }
    static int16_t scoreOf(const Move &move) {
        return move.score;
    }
    static AlphaBetaGenericHashElement &scoreOf(const State &state) {
        return state.second.get(state.first.hash);
    }
    static Move *generateMoves(State &state, Move *spaceForMoves) {
        return MoveGenerator::generateMoves(state.first, spaceForMoves);
    }
    static int16_t scoreFinalPosition(State &state) {
        if (MoveGenerator::isSquareAttacked(state.first, bit::mostSignificantBit(state.first.bitmask[toInt(state.first.toMove)][toInt(Piece::king)]), opponent(state.first.toMove))) {
            return state.first.toMove == Color::white ? std::numeric_limits<int16_t>::min() + state.second.alphaBetaDepth : std::numeric_limits<int16_t>::max() - state.second.alphaBetaDepth;
        } else {
            return 0;
        }
    }
    static void makeMove(State &state, Move &move) {
        state.first.makeMove(move);
    }
    static void unmakeMove(State &state, Move &move) {
        state.first.unmakeMove(move);
    }
};
