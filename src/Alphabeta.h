#include <algorithm>

template <typename GameTraits, bool isMin, int depth>
struct Alphabeta {
    static int16_t go(typename GameTraits::State &state, int16_t alpha, int16_t beta, typename GameTraits::Move *spaceForMoves) {
        AlphaBetaGenericHashElement &hashed = GameTraits::scoreOf(state);
        if (false && hashed.hash == state.first.hash && hashed.depth == depth) {
            return hashed.score;
        }
        typename GameTraits::Move *afterLastMove = GameTraits::generateMoves(state, spaceForMoves);
        if (spaceForMoves == afterLastMove) {
            return GameTraits::scoreFinalPosition(state) + (isMin ? depth : -depth);
        }

        if (depth > 1) {
            for (typename GameTraits::Move *move = spaceForMoves; move < afterLastMove; ++move) {
                GameTraits::makeMove(state, *move);
                GameTraits::scoreMove(*move, GameTraits::scoreState(state));
                GameTraits::unmakeMove(state, *move);
            }
            static auto sortFun = [](const typename GameTraits::Move &m1, const typename GameTraits::Move &m2) {
                return isMin ? GameTraits::scoreOf(m1) < GameTraits::scoreOf(m2) : GameTraits::scoreOf(m1) > GameTraits::scoreOf(m2);
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
//            GameTraits::scoreState(state, result, depth - 1);
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
        AlphaBetaGenericHashElement &hashed = GameTraits::scoreOf(state);
        if (hashed.hash == state.first.hash) {
            return hashed.score;
        }
        int16_t result = GameTraits::scoreState(state);
        GameTraits::scoreState(state, result, 0);
        return result;
    }
};
