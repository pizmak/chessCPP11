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

void MoveGenerator::addCastlingFlags(BoardType &board, Move &move) {
#ifndef DEBUG
    return; // in release mode there is no need to set castling flags for moves that captures rooks
    // but we want to do it in debug mode to call checkIntegrity in makeMove
#endif
      if (board.getMoveSide() == Color::black && board.getFlags() & BoardFlags::K_castling && move.to == board.getKingSideRookSquare<Color::white>()
          && move.captured == Piece::rook && board.getPieceColor(board.getKingSideRookSquare<Color::white>()) == Color::white) {
        move.flags |= MoveFlags::K_castling;
      }
      if (board.getMoveSide() == Color::black && board.getFlags() & BoardFlags::Q_castling && move.to == board.getQueenSideRookSquare<Color::white>()
          && move.captured == Piece::rook && board.getPieceColor(board.getQueenSideRookSquare<Color::white>()) == Color::white) {
        move.flags |= MoveFlags::Q_castling;
      }
      if (board.getMoveSide() == Color::white && board.getFlags() & BoardFlags::k_castling && move.to == board.getKingSideRookSquare<Color::black>()
          && move.captured == Piece::rook && board.getPieceColor(board.getKingSideRookSquare<Color::black>()) == Color::black) {
        move.flags |= MoveFlags::k_castling;
      }
      if (board.getMoveSide() == Color::white && board.getFlags() & BoardFlags::q_castling && move.to == board.getQueenSideRookSquare<Color::black>()
          && move.captured == Piece::rook && board.getPieceColor(board.getQueenSideRookSquare<Color::black>()) == Color::black) {
        move.flags |= MoveFlags::q_castling;
      }
}

Move *MoveGenerator::movesOfShortDistancePiece(BoardType &board, uint8_t square, uint64_t mask, Move *startMove, EnumFlags<MoveFlags> flags) {
    mask &= ~board.piecesOf(board.getMoveSide());
    bit::foreach_bit(mask, [&board, &startMove, square, flags](uint8_t moveTo) {
        *startMove = {square, moveTo, board.getEnPassantSquare(), board.getPiece(moveTo), flags};
        addCastlingFlags(board, *startMove);
        if (isMoveValid(board, *startMove)) {
            ++startMove;
        }
    });
    return startMove;
}

Move *MoveGenerator::movesOfLongDistancePiece(BoardType &board, uint8_t square, uint64_t mask[64][4], Move *startMove, EnumFlags<MoveFlags> flags) {
    for (int direction = 0; direction < 4; ++direction) {
        uint64_t piecesOnLine = mask[square][direction] & board.allPieces();
        uint8_t possiblePiece = 0;
        uint64_t movesMask = mask[square][direction];
        if (piecesOnLine) {
            possiblePiece = (bit::single(square) < mask[square][direction] ?
                    bit::leastSignificantBit : bit::mostSignificantBit)(piecesOnLine);
            if (board.getPieceColor(possiblePiece) != board.getMoveSide()) {
                *startMove = {square, possiblePiece, board.getEnPassantSquare(), board.getPiece(possiblePiece), flags};
                addCastlingFlags(board, *startMove);
                if (isMoveValid(board, *startMove)) {
                    ++startMove;
                }
            }
            movesMask = bit::sub(movesMask, mask[possiblePiece][direction]);
            bit::unset(movesMask, possiblePiece);
        }

        bit::foreach_bit(movesMask, [&board, square, &startMove, flags](uint8_t targetSquare) {
            *startMove = {square, targetSquare, board.getEnPassantSquare(), Piece::empty, flags};
            if (isMoveValid(board, *startMove)) {
                ++startMove;
            }
        });
    }
    return startMove;
}

Move *MoveGenerator::generateMoves(BoardType &board, Move *startMove) {
    if (board.isDraw()) { // no valid moves due to position repetition or 50 moves rule
        std::cerr << "draw" << std::endl;
        return startMove;
    }
    bit::foreach_bit(board.getBitmask(board.getMoveSide(), Piece::knight), [&board, &startMove](uint8_t knight) {
        startMove = MoveGenerator::generateKnightMoves(board, knight, startMove);
    });
    bit::foreach_bit(board.getBitmask(board.getMoveSide(), Piece::bishop), [&board, &startMove](uint8_t bishop) {
        startMove = MoveGenerator::generateBishopMoves(board, bishop, startMove);
    });
    bit::foreach_bit(board.getBitmask(board.getMoveSide(), Piece::rook), [&board, &startMove](uint8_t rook) {
        startMove = MoveGenerator::generateRookMoves(board, rook, startMove);
    });
    bit::foreach_bit(board.getBitmask(board.getMoveSide(), Piece::queen), [&board, &startMove](uint8_t queen) {
        startMove = MoveGenerator::generateQueenMoves(board, queen, startMove);
    });
    bit::foreach_bit(board.getBitmask(board.getMoveSide(), Piece::king), [&board, &startMove](uint8_t king) {
        startMove = MoveGenerator::generateKingMoves(board, king, startMove);
    });
    if (board.getMoveSide() == Color::white) {
        bit::foreach_bit(board.getBitmask(board.getMoveSide(), Piece::pawn), [&board, &startMove](uint8_t pawn) {
            startMove = MoveGenerator::generatePawnMoves<Color::white>(board, pawn, startMove);
        });
    } else {
        bit::foreach_bit(board.getBitmask(board.getMoveSide(), Piece::pawn), [&board, &startMove](uint8_t pawn) {
            startMove = MoveGenerator::generatePawnMoves<Color::black>(board, pawn, startMove);
        });
    }
    return startMove;
}

template <Color color>
Move *MoveGenerator::generatePawnMoves(BoardType &board, uint8_t square, Move *startMove) {
    static_assert(color == Color::white || color == Color::black, "");
    if (bit::isSet(pawnBitmask[toInt(color)][color == Color::white ? square - 8 : square + 8], board.getEnPassantSquare())) {
        *startMove = {square, color == Color::white ? uint8_t(board.getEnPassantSquare() + 8) : uint8_t(board.getEnPassantSquare() - 8), board.getEnPassantSquare(), Piece::empty, MoveFlags::enPassantCapture};
        addCastlingFlags(board, *startMove);
        if (isMoveValid(board, *startMove)) {
            ++startMove;
        }
    }
    uint8_t targetSquare = color == Color::white ? square + 8 : square - 8;
    if (board.getPiece(targetSquare) == Piece::empty) {
        *startMove = {square, targetSquare, board.getEnPassantSquare(), Piece::empty};
        if (isMoveValid(board, *startMove)) {
            if (startMove->to >= n2N("a8") || startMove->to <= n2N("h1")) {
                startMove[1] = startMove[2] = startMove[3] = *startMove;
                startMove->flags |= MoveFlags::queenPromotion;
                startMove[1].flags |= MoveFlags::rookPromotion;
                startMove[2].flags |= MoveFlags::bishopPromotion;
                startMove[3].flags |= MoveFlags::knightPromotion;
                startMove += 3;
            }
            ++startMove;
            uint8_t targetSquare = color == Color::white ? square + 16 : square - 16;
            bool inSecondLine = color == Color::white ? square <= n2N("h2") : square >= n2N("a7");
            if (inSecondLine && board.getPiece(targetSquare) == Piece::empty) {
                *startMove = {square, targetSquare, board.getEnPassantSquare(), Piece::empty};
                if (isMoveValid(board, *startMove)) {
                    ++startMove;
                }
            }
        }
    }
    uint64_t captures = pawnBitmask[toInt(color)][square] & board.piecesOf(opponent(color));
    bit::foreach_bit(captures, [&board, &startMove, square](uint8_t targetSquare) {
        *startMove = {square, targetSquare, board.getEnPassantSquare(), board.getPiece(targetSquare)};
        addCastlingFlags(board, *startMove);
        if (isMoveValid(board, *startMove)) {
            if (startMove->to >= n2N("a8") || startMove->to <= n2N("h1")) {
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
    EnumFlags<MoveFlags> flags{};
    if (square == board.getQueenSideRookSquare<Color::white>() && board.getFlags() & BoardFlags::Q_castling) {
        flags = MoveFlags::Q_castling;
    } else if (square == board.getKingSideRookSquare<Color::white>() && board.getFlags() & BoardFlags::K_castling) {
        flags = MoveFlags::K_castling;
    } else if (square == board.getQueenSideRookSquare<Color::black>() && board.getFlags() & BoardFlags::q_castling) {
        flags = MoveFlags::q_castling;
    } else if (square == board.getKingSideRookSquare<Color::black>() && board.getFlags() & BoardFlags::k_castling) {
        flags = MoveFlags::k_castling;
    }
    return movesOfLongDistancePiece(board, square, rookBitmask, startMove, flags);
}

Move *MoveGenerator::generateQueenMoves(BoardType &board, uint8_t square, Move *startMove) {
    startMove = generateBishopMoves(board, square, startMove);
    return generateRookMoves(board, square, startMove);
}

Move *MoveGenerator::generateKingMoves(BoardType &board, uint8_t square, Move *startMove) {
    EnumFlags<MoveFlags> flags{};
    if (square == board.getKingStartSquare<Color::white>() && board.getFlags() & BoardFlags::K_castling) {
        flags |= MoveFlags::K_castling;
        if (whiteKingSideCastlingPossible(board)) {
            *startMove = {board.getKingStartSquare<Color::white>(), n2N("g1"), board.getEnPassantSquare(), Piece::empty, MoveFlags::K_castling | MoveFlags::castling};
            if (board.getFlags() & BoardFlags::Q_castling) {
                startMove->flags |= MoveFlags::Q_castling;
            }
            ++startMove;
        }
    }
    if (square == board.getKingStartSquare<Color::white>() && board.getFlags() & BoardFlags::Q_castling) {
        flags |= MoveFlags::Q_castling;
        if (whiteQueenSideCastlingPossible(board)) {
            *startMove = {board.getKingStartSquare<Color::white>(), n2N("c1"), board.getEnPassantSquare(), Piece::empty, MoveFlags::Q_castling | MoveFlags::castling};
            if (board.getFlags() & BoardFlags::K_castling) {
                startMove->flags |= MoveFlags::K_castling;
            }
            ++startMove;
        }
    }
    if (square == board.getKingStartSquare<Color::black>() && board.getFlags() & BoardFlags::k_castling) {
        flags |= MoveFlags::k_castling;
        if (blackKingSideCastlingPossible(board)) {
            *startMove = {board.getKingStartSquare<Color::black>(), n2N("g8"), board.getEnPassantSquare(), Piece::empty, MoveFlags::k_castling | MoveFlags::castling};
            if (board.getFlags() & BoardFlags::q_castling) {
                startMove->flags |= MoveFlags::q_castling;
            }
            ++startMove;
        }
    }
    if (square == board.getKingStartSquare<Color::black>() && board.getFlags() & BoardFlags::q_castling) {
        flags |= MoveFlags::q_castling;
        if (blackQueenSideCastlingPossible(board)) {
            *startMove = {board.getKingStartSquare<Color::black>(), n2N("c8"), board.getEnPassantSquare(), Piece::empty, MoveFlags::q_castling | MoveFlags::castling};
            if (board.getFlags() & BoardFlags::k_castling) {
                startMove->flags |= MoveFlags::k_castling;
            }
            ++startMove;
        }
    }
    return movesOfShortDistancePiece(board, square, kingBitmask[square], startMove, flags);
}

uint64_t MoveGenerator::maskOfShortDistancePiece(uint8_t square, const PairList &list) {
    uint64_t ret = 0;
    uint8_t _file = file(square);
    uint8_t _rank = rank(square);
    for (auto &pair : list) {
        if (inRange(_rank + pair.first, _file + pair.second)) {
            bit::set(ret, number(_file + pair.second, _rank + pair.first));
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
            bit::set(array[direction], number(_file + multiplier * pair.second, _rank + multiplier * pair.first));
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
    bool ret = isSquareAttacked(board, bit::mostSignificantBit(board.getBitmask(opponent(board.getMoveSide()), Piece::king)), board.getMoveSide());
    board.unmakeMove(m);
    return !ret;
}

bool MoveGenerator::isSquareAttacked(BoardType &board, uint8_t square, Color color) {
    if (knightBitmask[square] & board.getBitmask(color, Piece::knight)) {
        return true;
    }
    if (kingBitmask[square] & board.getBitmask(color, Piece::king)) {
        return true;
    }
    if (pawnBitmask[toInt(opponent(color))][square] & board.getBitmask(color, Piece::pawn)) {
        return true;
    }
    auto isAttackedBy = [&](uint64_t mask[64][4], Piece p) {
        for (int direction = 0; direction < 4; ++direction) {
            uint64_t piecesOnLine = mask[square][direction] & board.allPieces();
            uint8_t possiblePiece = 0;
            if (piecesOnLine) {
                possiblePiece = (bit::single(square) < mask[square][direction] ?
                        bit::leastSignificantBit : bit::mostSignificantBit)(piecesOnLine);
                if (board.getPieceColor(possiblePiece) == color && (board.getPiece(possiblePiece) == Piece::queen || board.getPiece(possiblePiece) == p)) {
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

bool MoveGenerator::whiteKingSideCastlingPossible(BoardType &board) {
    uint8_t kingSquare = board.getKingStartSquare<Color::white>();
    uint8_t rookSquare = board.getKingSideRookSquare<Color::white>();
    if (board.getPiece(rookSquare) != Piece::rook || board.getPieceColor(rookSquare) != Color::white) {
        return false;
    }
    uint8_t emptyStart = std::min(n2N("f1"), kingSquare);
    uint8_t emptyEnd = std::max(n2N("g1"), rookSquare);
    for (uint8_t i = emptyStart; i <= emptyEnd; ++i) {
        if (board.getPieceColor(i) != Color::empty && i != kingSquare && i != rookSquare) {
            return false;
        }
    }
    for (uint8_t i = kingSquare; i <= n2N("g1"); ++i) {
        if (isSquareAttacked(board, i, Color::black)) {
            return false;
        }
    }
    return true;
}

bool MoveGenerator::blackKingSideCastlingPossible(BoardType &board) {
    uint8_t kingSquare = board.getKingStartSquare<Color::black>();
    uint8_t rookSquare = board.getKingSideRookSquare<Color::black>();
    if (board.getPiece(rookSquare) != Piece::rook || board.getPieceColor(rookSquare) != Color::black) {
        return false;
    }
    uint8_t emptyStart = std::min(n2N("f8"), kingSquare);
    uint8_t emptyEnd = std::max(n2N("g8"), rookSquare);
    for (uint8_t i = emptyStart; i <= emptyEnd; ++i) {
        if (board.getPieceColor(i) != Color::empty && i != kingSquare && i != rookSquare) {
            return false;
        }
    }
    for (uint8_t i = kingSquare; i <= n2N("g8"); ++i) {
        if (isSquareAttacked(board, i, Color::white)) {
            return false;
        }
    }
    return true;
}

bool MoveGenerator::whiteQueenSideCastlingPossible(BoardType &board) {
    uint8_t kingSquare = board.getKingStartSquare<Color::white>();
    uint8_t rookSquare = board.getQueenSideRookSquare<Color::white>();
    if (board.getPiece(rookSquare) != Piece::rook || board.getPieceColor(rookSquare) != Color::white) {
        return false;
    }
    uint8_t emptyStart = std::min(n2N("c1"), rookSquare);
    uint8_t emptyEnd = std::max(n2N("d1"), kingSquare);
    for (uint8_t i = emptyStart; i <= emptyEnd; ++i) {
        if (board.getPieceColor(i) != Color::empty && i != kingSquare && i != rookSquare) {
            return false;
        }
    }
    int checkStartSquare = std::min(kingSquare, n2N("c1"));
    int checkEndSquare = std::max(kingSquare, n2N("c1"));
    for (uint8_t i = checkStartSquare; i <= checkEndSquare; ++i) {
        if (isSquareAttacked(board, i, Color::black)) {
            return false;
        }
    }
    return true;
}

bool MoveGenerator::blackQueenSideCastlingPossible(BoardType &board) {
    uint8_t kingSquare = board.getKingStartSquare<Color::black>();
    uint8_t rookSquare = board.getQueenSideRookSquare<Color::black>();
    if (board.getPiece(rookSquare) != Piece::rook || board.getPieceColor(rookSquare) != Color::black) {
        return false;
    }
    uint8_t emptyStart = std::min(n2N("c8"), rookSquare);
    uint8_t emptyEnd = std::max(n2N("d8"), kingSquare);
    for (uint8_t i = emptyStart; i <= emptyEnd; ++i) {
        if (board.getPieceColor(i) != Color::empty && i != kingSquare && i != rookSquare) {
            return false;
        }
    }
    int checkStartSquare = std::min(kingSquare, n2N("c8"));
    int checkEndSquare = std::max(kingSquare, n2N("c8"));
    for (uint8_t i = checkStartSquare; i <= checkEndSquare; ++i) {
        if (isSquareAttacked(board, i, Color::white)) {
            return false;
        }
    }
    return true;
}
