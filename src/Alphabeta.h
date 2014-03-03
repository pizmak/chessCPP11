#include <algorithm>
#include "utils/Statistics.h"
#include <stdio.h>

thread_local static uint8_t maxDeepeningLevel = 5;
thread_local static uint8_t currentDepeningLevel = 0;
thread_local static int16_t evalSimpleBeginningOfDepening = 0;
int16_t threadAlpha, threadBeta;

template <typename GameTraits, bool isMin, int depth>
struct Alphabeta {
    static int16_t go(typename GameTraits::State &state, int16_t alpha, int16_t beta, typename GameTraits::Move *spaceForMoves, uint8_t level) {
        if (alpha < threadAlpha) alpha = threadAlpha;
        if (beta > threadBeta) beta = threadBeta;
        if (const AlphaBetaGenericHashElement &hashed = GameTraits::getStateScore(state, depth, alpha, beta)) {
            Statistics::globalStatistics().increment("hash.hits");
            return hashed.data.score;
        }
        Statistics::globalStatistics().increment("hash.misses");
        typename GameTraits::Move *afterLastMove = GameTraits::generateMoves(state, spaceForMoves);
        if (spaceForMoves == afterLastMove) {
            int16_t ret = GameTraits::evaluateFinalPosition(state);
            return  ret == 0 ? 0 : ret + (isMin ? -level : level);
        }

        if (depth > 1) {
            for (typename GameTraits::Move *move = spaceForMoves; move < afterLastMove; ++move) {
                GameTraits::makeMove(state, *move);
                state.first.checkIntegrity(*move);
                GameTraits::storeMoveScore(*move, GameTraits::evaluateSimple(state));
                Statistics::globalStatistics().increment("alphabeta.evalSimple");
                GameTraits::unmakeMove(state, *move);
                state.first.checkIntegrity(*move);
            }
            static auto sortFun = [](const typename GameTraits::Move &m1, const typename GameTraits::Move &m2) {
                return isMin ? GameTraits::getMoveScore(m1) < GameTraits::getMoveScore(m2) : GameTraits::getMoveScore(m1) > GameTraits::getMoveScore(m2);
            };
            std::sort(spaceForMoves, afterLastMove, sortFun);
        }

        ScoreAccuracy accuracy = ScoreAccuracy::exact;
        int16_t startValueOfAlphaBeta = isMin ? beta : alpha;
        int16_t bestResult = isMin ? std::numeric_limits<int16_t>::max() : std::numeric_limits<int16_t>::min();
        for (typename GameTraits::Move *move = spaceForMoves; move < afterLastMove; ++move) {
            GameTraits::makeMove(state, *move);
            state.first.checkIntegrity(*move);
            int16_t result;
            if (depth == 1 && (move->isCapture() /*|| afterLastMove - spaceForMoves < 8*/ /*to jest prawie na pewno szach, a nawet jak nie to sie przyda*/)) {
                if (currentDepeningLevel == 0) {
                    GameTraits::unmakeMove(state, *move);
                    evalSimpleBeginningOfDepening = GameTraits::evaluateSimple(state);
                    GameTraits::makeMove(state, *move);
                }
                bool stop = false;
                if (currentDepeningLevel == 1) {
                    int16_t actualEval = GameTraits::evaluateSimple(state);
                    int16_t multiplier = isMin ? -1 : 1;
                    if (multiplier * (evalSimpleBeginningOfDepening - actualEval) > 21) {
                        //std::cerr << "stopping!" << std::endl;
                        stop = true;
                    }
                }
                char buffer[3];
                snprintf(buffer, 3, "%d", currentDepeningLevel);
                Statistics::globalStatistics().increment(std::string("alphabeta.deepening[") + buffer + "]");

                ++currentDepeningLevel;
                result = currentDepeningLevel >= maxDeepeningLevel || stop ?
                    Alphabeta<GameTraits, !isMin, 0 /* check deeper in case of capture */>::go(state, alpha, beta, afterLastMove, level + 1) :
                    Alphabeta<GameTraits, !isMin, 1 /* check deeper in case of capture */>::go(state, alpha, beta, afterLastMove, level + 1);
                --currentDepeningLevel;
            } else {
                result = Alphabeta<GameTraits, !isMin, depth - 1>::go(state, alpha, beta, afterLastMove, level + 1);
            }
            GameTraits::unmakeMove(state, *move);
            state.first.checkIntegrity(*move);
            if (isMin) {
                beta = std::min(beta, result);
                bestResult = std::min(bestResult, result);
            } else {
                alpha = std::max(alpha, result);
                bestResult = std::max(bestResult, result);
            }
            if (alpha >= beta) {
                Statistics::globalStatistics().increment("alphabeta.cuts");
                accuracy = isMin ? ScoreAccuracy::upperBound : ScoreAccuracy::lowerBound;
                break;
            }
        }
        if (isMin) {
            if (beta == startValueOfAlphaBeta) {
                accuracy = ScoreAccuracy::lowerBound;
            }
        } else {
            if (alpha == startValueOfAlphaBeta) {
                accuracy = ScoreAccuracy::upperBound;
            }
        }
        GameTraits::storeStateScore(state, bestResult, depth, accuracy);
        return bestResult;
    }
};

template <typename GameTraits, bool isMin>
struct Alphabeta<GameTraits, isMin, 0> {
    static inline int16_t go(typename GameTraits::State &state, int16_t alpha __attribute__((unused)), int16_t beta __attribute__((unused)), typename GameTraits::Move *spaceForMoves __attribute__((unused)), uint8_t level __attribute__((unused))) {
        if (const AlphaBetaGenericHashElement &hashed = GameTraits::getStateScore(state, 0, alpha, beta)) {
            Statistics::globalStatistics().increment("hash.hits");
            return hashed.data.score;
        }
        Statistics::globalStatistics().increment("hash.misses");
        int16_t result = GameTraits::evaluate(state);
        GameTraits::storeStateScore(state, result, 0, ScoreAccuracy::exact);
        return result;
    }
};

#define CALL_ALPHA_BETA(isMin, i) \
    case i: \
        return Alphabeta<GameTraits, isMin, i>::go(state, alpha, beta,  spaceForMoves, 0);

template <typename GameTraits>
int16_t callAlphaBeta(typename GameTraits::State &state, bool isMin, int16_t alpha, int16_t beta, uint8_t depth, typename GameTraits::Move *spaceForMoves) {
    if (isMin) {
        switch (depth) {
            FOREACH_ARG(CALL_ALPHA_BETA, true, NO_SEPARATOR, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
        }
    } else {
        switch (depth) {
            FOREACH_ARG(CALL_ALPHA_BETA, false, NO_SEPARATOR, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
        }
    }
    std::cerr << "invalid value of alpha beta: " << depth << std::endl;
    return 0;
}
