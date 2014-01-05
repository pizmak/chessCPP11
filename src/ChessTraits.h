#pragma once

#include "ChessBoard.h"
#include "Move.h"
#include "Engine.h"
#include "MoveGenerator.h"
#include "ChessEvaluator.h"
#include "bit.h"
#include <climits>

struct ChessTraits {
    using State = std::pair<BoardType &, Engine &>;
    using Move = ::Move;
    static int16_t evaluate(State &state) {
        return ChessEvaluator::evaluate(state.first);
    }
    static int16_t evaluateSimple(State &state) {
        return state.first.materialDifference;
    }
    static void storeStateScore(State &state, int16_t score, uint8_t level) {
        state.second.insert(state.first.hash, {state.first.hash, score, level});
    }
    static AlphaBetaGenericHashElement &getStateScore(const State &state, uint8_t minLevel) {
        static AlphaBetaGenericHashElement empty;
        AlphaBetaGenericHashElement &ret = state.second.get(state.first.hash);
        return ret.hash == state.first.hash && ret.depth == minLevel ? ret : empty;
    }
    static void storeMoveScore(Move &move, int16_t score) {
        move.score = score;
    }
    static int16_t getMoveScore(const Move &move) {
        return move.score;
    }
    static Move *generateMoves(State &state, Move *spaceForMoves) {
        return MoveGenerator::generateMoves(state.first, spaceForMoves);
    }
    static int16_t evaluateFinalPosition(State &state) {
        if (MoveGenerator::isSquareAttacked(state.first, bit::mostSignificantBit(state.first.bitmask[toInt(state.first.toMove)][toInt(Piece::king)]), opponent(state.first.toMove))) {
            return state.first.toMove == Color::white ? std::numeric_limits<int16_t>::min() + state.second.getAlphaBetaDepth() : std::numeric_limits<int16_t>::max() - state.second.getAlphaBetaDepth();
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
