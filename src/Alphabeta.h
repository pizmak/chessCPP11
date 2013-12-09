#include <algorithm>

template <typename GameTraits, bool isMin, int depth>
struct Alphabeta {
    static int16_t go(typename GameTraits::State &state, int16_t alpha, int16_t beta, typename GameTraits::Move *spaceForMoves) {
        typename GameTraits::Move *afterLastMove = GameTraits::generateMoves(state, spaceForMoves); // zlicza ruchy mozliwe do wykonania
        if (spaceForMoves == afterLastMove) {
            return GameTraits::scoreFinalPosition(state) + (isMin ? depth : -depth);
        }

        //*************************************************************
        if (depth > 1) {
            // przedocena pozycji .. iteracyjne poglebianie
            for (typename GameTraits::Move *move = spaceForMoves; move < afterLastMove; ++move) {
                GameTraits::makeMove(state, *move);
                GameTraits::scoreMove(*move, GameTraits::scoreState(state)); //alfabeta(index, przeciwnik(kto_gra), GLEBOKOSC_INTERACYJNEGO_POGLEBIANIA, alpha, beta, wezel - 1);
                GameTraits::unmakeMove(state, *move);
            }
            // koniec przedoceny
            // SORTOWANIE RUCHOW

            std::sort(spaceForMoves, afterLastMove, [](const typename GameTraits::Move &m1, const typename GameTraits::Move &m2) {
                return isMin ? GameTraits::scoreOf(m1) < GameTraits::scoreOf(m2) : GameTraits::scoreOf(m1) > GameTraits::scoreOf(m2);
            });
            //*************************************************************
        }

        for (typename GameTraits::Move *move = spaceForMoves; move < afterLastMove; ++move) {
            GameTraits::makeMove(state, *move);
            int16_t result = Alphabeta<GameTraits, !isMin, depth - 1>::go(state, alpha, beta, afterLastMove);
            GameTraits::scoreState(state, result, depth);
            if (isMin) {
                beta = std::min(beta, result);
            } else {
                alpha = std::max(alpha, result);
            }
            GameTraits::unmakeMove(state, *move);
            if (alpha >= beta) {
                return isMin ? alpha : beta;
            }
        }
        return isMin ? beta : alpha;
    }
};

template <typename GameTraits, bool isMin>
struct Alphabeta<GameTraits, isMin, 0> {
    static int16_t go(typename GameTraits::State &state, int16_t alpha __attribute__((unused)), int16_t beta __attribute__((unused)), typename GameTraits::Move *spaceForMoves __attribute__((unused))) {
        return GameTraits::scoreState(state);
    }
};
