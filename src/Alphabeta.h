#include <algorithm>

template <typename GameTraits, bool isMin, int depth>
struct Alphabeta {
    static int16_t go(typename GameTraits::State &state, int16_t alpha, int16_t beta, typename GameTraits::Move *spaceForMoves) {
        if (AlphaBetaGenericHashElement &hashed = GameTraits::getStateScore(state, depth)) {
            return hashed.score;
        }
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

        for (typename GameTraits::Move *move = spaceForMoves; move < afterLastMove; ++move) {
            GameTraits::makeMove(state, *move);
            int16_t result;
            if (depth == 1 && move->captured != Piece::empty) {
                result = Alphabeta<GameTraits, !isMin, 1 /* check deeper in case of capture */>::go(state, alpha, beta, afterLastMove);
            } else {
                result = Alphabeta<GameTraits, !isMin, depth - 1>::go(state, alpha, beta, afterLastMove);
            }
//            GameTraits::storeStateScore(state, result, depth - 1);
            if (isMin) {
                beta = std::min(beta, result);
            } else {
                alpha = std::max(alpha, result);
            }
            GameTraits::unmakeMove(state, *move);
            if (alpha >= beta) {
                break;
            }
        }
        return isMin ? beta : alpha;
    }
};

template <typename GameTraits, bool isMin>
struct Alphabeta<GameTraits, isMin, 0> {
    static inline int16_t go(typename GameTraits::State &state, int16_t alpha __attribute__((unused)), int16_t beta __attribute__((unused)), typename GameTraits::Move *spaceForMoves __attribute__((unused))) {
        if (AlphaBetaGenericHashElement &hashed = GameTraits::getStateScore(state, 0)) {
            return hashed.score;
        }
        int16_t result = GameTraits::evaluate(state);
        GameTraits::storeStateScore(state, result, 0);
        return result;
    }
};
