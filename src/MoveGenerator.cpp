#include "MoveGenerator.h"
#include "Move.h"
#include "notation.h"

#include <algorithm>

uint64_t MoveGenerator::pawnBitmask[2][64];
uint64_t MoveGenerator::knightBitmask[64];
uint64_t MoveGenerator::bishopBitmask[64][4];
uint64_t MoveGenerator::rookBitmask[64][4];
uint64_t MoveGenerator::kingBitmask[64];

void MoveGenerator::initBitmasks() {
    for (int square = 0; square < 64; ++square) {
        pawnBitmask[toInt(Color::white)][square] = pawnMask(square, Color::white);
        pawnBitmask[toInt(Color::black)][square] = pawnMask(square, Color::black);
        knightBitmask[square] = knightMask(square);
        bishopMask(square, bishopBitmask[square]);
        rookMask(square, rookBitmask[square]);
        kingBitmask[square] = kingMask(square);
    }
}

Move *MoveGenerator::movesOfShortDistancePiece(BoardType &board, uint8_t square, uint64_t mask, Move *startMove) {
    mask &= ~board.piecesOf(board.toMove);
    bit::foreach_bit(mask, [&board, &startMove, square](uint8_t moveTo) {
        *startMove = {square, moveTo, board.enPassantSquare, board.pieces[moveTo]};
        if (isMoveValid(board, *startMove)) {
            ++startMove;
        }
    });
    return startMove;
}

Move *MoveGenerator::movesOfLongDistancePiece(BoardType &board, uint8_t square, uint64_t mask[64][4], Move *startMove) {
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

Move *MoveGenerator::generateMoves(BoardType &board, Move *startMove) {
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::knight)], [&board, &startMove](uint8_t knight) {
        startMove = MoveGenerator::generateKnightMoves(board, knight, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::bishop)], [&board, &startMove](uint8_t bishop) {
        startMove = MoveGenerator::generateBishopMoves(board, bishop, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::rook)], [&board, &startMove](uint8_t rook) {
        startMove = MoveGenerator::generateRookMoves(board, rook, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::queen)], [&board, &startMove](uint8_t queen) {
        startMove = MoveGenerator::generateQueenMoves(board, queen, startMove);
    });
    bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::king)], [&board, &startMove](uint8_t king) {
        startMove = MoveGenerator::generateKingMoves(board, king, startMove);
    });
    if (board.toMove == Color::white) {
        bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::pawn)], [&board, &startMove](uint8_t pawn) {
            startMove = MoveGenerator::generatePawnMoves<Color::white>(board, pawn, startMove);
        });
    } else {
        bit::foreach_bit(board.bitmask[toInt(board.toMove)][toInt(Piece::pawn)], [&board, &startMove](uint8_t pawn) {
            startMove = MoveGenerator::generatePawnMoves<Color::black>(board, pawn, startMove);
        });
    }
    return startMove;
}

template <Color color>
Move *MoveGenerator::generatePawnMoves(BoardType &board, uint8_t square, Move *startMove) {
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

Move *MoveGenerator::generateKnightMoves(BoardType &board, uint8_t square, Move *startMove) {
    return movesOfShortDistancePiece(board, square, knightBitmask[square], startMove);
}

Move *MoveGenerator::generateBishopMoves(BoardType &board, uint8_t square, Move *startMove) {
    return movesOfLongDistancePiece(board, square, bishopBitmask, startMove);
}

Move *MoveGenerator::generateRookMoves(BoardType &board, uint8_t square, Move *startMove) {
    Move *afterLastMove = movesOfLongDistancePiece(board, square, rookBitmask, startMove);
    static auto QcastFun = [](Move &move) {
        move.flags |= MoveFlags::Q_castling;
    };
    static auto KcastFun = [](Move &move) {
        move.flags |= MoveFlags::K_castling;
    };
    static auto qcastFun = [](Move &move) {
        move.flags |= MoveFlags::q_castling;
    };
    static auto kcastFun = [](Move &move) {
        move.flags |= MoveFlags::k_castling;
    };
    if (square == 0 && board.flags & BoardFlags::Q_castling) {
        std::for_each(startMove, afterLastMove, QcastFun);
    } else if (square == 7 && board.flags & BoardFlags::K_castling) {
        std::for_each(startMove, afterLastMove, KcastFun);
    } else if (square == 0x38 && board.flags & BoardFlags::q_castling) {
        std::for_each(startMove, afterLastMove, qcastFun);
    } else if (square == 0x3F && board.flags & BoardFlags::k_castling) {
        std::for_each(startMove, afterLastMove, kcastFun);
    }
    return afterLastMove;
}

Move *MoveGenerator::generateQueenMoves(BoardType &board, uint8_t square, Move *startMove) {
    startMove = generateBishopMoves(board, square, startMove);
    return generateRookMoves(board, square, startMove);
}

Move *MoveGenerator::generateKingMoves(BoardType &board, uint8_t square, Move* startMove) {
    Move *afterLastMove = movesOfShortDistancePiece(board, square, kingBitmask[square], startMove);
    static auto QcastFun = [](Move &move) {
        move.flags |= MoveFlags::Q_castling;
    };
    static auto KcastFun = [](Move &move) {
        move.flags |= MoveFlags::K_castling;
    };
    static auto qcastFun = [](Move &move) {
        move.flags |= MoveFlags::q_castling;
    };
    static auto kcastFun = [](Move &move) {
        move.flags |= MoveFlags::k_castling;
    };
    if (square == n2N("e1") && board.flags & BoardFlags::K_castling) {
        std::for_each(startMove, afterLastMove, KcastFun);
        if (board.piecesColors[n2N("f1")] == Color::empty && board.piecesColors[n2N("g1")] == Color::empty && board.pieces[n2N("h1")] == Piece::rook && board.piecesColors[n2N("h1")] == Color::white &&
                !isSquareAttacked(board, n2N("e1"), Color::black) && !isSquareAttacked(board, n2N("f1"), Color::black) && !isSquareAttacked(board, n2N("g1"), Color::black)) {
            *afterLastMove = {n2N("e1"), n2N("g1"), board.enPassantSquare, Piece::empty, MoveFlags::K_castling | MoveFlags::castling};
            ++afterLastMove;
        }
    }
    if (square == n2N("e1") && board.flags & BoardFlags::Q_castling) {
        std::for_each(startMove, afterLastMove, QcastFun);
        if (board.piecesColors[n2N("d1")] == Color::empty && board.piecesColors[n2N("c1")] == Color::empty && board.piecesColors[n2N("b1")] == Color::empty &&  board.pieces[n2N("a1")] == Piece::rook && board.piecesColors[n2N("a1")] == Color::white &&
                !isSquareAttacked(board, n2N("e1"), Color::black) && !isSquareAttacked(board, n2N("d1"), Color::black) && !isSquareAttacked(board, n2N("c1"), Color::black)) {
            *afterLastMove = {n2N("e1"), n2N("c1"), board.enPassantSquare, Piece::empty, MoveFlags::Q_castling | MoveFlags::castling};
            if (board.flags & BoardFlags::K_castling) {
                afterLastMove->flags |= MoveFlags::K_castling;
            }
            ++afterLastMove;
        }
        return afterLastMove;
    }
    if (square == n2N("e8") && board.flags & BoardFlags::k_castling) {
        std::for_each(startMove, afterLastMove, kcastFun);
        if (board.piecesColors[n2N("f8")] == Color::empty && board.piecesColors[n2N("g8")] == Color::empty && board.pieces[n2N("h8")] == Piece::rook && board.piecesColors[n2N("h8")] == Color::black &&
                !isSquareAttacked(board, n2N("e8"), Color::white) && !isSquareAttacked(board, n2N("f8"), Color::white) && !isSquareAttacked(board, n2N("g8"), Color::white)) {
            *afterLastMove = {n2N("e8"), n2N("g8"), board.enPassantSquare, Piece::empty, MoveFlags::k_castling | MoveFlags::castling};
            ++afterLastMove;
        }
    }
    if (square == n2N("e8") && board.flags & BoardFlags::q_castling) {
        std::for_each(startMove, afterLastMove, qcastFun);
        if (board.piecesColors[n2N("d8")] == Color::empty && board.piecesColors[n2N("c8")] == Color::empty && board.piecesColors[n2N("b8")] == Color::empty &&  board.pieces[n2N("a8")] == Piece::rook && board.piecesColors[n2N("a8")] == Color::black &&
                !isSquareAttacked(board, n2N("e8"), Color::white) && !isSquareAttacked(board, n2N("d8"), Color::white) && !isSquareAttacked(board, n2N("c8"), Color::white)) {
            *afterLastMove = {n2N("e8"), n2N("c8"), board.enPassantSquare, Piece::empty, MoveFlags::q_castling | MoveFlags::castling};
            if (board.flags & BoardFlags::k_castling) {
                afterLastMove->flags |= MoveFlags::k_castling;
            }
            ++afterLastMove;
        }
    }
    return afterLastMove;
}

uint64_t MoveGenerator::maskOfShortDistancePiece(uint8_t square, const PairList &list) {
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

void MoveGenerator::maskOfLongDistancePiece(uint8_t square, uint64_t array[4], const PairList &list) {
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

uint64_t MoveGenerator::pawnMask(uint8_t square, Color pawnColor) {
    static PairList whitePawnList = {{1, -1}, {1, 1}};
    static PairList blackPawnList = {{-1, -1}, {-1, 1}};
    return maskOfShortDistancePiece(square, pawnColor == Color::white ? whitePawnList : blackPawnList);
}

uint64_t MoveGenerator::knightMask(uint8_t square) {
    static PairList list = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
    return maskOfShortDistancePiece(square, list);
}

void MoveGenerator::bishopMask(uint8_t square, uint64_t array[4]) {
    static PairList list = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    maskOfLongDistancePiece(square, array, list);
}

void MoveGenerator::rookMask(uint8_t square, uint64_t array[4]) {
    static PairList list = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    maskOfLongDistancePiece(square, array, list);
}

uint64_t MoveGenerator::kingMask(uint8_t square) {
    static PairList list = {{1, -1}, {1, 0}, {1, 1}, {0, -1}, {0, 1}, {-1, -1}, {-1, 0}, {-1, 1}};
    return maskOfShortDistancePiece(square, list);
}

bool MoveGenerator::isMoveValid(BoardType &board, const Move &m) {
    board.makeMove(m);
    bool ret = isSquareAttacked(board, bit::mostSignificantBit(board.bitmask[toInt(opponent(board.toMove))][toInt(Piece::king)]), board.toMove);
    board.unmakeMove(m);
    return !ret;
}

bool MoveGenerator::isSquareAttacked(BoardType &board, uint8_t square, Color color) {
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

