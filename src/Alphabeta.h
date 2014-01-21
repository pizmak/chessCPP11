#include <algorithm>

static uint8_t maxDeepeningLevel = 5;
static uint8_t currentDepeningLevel = 0;

template <typename GameTraits, bool isMin, int depth>
struct Alphabeta {
    static int16_t go(typename GameTraits::State &state, int16_t alpha, int16_t beta, typename GameTraits::Move *spaceForMoves) {
        if (AlphaBetaGenericHashElement &hashed = GameTraits::getStateScore(state, depth, alpha, beta)) {
            ++ChessEvaluator::hashHits;
            return hashed.score;
        }
        ++ChessEvaluator::hashMisses;
        typename GameTraits::Move *afterLastMove = GameTraits::generateMoves(state, spaceForMoves);
        if (spaceForMoves == afterLastMove) {
            return GameTraits::evaluateFinalPosition(state) + (isMin ? depth : -depth);
        }

        if (depth > 1) {
            for (typename GameTraits::Move *move = spaceForMoves; move < afterLastMove; ++move) {
                GameTraits::makeMove(state, *move);
                GameTraits::storeMoveScore(*move, GameTraits::evaluateSimple(state));
                GameTraits::unmakeMove(state, *move);
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
            int16_t result;
            if (depth == 1 && (move->captured != Piece::empty || afterLastMove - spaceForMoves < 8 /*to jest prawie na pewno szach, a nawet jak nie to sie przyda*/)) {
                ++currentDepeningLevel;
                result = currentDepeningLevel > maxDeepeningLevel ?
                        isMin ? std::numeric_limits<int16_t>::max() : std::numeric_limits<int16_t>::min() :
                        Alphabeta<GameTraits, !isMin, 1 /* check deeper in case of capture */>::go(state, alpha, beta, afterLastMove);
                --currentDepeningLevel;
            } else {
                result = Alphabeta<GameTraits, !isMin, depth - 1>::go(state, alpha, beta, afterLastMove);
            }
            GameTraits::unmakeMove(state, *move);
            if (isMin) {
                beta = std::min(beta, result);
                bestResult = std::min(bestResult, result);
            } else {
                alpha = std::max(alpha, result);
                bestResult = std::max(bestResult, result);
            }
            if (alpha >= beta) {
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
    static inline int16_t go(typename GameTraits::State &state, int16_t alpha __attribute__((unused)), int16_t beta __attribute__((unused)), typename GameTraits::Move *spaceForMoves __attribute__((unused))) {
        if (AlphaBetaGenericHashElement &hashed = GameTraits::getStateScore(state, 0, alpha, beta)) {
            ++ChessEvaluator::hashHits;
            return hashed.score;
        }
        ++ChessEvaluator::hashMisses;
        int16_t result = GameTraits::evaluate(state);
        GameTraits::storeStateScore(state, result, 0, ScoreAccuracy::exact);
        return result;
    }
};
