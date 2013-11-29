#include "Engine.h"

#include <string.h>

#include "notation.h"
#include <set>
#include <algorithm>

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
    fillMoveFlags(m);
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

Move *Engine::generateMoves(Move *startMove) {
    if (board.toMove == Color::white) {
        bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::pawn)], [&startMove, this](uint8_t pawn) {
            startMove = generatePawnMoves<Color::white>(pawn, startMove);
        });
    } else {
        bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::pawn)], [&startMove, this](uint8_t pawn) {
            startMove = generatePawnMoves<Color::black>(pawn, startMove);
        });
    }
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::knight)], [&startMove, this](uint8_t knight) {
        startMove = generateKnightMoves(knight, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::bishop)], [&startMove, this](uint8_t bishop) {
        startMove = generateBishopMoves(bishop, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::rook)], [&startMove, this](uint8_t rook) {
        startMove = generateRookMoves(rook, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::queen)], [&startMove, this](uint8_t queen) {
        startMove = generateQueenMoves(queen, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::king)], [&startMove, this](uint8_t king) {
        startMove = generateKingMoves(king, startMove);
    });
    return startMove;
}

void Engine::fillMoveFlags(Move &m) {
    m.enPassantSquare = board.enPassantSquare;
    m.captured = board.pieces[m.to];
    if (board.pieces[m.from] == Piece::king && (m.from - m.to == 2 || m.to - m.from == 2)) {
        m.flags |= MoveFlags::castling;
    }
    if (board.pieces[m.from] == Piece::pawn && (board.enPassantSquare - m.to == 8 || m.to - board.enPassantSquare == 8)) {
        m.flags |= MoveFlags::enPassantCapture;
    }
}

Move *Engine::movesOfShortDistancePiece(uint8_t square, uint64_t mask, Move *startMove) {
    mask &= ~board.piecesOf(board.toMove);
    board.dump(std::cerr);
    printBitmaskAsBoard(mask, std::cerr);
    bit::foreach_bit(mask, [this, &startMove, square](uint8_t moveTo) {
        *startMove = {square, moveTo, board.enPassantSquare, board.pieces[moveTo]};
        if (isMoveValid(*startMove)) {
            ++startMove;
        }
    });
    return startMove;
}

Move *Engine::movesOfLongDistancePiece(uint8_t square, uint64_t mask[64][4], Move *startMove) {
    for (int direction = 0; direction < 4; ++direction) {
        uint64_t piecesOnLine = mask[square][direction] & board.allPieces();
        uint8_t possiblePiece = 0;
        uint64_t movesMask = mask[square][direction];
        if (piecesOnLine) {
            possiblePiece = (bit::single(square) < mask[square][direction] ?
                    bit::leastSignificantBit : bit::mostSignificantBit)(piecesOnLine);
            if (board.piecesColors[possiblePiece] != board.toMove) {
                *startMove = {square, possiblePiece, board.enPassantSquare, board.pieces[possiblePiece]};
                if (isMoveValid(*startMove)) {
                    ++startMove;
                }
            }
            movesMask = bit::sub(movesMask, mask[possiblePiece][direction]);
            bit::unset(movesMask, possiblePiece);
        }

        bit::foreach_bit(movesMask, [this, square, &startMove](uint8_t targetSquare) {
            *startMove = {square, targetSquare, board.enPassantSquare, Piece::empty};
            if (isMoveValid(*startMove)) {
                ++startMove;
            }
        });
    }
    return startMove;
}

template <Color color>
Move *Engine::generatePawnMoves(uint8_t square, Move *startMove) {
    uint8_t targetSquare = color == Color::white ? square + 8 : square - 8;
    if (board.pieces[targetSquare] == Piece::empty) {
        *startMove = {square, targetSquare, board.enPassantSquare, Piece::empty};
        if (isMoveValid(*startMove)) {
            ++startMove;
            uint8_t targetSquare = color == Color::white ? square + 16 : square - 16;
            bool inSecondLine = color == Color::white ? square < 0x10 : square > 0x2F;
            if (inSecondLine && board.pieces[targetSquare] == Piece::empty) {
                *startMove = {square, targetSquare, board.enPassantSquare, Piece::empty};
                if (isMoveValid(*startMove)) {
                    ++startMove;
                }
            }
        }
    }
    uint64_t captures = pawnBitmask[toInt(color)][square] & board.piecesOf(opponent(color));
    bit::foreach_bit(captures, [this, &startMove, square](uint8_t targetSquare) {
        *startMove = {square, targetSquare, board.enPassantSquare, board.pieces[targetSquare]};
        if (isMoveValid(*startMove)) {
            ++startMove;
        }
    });
    // TODO en passant capture
    return startMove;
}

Move *Engine::generateKnightMoves(uint8_t square, Move *startMove) {
    return movesOfShortDistancePiece(square, knightBitmask[square], startMove);
}

Move *Engine::generateBishopMoves(uint8_t square, Move *startMove) {
    return movesOfLongDistancePiece(square, bishopBitmask, startMove);
}

Move *Engine::generateRookMoves(uint8_t square, Move *startMove) {
    Move *afterLastMove = movesOfLongDistancePiece(square, rookBitmask, startMove);
    if (square == 0 && board.flags & BoardFlags::w_q_rook) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::w_q_rook_first;
        });
    } else if (square == 7 && board.flags & BoardFlags::w_k_rook) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::w_k_rook_first;
        });
    } else if (square == 0x38 && board.flags & BoardFlags::b_q_rook) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::b_q_rook_first;
        });
    } else if (square == 0x3E && board.flags & BoardFlags::b_k_rook) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::b_k_rook_first;
        });
    }
    return afterLastMove;
}

Move *Engine::generateQueenMoves(uint8_t square, Move *startMove) {
    startMove = generateBishopMoves(square, startMove);
    return generateRookMoves(square, startMove);
}

Move *Engine::generateKingMoves(uint8_t square, Move* startMove) {
    Move *afterLastMove = movesOfShortDistancePiece(square, kingBitmask[square], startMove);
    if (square == 4 && board.flags & BoardFlags::w_king) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::w_king_first;
        });
    } else if (square == 0x3C && board.flags & BoardFlags::b_king) {
        std::for_each(startMove, afterLastMove, [](Move &move) {
            move.flags |= MoveFlags::b_king_first;
        });
    }

    // TODO roszada
    return afterLastMove;
}

Move Engine::go() {
    Move *afterLastMove = generateMoves(moves);
    if (moves == afterLastMove) {
        std::cerr << "no valid moves" << std::endl;
        return {notation2Number("e2"), notation2Number("e4") };
    }
    std::for_each(moves, afterLastMove, [](Move &m) {
        std::cerr << m << std::endl;
    });
    return moves[0];
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

bool Engine::isMoveValid(const Move &m) {
    board.makeMove(m);
    bool ret = isSquareAttacked(bit::mostSignificantBit(board.bitmask[toInt(opponent(board.toMove))][toInt(Piece::king)]), board.toMove);
    board.unmakeMove(m);
    return !ret;
}

bool Engine::isSquareAttacked(uint8_t square, Color color) {
    if (knightBitmask[square] & board.bitmask[toInt(color)][toInt(Piece::knight)]) {
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
