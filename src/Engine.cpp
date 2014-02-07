#include "Engine.h"
#include "utils/asserts.h"
#include "notation.h"
#include "ChessEvaluator.h"
#include "Alphabeta.h"
#include "utils/ScopeTimer.h"
#include "MoveGenerator.h"
#include "ChessTraits.h"
#include "utils/Statistics.h"
#include <string>
#include <set>
#include <cctype>

void Engine::reset() {
    board = BoardType();
}

void Engine::move(const std::string &move) {
    Move m = parseMove(move);
    fillMoveFlags(board, m);
    board.makeMove(m);
}

void Engine::move(const Move &m) {
    board.makeMove(m);
}

void Engine::move(const std::list<std::string> &moves) {
    for (auto &m : moves) {
        move(m);
    }
}

void Engine::fillMoveFlags(BoardType &board, Move &m) {
    m.enPassantSquare = board.getEnPassantSquare();
    m.captured = board.getPiece(m.to);
    if (board.getPiece(m.from) == Piece::king) {
        if (board.getMoveSide() == Color::white && board.getFlags() & BoardFlags::K_castling) {
            m.flags |= MoveFlags::K_castling;
        }
        if (board.getMoveSide() == Color::white && board.getFlags() & BoardFlags::Q_castling) {
            m.flags |= MoveFlags::Q_castling;
        }
        if (board.getMoveSide() == Color::black && board.getFlags() & BoardFlags::k_castling) {
            m.flags |= MoveFlags::k_castling;
        }
        if (board.getMoveSide() == Color::black && board.getFlags() & BoardFlags::q_castling) {
            m.flags |= MoveFlags::q_castling;
        }
        if (m.from - m.to == 2 || m.to - m.from == 2) {
            m.flags |= MoveFlags::castling;
        }
    }
    if (board.getPiece(m.from) == Piece::pawn &&
            (board.getMoveSide() == Color::white && m.to - board.getEnPassantSquare() == 8 || board.getMoveSide() == Color::black && board.getEnPassantSquare() - m.to == 8)) {
        m.flags |= MoveFlags::enPassantCapture;
    }
    if (board.getPiece(m.from) == Piece::rook) {
        if (m.from == 7 && board.getFlags() & BoardFlags::K_castling) {
            m.flags |= MoveFlags::K_castling;
        } else if (m.from == 0 && board.getFlags() & BoardFlags::Q_castling) {
            m.flags |= MoveFlags::Q_castling;
        } else if (m.from == 0x3F && board.getFlags() & BoardFlags::k_castling) {
            m.flags |= MoveFlags::k_castling;
        } else if (m.from == 0x38 && board.getFlags() & BoardFlags::q_castling) {
            m.flags |= MoveFlags::q_castling;
        }
    }
}

Move Engine::go() {
    ScopeTimer timer("Move");
    stopped.store(false);
    Statistics::globalStatistics().checkpointAll();
    ChessEvaluator::updateStageOfGame(board);
    Move *afterLastMove = MoveGenerator::generateMoves(board, moves);
    ASSERT(afterLastMove > moves, "no moves");

    bool isMin = board.getMoveSide() == Color::black;
    int16_t alpha = std::numeric_limits<int16_t>::min(), beta = std::numeric_limits<int16_t>::max();
    Move bestMove = moves[0];
    int multiplier = isMin ? -1 : 1;
    if (afterLastMove - moves == 1) {
        return bestMove;
    }
    bestMove.score = isMin ? beta : alpha;

    ChessTraits::State state(board, *this);
    for (Move *m = moves; m < afterLastMove; ++m) {
        board.makeMove(*m);
        m->score = board.isDraw() ? 0 : callAlphaBeta<ChessTraits>(state, !isMin, alpha, beta, 1, afterLastMove);
        if (multiplier * m->score > multiplier * bestMove.score) {
            bestMove = *m;
//            (isMin ? beta : alpha) = m->score;
        }
        // TODO tutaj poglebianie ma sens? poza tym nie wiem czy nie lepiej dac np glebokosc 3 - powinno byc praktycznie tak samo szybko, a beda lepiej posortowane ruchy
        board.unmakeMove(*m);
    }

    static auto sortFun = [this](const Move &m1, const Move &m2) {
        return board.getMoveSide() == Color::black ? m1.score < m2.score : m1.score > m2.score;
    };
    std::sort(moves, afterLastMove, sortFun);

    alpha = std::numeric_limits<int16_t>::min();
    beta = std::numeric_limits<int16_t>::max();
    bestMove.score = isMin ? beta : alpha;
    for (Move *m = moves; m < afterLastMove; ++m) {
        board.makeMove(*m);
        m->score = board.isDraw() ? 0 : callAlphaBeta<ChessTraits>(state, !isMin, alpha, beta, alphaBetaDepth, afterLastMove);
        if (multiplier * m->score > multiplier * bestMove.score) {
            bestMove = *m;
            (isMin ? beta : alpha) = m->score;
            std::cerr << std::endl << *m << " - new best" << std::endl;
        } else {
            std::cerr << ".";
        }
        board.unmakeMove(*m);
        if (stopped) {
            break;
        }
    }
    std::cerr << std::endl;
    ScoreAccuracy scoreAccuracy = !stopped ? ScoreAccuracy::exact : board.getMoveSide() == Color::white ? ScoreAccuracy::lowerBound : ScoreAccuracy::upperBound;
    insert(board.getHash(), {board.getHash(), {bestMove.score, uint8_t(alphaBetaDepth + 1), scoreAccuracy}});
    std::cerr << std::endl;
    Statistics::globalStatistics().printAllSimple(std::cerr);
    std::cerr << std::endl;

// TODO watki operacje odczytu globalnych zmiennych i opracje ++ na nich powinny wykonywac bez synchronizacji?
//    board.history.printHistory();
    return bestMove;
}

void Engine::setupFenPosition(std::list<std::string> fenPosition) {
    setupFenPosition(board, fenPosition);
}

void Engine::setupFenPosition(BoardType &board, std::list<std::string> fenPosition) {
    ASSERT(fenPosition.size() == 6, "invalid fen position");
    board.initFromFen(fenPosition);
}

void Engine::clearHash() {
    clear();
}

void Engine::stop() {
    stopped = true;
}

uint8_t Engine::getAlphaBetaDepth() {
    return alphaBetaDepth;
}

void Engine::setAlphaBetaDepth(uint8_t alphaBetaDepth) {
    this->alphaBetaDepth = alphaBetaDepth;
}

const BoardType &Engine::getBoard() {
    return board;
}
