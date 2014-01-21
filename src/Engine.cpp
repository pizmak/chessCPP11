#include "Engine.h"
#include "asserts.h"
#include "notation.h"
#include "ChessEvaluator.h"
#include "utils.h"
#include "Alphabeta.h"
#include "ScopeTimer.h"
#include "MoveGenerator.h"
#include "ChessTraits.h"

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
    m.enPassantSquare = board.enPassantSquare;
    m.captured = board.pieces[m.to];
    if (board.pieces[m.from] == Piece::king) {
        if (board.toMove == Color::white && board.flags & BoardFlags::K_castling) {
            m.flags |= MoveFlags::K_castling;
        }
        if (board.toMove == Color::white && board.flags & BoardFlags::Q_castling) {
            m.flags |= MoveFlags::Q_castling;
        }
        if (board.toMove == Color::black && board.flags & BoardFlags::k_castling) {
            m.flags |= MoveFlags::k_castling;
        }
        if (board.toMove == Color::black && board.flags & BoardFlags::q_castling) {
            m.flags |= MoveFlags::q_castling;
        }
        if (m.from - m.to == 2 || m.to - m.from == 2) {
            m.flags |= MoveFlags::castling;
        }
    }
    if (board.pieces[m.from] == Piece::pawn &&
            (board.toMove == Color::white && m.to - board.enPassantSquare == 8 || board.toMove == Color::black && board.enPassantSquare - m.to == 8)) {
        m.flags |= MoveFlags::enPassantCapture;
    }
    if (board.pieces[m.from] == Piece::rook) {
        if (m.from == 7 && board.flags & BoardFlags::K_castling) {
            m.flags |= MoveFlags::K_castling;
        } else if (m.from == 0 && board.flags & BoardFlags::Q_castling) {
            m.flags |= MoveFlags::Q_castling;
        } else if (m.from == 0x3F && board.flags & BoardFlags::k_castling) {
            m.flags |= MoveFlags::k_castling;
        } else if (m.from == 0x38 && board.flags & BoardFlags::q_castling) {
            m.flags |= MoveFlags::q_castling;
        }
    }
}

#define CALL_ALPHA_BETA(i) \
    case i: \
        return Alphabeta<ChessTraits, isMin, i>::go(state, \
                isMin ? alphaOrBeta : std::numeric_limits<int16_t>::min(), \
                !isMin ? alphaOrBeta : std::numeric_limits<int16_t>::max(), moveStorage);

template <bool isMin>
int16_t Engine::callAlphaBeta(Move *moveStorage, int16_t alphaOrBeta, uint8_t depth) {
    ChessTraits::State state(board, *this);
    switch (depth) {
        FOREACH(CALL_ALPHA_BETA, NO_SEPARATOR, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
    }
    std::cerr << "invalid value of alpha beta: " << depth << std::endl;
    return 0;
}

Move Engine::go() {
    ScopeTimer timer("Move");
    stopped.store(false);
    uint64_t numberOfCalls = ChessEvaluator::numberOfCalls;
    ChessEvaluator::updateStageOfGame(board);
    Move *afterLastMove = MoveGenerator::generateMoves(board, moves);
    ASSERT(afterLastMove > moves, "no moves");

    Move bestMove = moves[0];
    int multiplier = board.toMove == Color::white ? 1 : -1;
    if (afterLastMove - moves == 1) {
        return bestMove;
    }
    bestMove.score = std::numeric_limits<int16_t>::max() * -multiplier;

    for (Move *m = moves; m < afterLastMove; ++m) {
        board.makeMove(*m);
        m->score = board.toMove == Color::black ? callAlphaBeta<true>(afterLastMove, bestMove.score, 1) : callAlphaBeta<false>(afterLastMove, bestMove.score, 1);
        board.unmakeMove(*m);
    }

    static auto sortFun = [this](const Move &m1, const Move &m2) {
        return board.toMove == Color::black ? m1.score < m2.score : m1.score > m2.score;
    };
    std::sort(moves, afterLastMove, sortFun);

    for (Move *m = moves; m < afterLastMove; ++m) {
        board.makeMove(*m);
        m->score = board.toMove == Color::black ? callAlphaBeta<true>(afterLastMove, bestMove.score, alphaBetaDepth) : callAlphaBeta<false>(afterLastMove, bestMove.score, alphaBetaDepth);
        board.unmakeMove(*m);
        if (multiplier * m->score > multiplier * bestMove.score) {
            bestMove = *m;
            std::cerr << "\n" << *m << " - new best\n";
        } else {
            std::cerr << ".";
        }
        if (stopped) {
            break;
        }
    }
    ScoreAccuracy scoreAccuracy = !stopped ? ScoreAccuracy::exact : board.toMove == Color::white ? ScoreAccuracy::lowerBound : ScoreAccuracy::upperBound;
    insert(board.getHash(), {board.getHash(), bestMove.score, uint8_t(alphaBetaDepth + 1), scoreAccuracy});
    std::cerr << "\nnumber of calls to scorePosition: " << ChessEvaluator::numberOfCalls << "(" << ChessEvaluator::numberOfCalls - numberOfCalls << ")" << std::endl;
    return bestMove;
}

void Engine::setupFenPosition(std::list<std::string> fenPosition) {
    setupFenPosition(board, fenPosition);
}

void Engine::setupFenPosition(BoardType &board, std::list<std::string> fenPosition) {
    ASSERT(fenPosition.size() == 6, "invalid fen position");
    board.clear();
    if (fenPosition.size() != 6) {
        return;
    }
    std::list<std::string> ranks = split(fenPosition.front(), '/');
    fenPosition.pop_front();
    uint8_t _rank = 8;
    for (auto rank : ranks) {
        --_rank;
        uint8_t file = 0;
        while (rank.size() > 0) {
            if (std::isdigit(rank[0])) {
                file += rank[0] - '0';
            } else {
                Piece piece = notation2Piece(rank[0]);
                board.appearPiece(piece, rank[0] >= 'a' ? Color::black : Color::white, number(_rank, file));
                ++file;
            }
            rank = rank.substr(1);
        }
        ASSERT(file == 8, file);
    }
    ASSERT(_rank == 0, _rank);

    std::string toMove = fenPosition.front();
    fenPosition.pop_front();
    ASSERT(toMove == "w" || toMove == "b", toMove);
    board.toMove = toMove == "w" ? Color::white : Color::black;

    std::string castles = fenPosition.front();
    fenPosition.pop_front();
    if (castles.find("K") != std::string::npos) {
        board.flags |= BoardFlags::K_castling;
    }
    if (castles.find("Q") != std::string::npos) {
        board.flags |= BoardFlags::Q_castling;
    }
    if (castles.find("k") != std::string::npos) {
        board.flags |= BoardFlags::k_castling;
    }
    if (castles.find("q") != std::string::npos) {
        board.flags |= BoardFlags::q_castling;
    }

    std::string enPassant = fenPosition.front();
    fenPosition.pop_front();
    if (enPassant != "-") {
        board.enPassantSquare = notation2Number(enPassant);
    }
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
