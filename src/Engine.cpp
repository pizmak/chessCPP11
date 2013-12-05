#include "Engine.h"

#include <string>
#include <set>
#include <algorithm>
#include <cctype>

#include "asserts.h"

#include "notation.h"
#include "ScorePosition.h"
#include "utils.h"
#include "alfabeta.h"

uint64_t Engine::pawnBitmask[2][64];
uint64_t Engine::knightBitmask[64];
uint64_t Engine::bishopBitmask[64][4];
uint64_t Engine::rookBitmask[64][4];
uint64_t Engine::kingBitmask[64];

void Engine::reset() {
    board = Board();
}

void Engine::move(const std::string &move) {
    Move m = parseMove(move);
    fillMoveFlags(board, m);
    board.makeMove(m);
}

void Engine::move(const std::list<std::string> &moves) {
    for (auto &m : moves) {
        move(m);
    }
}

void Engine::initBitmasks() {
    for (int square = 0; square < 64; ++square) {
        pawnBitmask[toInt(Color::white)][square] = pawnMask(square, Color::white);
        pawnBitmask[toInt(Color::black)][square] = pawnMask(square, Color::black);
        knightBitmask[square] = knightMask(square);
        bishopMask(square, bishopBitmask[square]);
        rookMask(square, rookBitmask[square]);
        kingBitmask[square] = kingMask(square);
    }
}

Move *Engine::generateMoves(Board &board, Move *startMove) {
    if (board.toMove == Color::white) {
        bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::pawn)], [&board, &startMove](uint8_t pawn) {
            startMove = generatePawnMoves<Color::white>(board, pawn, startMove);
        });
    } else {
        bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::pawn)], [&board, &startMove](uint8_t pawn) {
            startMove = generatePawnMoves<Color::black>(board, pawn, startMove);
        });
    }
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::knight)], [&board, &startMove](uint8_t knight) {
        startMove = generateKnightMoves(board, knight, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::bishop)], [&board, &startMove](uint8_t bishop) {
        startMove = generateBishopMoves(board, bishop, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::rook)], [&board, &startMove](uint8_t rook) {
        startMove = generateRookMoves(board, rook, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::queen)], [&board, &startMove](uint8_t queen) {
        startMove = generateQueenMoves(board, queen, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::king)], [&board, &startMove](uint8_t king) {
        startMove = generateKingMoves(board, king, startMove);
    });
    return startMove;
}

void Engine::fillMoveFlags(Board &board, Move &m) {
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
        } else if (m.from == 0x3E && board.flags & BoardFlags::k_castling) {
            m.flags |= MoveFlags::k_castling;
        } else if (m.from == 0x38 && board.flags & BoardFlags::q_castling) {
            m.flags |= MoveFlags::q_castling;
        }
    }
}

Move *Engine::movesOfShortDistancePiece(Board &board, uint8_t square, uint64_t mask, Move *startMove) {
    mask &= ~board.piecesOf(board.toMove);
    bit::foreach_bit(mask, [&board, &startMove, square](uint8_t moveTo) {
        *startMove = {square, moveTo, board.enPassantSquare, board.pieces[moveTo]};
        if (isMoveValid(board, *startMove)) {
            ++startMove;
        }
    });
    return startMove;
}

Move *Engine::movesOfLongDistancePiece(Board &board, uint8_t square, uint64_t mask[64][4], Move *startMove) {
    for (int direction = 0; direction < 4; ++direction) {
        uint64_t piecesOnLine = mask[square][direction] & board.allPieces();
        uint8_t possiblePiece = 0;
        uint64_t movesMask = mask[square][direction];
        if (piecesOnLine) {
            possiblePiece = (bit::single(square) < mask[square][direction] ?
                    bit::leastSignificantBit : bit::mostSignificantBit)(piecesOnLine);
            if (board.piecesColors[possiblePiece] != board.toMove) {
                *startMove = {square, possiblePiece, board.enPassantSquare, board.pieces[possiblePiece]};
                if (isMoveValid(board, *startMove)) {
                    ++startMove;
                }
            }
            movesMask = bit::sub(movesMask, mask[possiblePiece][direction]);
            bit::unset(movesMask, possiblePiece);
        }

        bit::foreach_bit(movesMask, [&board, square, &startMove](uint8_t targetSquare) {
            *startMove = {square, targetSquare, board.enPassantSquare, Piece::empty};
            if (isMoveValid(board, *startMove)) {
                ++startMove;
            }
        });
    }
    return startMove;
}

template <Color color>
Move *Engine::generatePawnMoves(Board &board, uint8_t square, Move *startMove) {
    static_assert(color == Color::white || color == Color::black, "");
    if (bit::isSet(pawnBitmask[toInt(color)][color == Color::white ? square - 8 : square + 8], board.enPassantSquare)) {
        *startMove = {square, color == Color::white ? uint8_t(board.enPassantSquare + 8) : uint8_t(board.enPassantSquare - 8), board.enPassantSquare, Piece::empty, MoveFlags::enPassantCapture};
        if (isMoveValid(board, *startMove)) {
            ++startMove;
        }
    }
    uint8_t targetSquare = color == Color::white ? square + 8 : square - 8;
    if (board.pieces[targetSquare] == Piece::empty) {
        *startMove = {square, targetSquare, board.enPassantSquare, Piece::empty};
        if (isMoveValid(board, *startMove)) {
            if (startMove->to > 0x37 || startMove->to < 8) {
                startMove[1] = startMove[2] = startMove[3] = *startMove;
                startMove->flags |= MoveFlags::queenPromotion;
                startMove[1].flags |= MoveFlags::rookPromotion;
                startMove[2].flags |= MoveFlags::bishopPromotion;
                startMove[3].flags |= MoveFlags::knightPromotion;
                startMove += 3;
            }
            ++startMove;
            uint8_t targetSquare = color == Color::white ? square + 16 : square - 16;
            bool inSecondLine = color == Color::white ? square < 0x10 : square > 0x2F;
            if (inSecondLine && board.pieces[targetSquare] == Piece::empty) {
                *startMove = {square, targetSquare, board.enPassantSquare, Piece::empty};
                if (isMoveValid(board, *startMove)) {
                    ++startMove;
                }
            }
        }
    }
    uint64_t captures = pawnBitmask[toInt(color)][square] & board.piecesOf(opponent(color));
    bit::foreach_bit(captures, [&board, &startMove, square](uint8_t targetSquare) {
        *startMove = {square, targetSquare, board.enPassantSquare, board.pieces[targetSquare]};
        if (isMoveValid(board, *startMove)) {
            if (startMove->to > 0x37 || startMove->to < 8) {
                startMove[1] = startMove[2] = startMove[3] = *startMove;
                startMove->flags |= MoveFlags::queenPromotion;
                startMove[1].flags |= MoveFlags::rookPromotion;
                startMove[2].flags |= MoveFlags::bishopPromotion;
                startMove[3].flags |= MoveFlags::knightPromotion;
                startMove += 3;
            }
            ++startMove;
        }
    });
    return startMove;
}

Move *Engine::generateKnightMoves(Board &board, uint8_t square, Move *startMove) {
    return movesOfShortDistancePiece(board, square, knightBitmask[square], startMove);
}

Move *Engine::generateBishopMoves(Board &board, uint8_t square, Move *startMove) {
    return movesOfLongDistancePiece(board, square, bishopBitmask, startMove);
}

Move *Engine::generateRookMoves(Board &board, uint8_t square, Move *startMove) {
    Move *afterLastMove = movesOfLongDistancePiece(board, square, rookBitmask, startMove);
    if (square == 0 && board.flags & BoardFlags::Q_castling) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::Q_castling;
        });
    } else if (square == 7 && board.flags & BoardFlags::K_castling) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::K_castling;
        });
    } else if (square == 0x38 && board.flags & BoardFlags::q_castling) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::q_castling;
        });
    } else if (square == 0x3E && board.flags & BoardFlags::k_castling) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::k_castling;
        });
    }
    return afterLastMove;
}

Move *Engine::generateQueenMoves(Board &board, uint8_t square, Move *startMove) {
    startMove = generateBishopMoves(board, square, startMove);
    return generateRookMoves(board, square, startMove);
}

Move *Engine::generateKingMoves(Board &board, uint8_t square, Move* startMove) {
    Move *afterLastMove = movesOfShortDistancePiece(board, square, kingBitmask[square], startMove);
    if (square == 4 && board.flags & BoardFlags::K_castling) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::K_castling;
        });
        if (board.piecesColors[5] == Color::empty && board.piecesColors[6] == Color::empty &&
                !isSquareAttacked(board, 4, Color::black) && !isSquareAttacked(board, 5, Color::black) && !isSquareAttacked(board, 6, Color::black)) {
            *afterLastMove = {4, 6, 0, Piece::empty, MoveFlags::K_castling};
            ++afterLastMove;
        }
    }
    if (square == 4 && board.flags & BoardFlags::Q_castling) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::Q_castling;
        });
        if (board.piecesColors[3] == Color::empty && board.piecesColors[2] == Color::empty && board.piecesColors[1] == Color::empty &&
                !isSquareAttacked(board, 4, Color::black) && !isSquareAttacked(board, 3, Color::black) && !isSquareAttacked(board, 2, Color::black)) {
            *afterLastMove = {4, 2, 0, Piece::empty, MoveFlags::Q_castling};
            ++afterLastMove;
        }
    }
    if (square == 0x3C && board.flags & BoardFlags::k_castling) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::k_castling;
        });
        if (board.piecesColors[0x3D] == Color::empty && board.piecesColors[0x3E] == Color::empty &&
                !isSquareAttacked(board, 0x3C, Color::white) && !isSquareAttacked(board, 0x3D, Color::white) && !isSquareAttacked(board, 0x3E, Color::white)) {
            *afterLastMove = {0x3C, 0x3E, 0, Piece::empty, MoveFlags::k_castling};
            ++afterLastMove;
        }
    }
    if (square == 0x3C && board.flags & BoardFlags::q_castling) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::q_castling;
        });
        if (board.piecesColors[0x3B] == Color::empty && board.piecesColors[0x3A] == Color::empty && board.piecesColors[0x39] == Color::empty &&
                !isSquareAttacked(board, 0x3C, Color::white) && !isSquareAttacked(board, 0x3B, Color::white) && !isSquareAttacked(board, 0x3A, Color::white)) {
            *afterLastMove = {0x3C, 0x3A, 0, Piece::empty, MoveFlags::q_castling};
            ++afterLastMove;
        }
    }

    return afterLastMove;
}

static const int alphaBetaDepth = 5;

struct ChessTraits {
    using State = Board;
    using Move = ::Move;
    static int16_t scorePosition(State &state) {
        return ScorePosition::scorePosition(state);
    }
    static void scoreMove(Move &move, int16_t score) {
        move.score = score;
    }
    static int16_t scoreOf(const Move &move) {
        return move.score;
    }
    static Move *generateMoves(State &state, Move *spaceForMoves) {
        return Engine::generateMoves(state, spaceForMoves);
    }
    static int16_t scoreFinalPosition(State &state) {
        if (Engine::isSquareAttacked(state, bit::mostSignificantBit(state.bitmask[toInt(state.toMove)][toInt(Piece::king)]), opponent(state.toMove))) {
            return state.toMove == Color::white ? std::numeric_limits<int16_t>::min() + alphaBetaDepth : std::numeric_limits<int16_t>::max() - alphaBetaDepth;
        } else {
            return 0;
        }
    }
    static void makeMove(State &state, Move &move) {
        state.makeMove(move);
    }
    static void unmakeMove(State &state, Move &move) {
        state.unmakeMove(move);
    }
};

void Engine::init() {
    initBitmasks();
    ScorePosition::initialize();
}

Move Engine::go() {
    ScorePosition::updateStageOfGame(board);
    board.dump(std::cerr);
    Move *afterLastMove = generateMoves(board, moves);
    if (moves == afterLastMove) {
        std::cerr << "no valid moves" << std::endl;
        return {notation2Number("e2"), notation2Number("e4") };
    }
    Move bestMove = moves[0];
    int multiplier = board.toMove == Color::white ? 1 : -1;
    bestMove.score = std::numeric_limits<int16_t>::max() * -multiplier;
    std::for_each(moves, afterLastMove, [this, afterLastMove, &bestMove, multiplier](Move &m) {
        board.makeMove(m);
        m.score = board.toMove == Color::black ?
                alfabeta<ChessTraits, true>(board, alphaBetaDepth, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max(), afterLastMove) :
                alfabeta<ChessTraits, false>(board, alphaBetaDepth, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max(), afterLastMove);
        board.unmakeMove(m);
        if (multiplier * m.score > multiplier * bestMove.score) {
            bestMove = m;
            std::cerr << "new bestmove whith score: " << m.score << std::endl;
        }
        std::cerr << m << std::endl;
    });
    return bestMove;
}

uint64_t Engine::maskOfShortDistancePiece(uint8_t square, const PairList &list) {
    uint64_t ret = 0;
    uint8_t _file = file(square);
    uint8_t _rank = rank(square);
    for (auto &pair : list) {
        if (inRange(_rank + pair.first, _file + pair.second)) {
            bit::set(ret, number(_rank + pair.first, _file + pair.second));
        }
    }
    return ret;
}

void Engine::maskOfLongDistancePiece(uint8_t square, uint64_t array[4], const PairList &list) {
    array[0] = array[1] = array[2] = array[3] = 0;
    uint8_t _file = file(square);
    uint8_t _rank = rank(square);
    int multiplier = 0;
    int direction = 0;
    for (auto &pair : list) {
        multiplier = 1;
        while (inRange(_rank + multiplier * pair.first, _file + multiplier * pair.second)) {
            bit::set(array[direction], number(_rank + multiplier * pair.first, _file + multiplier * pair.second));
            ++multiplier;
        }
        ++direction;
    }
}

uint64_t Engine::pawnMask(uint8_t square, Color pawnColor) {
    static PairList whitePawnList = {{1, -1}, {1, 1}};
    static PairList blackPawnList = {{-1, -1}, {-1, 1}};
    return maskOfShortDistancePiece(square, pawnColor == Color::white ? whitePawnList : blackPawnList);
}

uint64_t Engine::knightMask(uint8_t square) {
    static PairList list = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
    return maskOfShortDistancePiece(square, list);
}

void Engine::bishopMask(uint8_t square, uint64_t array[4]) {
    static PairList list = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    maskOfLongDistancePiece(square, array, list);
}

void Engine::rookMask(uint8_t square, uint64_t array[4]) {
    static PairList list = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    maskOfLongDistancePiece(square, array, list);
}

uint64_t Engine::kingMask(uint8_t square) {
    static PairList list = {{1, -1}, {1, 0}, {1, 1}, {0, -1}, {0, 1}, {-1, -1}, {-1, 0}, {-1, 1}};
    return maskOfShortDistancePiece(square, list);
}

bool Engine::isMoveValid(Board &board, const Move &m) {
    board.makeMove(m);
    bool ret = isSquareAttacked(board, bit::mostSignificantBit(board.bitmask[toInt(opponent(board.toMove))][toInt(Piece::king)]), board.toMove);
    board.unmakeMove(m);
    return !ret;
}

bool Engine::isSquareAttacked(Board &board, uint8_t square, Color color) {
    if (knightBitmask[square] & board.bitmask[toInt(color)][toInt(Piece::knight)]) {
        return true;
    }
    if (kingBitmask[square] & board.bitmask[toInt(color)][toInt(Piece::king)]) {
        return true;
    }
    if (pawnBitmask[toInt(opponent(color))][square] & board.bitmask[toInt(color)][toInt(Piece::pawn)]) {
        return true;
    }
    auto isAttackedBy = [&](uint64_t mask[64][4], Piece p) {
        for (int direction = 0; direction < 4; ++direction) {
            uint64_t piecesOnLine = mask[square][direction] & board.allPieces();
            uint8_t possiblePiece = 0;
            if (piecesOnLine) {
                possiblePiece = (bit::single(square) < mask[square][direction] ?
                        bit::leastSignificantBit : bit::mostSignificantBit)(piecesOnLine);
                if (board.piecesColors[possiblePiece] == color && (board.pieces[possiblePiece] == Piece::queen || board.pieces[possiblePiece] == p)) {
                    return true;
                }
            }
        }
        return false;
    };
    if (isAttackedBy(bishopBitmask, Piece::bishop)) {
        return true;
    }
    if (isAttackedBy(rookBitmask, Piece::rook)) {
        return true;
    }
    return false;
}

void Engine::setupFenPosition(std::list<std::string> fenPosition) {
    setupFenPosition(board, fenPosition);
}

void Engine::setupFenPosition(Board &board, std::list<std::string> fenPosition) {
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
