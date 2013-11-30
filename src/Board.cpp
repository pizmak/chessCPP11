#include "Board.h"
#include "Move.h"
#include "asserts.h"
#include "notation.h"
#include "bit.h"
#include <iomanip>

uint64_t Board::piecesOf(Color color) {
    return bitmask[toInt(color)][toInt(Piece::pawn)] | bitmask[toInt(color)][toInt(Piece::knight)] | bitmask[toInt(color)][toInt(Piece::bishop)] |
            bitmask[toInt(color)][toInt(Piece::rook)] | bitmask[toInt(color)][toInt(Piece::queen)] | bitmask[toInt(color)][toInt(Piece::king)];
}

uint64_t Board::allPieces() {
    return piecesOf(Color::white) | piecesOf(Color::black);
}

void Board::disappearPiece(Board &board, Piece piece, Color color, uint8_t from) {
    ASSERT(board.pieces[from] == piece && board.piecesColors[from] == color,
            number2Notation(from), board.pieces[from], piece, board.piecesColors[from], color);
    board.pieces[from] = Piece::empty;
    board.piecesColors[from] = Color::empty;
    bit::unset(board.bitmask[toInt(color)][toInt(piece)], from);
}

void Board::appearPiece(Board &board, Piece piece, Color color, uint8_t to) {
    // we can appear piece on square which is empty on opponents square (capture) or our square (promotion)
    board.pieces[to] = piece;
    board.piecesColors[to] = color;
    bit::set(board.bitmask[toInt(color)][toInt(piece)], to);
}

void Board::takePiece(Board &board, Piece piece, Color color, Piece capturedPiece, uint8_t from, uint8_t to) {
    ASSERT(board.pieces[from] == piece && board.piecesColors[from] == color && board.pieces[to] == capturedPiece && board.piecesColors[to] == opponent(color),
            (int)from, (int)to, piece, color, capturedPiece, board.pieces[from], board.pieces[to]);

    bit::unset(board.bitmask[toInt(opponent(color))][toInt(capturedPiece)], to); // disappear opponent's piece
    disappearPiece(board, piece, color, from);
    appearPiece(board, piece, color, to);
}

void Board::untakePiece(Board &board, Piece piece, Color color, Piece capturedPiece, uint8_t from, uint8_t to) {
    ASSERT(board.pieces[to] == piece && board.piecesColors[to] == color && board.pieces[from] == Piece::empty && board.piecesColors[from] == Color::empty,
            (int)from, (int)to, piece, color, capturedPiece, board.pieces[from], board.pieces[to]);

    bit::unset(board.bitmask[toInt(color)][toInt(piece)], to); // disappear our piece
    appearPiece(board, capturedPiece, opponent(color), to);
    appearPiece(board, piece, color, from);
}

void Board::movePiece(Board &board, Piece piece, Color color, uint8_t from, uint8_t to) {
    disappearPiece(board, piece, color, from);
    appearPiece(board, piece, color, to);
}

// not very nice but fast way to convert MoveFlags to BoardFlags
// with assumption that correspondent enums has same values
EnumFlags<BoardFlags> toBoardFlags(EnumFlags<MoveFlags> moveFlags) {
    return BoardFlags(toInt(moveFlags) & toInt(castling));
}

void Board::makeMove(const Move &move) {
    ASSERT(piecesColors[move.from] == toMove, number2Notation(move.from), piecesColors[move.from], toMove);
    // TODO remove this variable as it is only for diagnostic purposes at the moment
    uint8_t newEnPassantSquare = pieces[move.from] == Piece::pawn && (move.to - move.from == 16 || move.from - move.to == 16) ? move.to : 0;
    if (move.captured != Piece::empty) {
        takePiece(*this, pieces[move.from], toMove, move.captured, move.from, move.to);
    } else {
        movePiece(*this, pieces[move.from], toMove, move.from, move.to);
    }
    if (pieces[move.to] == Piece::king) {
        std::cerr << "ruch krola" << std::endl;
        if (move.flags & MoveFlags::castling) {
            std::cerr << "roszada" << std::endl;
            ASSERT(flags & castling, flags);
            uint8_t startSquare = move.to > move.from ? 7 : 0;
            uint8_t offset = move.to > move.from ? -2 : 3;
            uint8_t colorOffset = 56 * int(toMove);
            movePiece(*this, Piece::rook, toMove, startSquare + colorOffset, startSquare + colorOffset + offset);
        }
    } else if (move.flags & promotions) {
        std::cerr << "promocja: " << move.flags << std::endl;
        disappearPiece(*this, pieces[move.to], toMove, move.to);
        appearPiece(*this, promotionPiece(move.flags & promotions), toMove, move.to);
    } else if (move.flags & MoveFlags::enPassantCapture) {
        std::cerr << "bicie w przelocie" << std::endl;
        ASSERT(enPassantSquare == (toMove == Color::white ? move.to - 8 : move.to + 8), move.to - 8, (int)enPassantSquare);
        disappearPiece(*this, Piece::pawn, opponent(toMove), enPassantSquare);
    }
    flags &= ~toBoardFlags(move.flags);
    enPassantSquare = newEnPassantSquare;
    toMove = opponent(toMove);
}

void Board::unmakeMove(const Move &move) {
    ASSERT(piecesColors[move.to] == opponent(toMove), toMove, piecesColors[move.to]);
    if (move.captured != Piece::empty) {
        untakePiece(*this, pieces[move.to], opponent(toMove), move.captured, move.from, move.to);
    } else {
        movePiece(*this, pieces[move.to], opponent(toMove), move.to, move.from);
    }
    enPassantSquare = move.enPassantSquare;
    if (pieces[move.from] == Piece::king) {
        if (move.flags & MoveFlags::castling) {
            uint8_t startSquare = move.to > move.from ? 7 : 0;
            uint8_t offset = move.to > move.from ? -2 : 3;
            uint8_t colorOffset = 56 * int(opponent(toMove));
            movePiece(*this, Piece::rook, opponent(toMove), startSquare + colorOffset + offset, startSquare + colorOffset);
        }
    } else if (move.flags & promotions) {
        disappearPiece(*this, pieces[move.from], opponent(toMove), move.from);
        appearPiece(*this, Piece::pawn, opponent(toMove), move.from);
    } else if (move.flags & MoveFlags::enPassantCapture) { // pion zbity w przelocie pojawia sie
        appearPiece(*this, Piece::pawn, toMove, move.enPassantSquare);
    }
    flags |= toBoardFlags(move.flags);
    toMove = opponent(toMove);
}

void Board::checkIntegrity() {
#ifndef DEBUG
    return;
#endif
    for (int i = 0; i < 64; ++i) {
        if (pieces[i] == Piece::empty) {
            ASSERT(piecesColors[i] == Color::empty, i, piecesColors[i]);
            for (int piece = 0; piece < 6; ++piece) {
                ASSERT(!bit::isSet(bitmask[0][piece], i), i, piece, bitmask[0][piece]);
                ASSERT(!bit::isSet(bitmask[1][piece], i), i, piece, bitmask[1][piece]);
            }
        } else {
            ASSERT(piecesColors[i] != Color::empty, i, piecesColors[i]);
            for (int ii = 0; ii < 6; ++ii) {
                ASSERT(bit::isSet(bitmask[0][ii], i) == (pieces[i] == Piece(ii) && piecesColors[i] == Color::white),
                        i, ii, bitmask[0][ii], bitmask[toInt(piecesColors[i])][toInt(pieces[i])], piecesColors[i], pieces[i]);
                ASSERT(bit::isSet(bitmask[1][ii], i) == (pieces[i] == Piece(ii) && piecesColors[i] == Color::black),
                        i, ii, bitmask[1][ii], bitmask[toInt(piecesColors[i])][toInt(pieces[i])], piecesColors[i], pieces[i]);
            }
        }
    }
    ASSERT(enPassantSquare == 0 || (toMove == Color::black && enPassantSquare >= 24 && enPassantSquare < 32) ||
                                   (toMove == Color::white && enPassantSquare >= 32 && enPassantSquare < 40), toMove, (int)enPassantSquare);
}

void Board::dumpRank(std::ostream &stream, uint8_t rank) {
    ASSERT(/*rank >= 0 && */rank < 8, rank);
    for (uint8_t file = 0; file < 8; ++file) {
        switch (piecesColors[number(rank, file)]) {
        case Color::empty:
            stream << (isWhite(rank, file) ? " " : ".");
            break;
        default:
            stream << piece2Notation(pieces[number(rank, file)], piecesColors[number(rank, file)]);
        }
    }
}

void Board::dumpMask(std::ostream &stream, Piece piece) {
    stream << "\t" << piece << "\t" << std::setw(18) << bitmask[toInt(Color::white)][toInt(piece)] <<
        "\t\t" << std::setw(18) << bitmask[toInt(Color::black)][toInt(piece)];
}

void Board::dump(std::ostream &stream) {
    stream << std::showbase << std::endl << "__________\tpiece\twhite\t\t\t\tblack" << std::endl;
    for (uint8_t rank = 7; /*rank >= 0 && */rank < 8; --rank) {
        stream << "|";
        dumpRank(stream, rank);
        stream << "|";
        switch (rank) {
        case 1:
            dumpMask(stream, Piece::pawn);
            break;
        case 2:
            dumpMask(stream, Piece::knight);
            break;
        case 3:
            dumpMask(stream, Piece::bishop);
            break;
        case 4:
            dumpMask(stream, Piece::rook);
            break;
        case 5:
            dumpMask(stream, Piece::queen);
            break;
        case 6:
            dumpMask(stream, Piece::king);
            break;
        }
        stream << std::endl;
    }
    stream << std::noshowbase << "----------\ttoMove: " << toMove;
    if (flags != BoardFlags::empty) {
        stream << ", flags: " << flags;
    }
    if (enPassantSquare != 0) {
        stream << ", e.p. on: " << (int)enPassantSquare;
    }
    stream << std::endl << std::endl;;
}

void Board::clear() {
    for(auto &p: this->pieces) {
        p = Piece::empty;
    }
    for(auto &c: this->piecesColors) {
        c = Color::empty;
    }

    for (auto &c: this->bitmask) {
        for (auto &p : c) {
            p = 0;
        }
    }

    this->enPassantSquare = 0;
    this->flags = BoardFlags::empty;
}
