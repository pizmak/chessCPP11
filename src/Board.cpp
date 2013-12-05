#include "Board.h"
#include "Move.h"
#include "asserts.h"
#include "notation.h"
#include "bit.h"
#include "logging.h"
#include <iomanip>

uint64_t Board::piecesOf(Color color) const {
    return bitmask[toInt(color)][toInt(Piece::pawn)] | bitmask[toInt(color)][toInt(Piece::knight)] | bitmask[toInt(color)][toInt(Piece::bishop)] |
            bitmask[toInt(color)][toInt(Piece::rook)] | bitmask[toInt(color)][toInt(Piece::queen)] | bitmask[toInt(color)][toInt(Piece::king)];
}

uint64_t Board::allPieces() const {
    return piecesOf(Color::white) | piecesOf(Color::black);
}

void Board::disappearPiece(Piece piece, Color color, uint8_t from) {
    ASSERT(pieces[from] == piece && piecesColors[from] == color,
            number2Notation(from), pieces[from], piece, piecesColors[from], color);
    pieces[from] = Piece::empty;
    piecesColors[from] = Color::empty;
    bit::unset(bitmask[toInt(color)][toInt(piece)], from);
}

void Board::appearPiece(Piece piece, Color color, uint8_t to) {
    // we can appear piece on square which is empty on opponents square (capture) or our square (promotion)
    pieces[to] = piece;
    piecesColors[to] = color;
    bit::set(bitmask[toInt(color)][toInt(piece)], to);
}

void Board::takePiece(Piece piece, Color color, Piece capturedPiece, uint8_t from, uint8_t to) {
    ASSERT(pieces[from] == piece && piecesColors[from] == color && pieces[to] == capturedPiece && piecesColors[to] == opponent(color),
            (int)from, (int)to, piece, color, capturedPiece, pieces[from], pieces[to]);

    bit::unset(bitmask[toInt(opponent(color))][toInt(capturedPiece)], to); // disappear opponent's piece
    disappearPiece(piece, color, from);
    appearPiece(piece, color, to);
}

void Board::untakePiece(Piece piece, Color color, Piece capturedPiece, uint8_t from, uint8_t to) {
    ASSERT(pieces[to] == piece && piecesColors[to] == color && pieces[from] == Piece::empty && piecesColors[from] == Color::empty,
            (int)from, (int)to, piece, color, capturedPiece, pieces[from], pieces[to]);

    bit::unset(bitmask[toInt(color)][toInt(piece)], to); // disappear our piece
    appearPiece(capturedPiece, opponent(color), to);
    appearPiece(piece, color, from);
}

void Board::movePiece(Piece piece, Color color, uint8_t from, uint8_t to) {
    disappearPiece(piece, color, from);
    appearPiece(piece, color, to);
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
        takePiece(pieces[move.from], toMove, move.captured, move.from, move.to);
    } else {
        movePiece(pieces[move.from], toMove, move.from, move.to);
    }
    if (pieces[move.to] == Piece::king) {
//        TRACELN("ruch krola");
        if (move.flags & MoveFlags::castling) {
            TRACELN("roszada");
            ASSERT(flags & castling, flags);
            uint8_t startSquare = move.to > move.from ? 7 : 0;
            uint8_t offset = move.to > move.from ? -2 : 3;
            uint8_t colorOffset = 56 * int(toMove);
            movePiece(Piece::rook, toMove, startSquare + colorOffset, startSquare + colorOffset + offset);
        }
    } else if (move.flags & promotions) {
        TRACELN("promocja: " << move.flags);
        disappearPiece(pieces[move.to], toMove, move.to);
        appearPiece(promotionPiece(move.flags & promotions), toMove, move.to);
    } else if (move.flags & MoveFlags::enPassantCapture) {
        TRACELN("bicie w przelocie");
        ASSERT(enPassantSquare == (toMove == Color::white ? move.to - 8 : move.to + 8), move.to - 8, (int)enPassantSquare);
        disappearPiece(Piece::pawn, opponent(toMove), enPassantSquare);
    }
    flags &= ~toBoardFlags(move.flags);
    enPassantSquare = newEnPassantSquare;
    toMove = opponent(toMove);
}

void Board::unmakeMove(const Move &move) {
    ASSERT(piecesColors[move.to] == opponent(toMove), toMove, piecesColors[move.to]);
    if (move.captured != Piece::empty) {
        untakePiece(pieces[move.to], opponent(toMove), move.captured, move.from, move.to);
    } else {
        movePiece(pieces[move.to], opponent(toMove), move.to, move.from);
    }
    enPassantSquare = move.enPassantSquare;
    if (pieces[move.from] == Piece::king) {
        if (move.flags & MoveFlags::castling) {
            uint8_t startSquare = move.to > move.from ? 7 : 0;
            uint8_t offset = move.to > move.from ? -2 : 3;
            uint8_t colorOffset = 56 * int(opponent(toMove));
            movePiece(Piece::rook, opponent(toMove), startSquare + colorOffset + offset, startSquare + colorOffset);
        }
    } else if (move.flags & promotions) {
        disappearPiece(pieces[move.from], opponent(toMove), move.from);
        appearPiece(Piece::pawn, opponent(toMove), move.from);
    } else if (move.flags & MoveFlags::enPassantCapture) { // pion zbity w przelocie pojawia sie
        appearPiece(Piece::pawn, toMove, move.enPassantSquare);
    }
    flags |= toBoardFlags(move.flags);
    toMove = opponent(toMove);
}

void Board::checkIntegrity() const {
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

void Board::dumpRank(std::ostream &stream, uint8_t rank) const {
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

void Board::dumpMask(std::ostream &stream, Piece piece) const {
    stream << "\t" << piece << "\t" << std::setw(18) << bitmask[toInt(Color::white)][toInt(piece)] <<
        "\t\t" << std::setw(18) << bitmask[toInt(Color::black)][toInt(piece)];
}

void Board::dump(std::ostream &stream) const {
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
