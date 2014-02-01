#include "ChessBoard.h"
#include "Move.h"
#include "utils/asserts.h"
#include "notation.h"
#include "utils/bit.h"
#include "utils/logging.h"
#include <iomanip>
#include <cstdint>
#include <iosfwd>

template <typename HashPolicy>
const int16_t ChessBoard<HashPolicy>::piecesValues[] = {100, 300, 300, 500, 900, 9000, 0}; // indexed by Piece

template <typename HashPolicy>
ChessBoard<HashPolicy>::ChessBoard() {
    initHash();
    history.init(HashPolicy::getHash());
}

template <typename HashPolicy>
uint64_t ChessBoard<HashPolicy>::piecesOf(Color color) const {
    return bitmask[toInt(color)][toInt(Piece::pawn)] | bitmask[toInt(color)][toInt(Piece::knight)] | bitmask[toInt(color)][toInt(Piece::bishop)] |
            bitmask[toInt(color)][toInt(Piece::rook)] | bitmask[toInt(color)][toInt(Piece::queen)] | bitmask[toInt(color)][toInt(Piece::king)];
}

template <typename HashPolicy>
uint64_t ChessBoard<HashPolicy>::allPieces() const {
    return piecesOf(Color::white) | piecesOf(Color::black);
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::disappearPiece(Piece piece, Color color, uint8_t from) {
    ASSERT(pieces[from] == piece && piecesColors[from] == color,
            number2Notation(from), pieces[from], piece, piecesColors[from], color);
    HashPolicy::updatePiece(from, toInt(piecesColors[from]), toInt(pieces[from]));
    pieces[from] = Piece::empty;
    piecesColors[from] = Color::empty;
    bit::unset(bitmask[toInt(color)][toInt(piece)], from);
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::appearPiece(Piece piece, Color color, uint8_t to) {
    // we can appear piece on square which is empty on opponents square (capture) or our square (promotion)
    pieces[to] = piece;
    piecesColors[to] = color;
    bit::set(bitmask[toInt(color)][toInt(piece)], to);
    HashPolicy::updatePiece(to, toInt(piecesColors[to]), toInt(pieces[to]));
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::takePiece(Piece piece, Color color, Piece capturedPiece, uint8_t from, uint8_t to) {
    ASSERT(pieces[from] == piece && piecesColors[from] == color && pieces[to] == capturedPiece && piecesColors[to] == opponent(color),
            (int)from, (int)to, piece, color, capturedPiece, pieces[from], pieces[to]);

    this->materialDifference += piecesValues[toInt(capturedPiece)] * (color == Color::white ? 1 : -1);
    bit::unset(bitmask[toInt(opponent(color))][toInt(capturedPiece)], to); // disappear opponent's piece
    disappearPiece(piece, color, from);
    appearPiece(piece, color, to);
    HashPolicy::updatePiece(to, toInt(opponent(color)), toInt(capturedPiece));
}

template<typename HashPolicy>
void ChessBoard<HashPolicy>::print() const {
    std::cerr << "\n";
    for (int8_t file = 0, rank = 7; rank >= 0;) {
        if (file == 0) {
            std::cerr << rank + 1 << ' ';
        }
        char toPrint = piece2Notation(pieces[rank * 8 + file], piecesColors[rank * 8 + file]);
        if (toPrint != ' ') {
            std::cerr << toPrint;
        } else {
            if ((rank + file) & 1) {
                std::cerr << toPrint;
            } else {
                std::cerr << '*';
            }
        }
        ++file;
        if (file > 7) {
            file = 0;
            --rank;
            std::cerr << std::endl;
        }
    }
    std::cerr << "\n  abcdefgh\n";
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::untakePiece(Piece piece, Color color, Piece capturedPiece, uint8_t from, uint8_t to) {
    ASSERT(pieces[to] == piece && piecesColors[to] == color && pieces[from] == Piece::empty && piecesColors[from] == Color::empty,
            (int)from, (int)to, piece, color, capturedPiece, pieces[from], pieces[to]);

    HashPolicy::updatePiece(to, toInt(color), toInt(piece));
    this->materialDifference -= piecesValues[toInt(capturedPiece)] * (color == Color::white ? 1 : -1);
    bit::unset(bitmask[toInt(color)][toInt(piece)], to); // disappear our piece
    appearPiece(capturedPiece, opponent(color), to);
    appearPiece(piece, color, from);
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::movePiece(Piece piece, Color color, uint8_t from, uint8_t to) {
    disappearPiece(piece, color, from);
    appearPiece(piece, color, to);
}

// not very nice but fast way to convert MoveFlags to BoardFlags
// with assumption that correspondent enums has same values
inline EnumFlags<BoardFlags> toBoardFlags(EnumFlags<MoveFlags> moveFlags) {
    return BoardFlags(toInt(moveFlags) & toInt(allCastlings));
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::makeMove(const Move &move) {
    ASSERT(piecesColors[move.from] == toMove, number2Notation(move.from), piecesColors[move.from], toMove);
    uint8_t newEnPassantSquare = pieces[move.from] == Piece::pawn && (move.to - move.from == 16 || move.from - move.to == 16) ? move.to : 0;
    if (move.captured != Piece::empty) {
        takePiece(pieces[move.from], toMove, move.captured, move.from, move.to);
    } else {
        movePiece(pieces[move.from], toMove, move.from, move.to);
    }
    if (pieces[move.to] == Piece::king) {
        if (move.flags & MoveFlags::castling) {
            ASSERT(flags & castling, flags);
            uint8_t startSquare = move.to > move.from ? 7 : 0;
            uint8_t offset = move.to > move.from ? -2 : 3;
            uint8_t colorOffset = 56 * int(toMove);
            movePiece(Piece::rook, toMove, startSquare + colorOffset, startSquare + colorOffset + offset);
        }
    } else if (move.flags & promotions) {
        disappearPiece(pieces[move.to], toMove, move.to);
        appearPiece(promotionPiece(move.flags & promotions), toMove, move.to);
        materialDifference += (piecesValues[toInt(promotionPiece(move.flags & promotions))] - piecesValues[toInt(Piece::pawn)]) * (toMove == Color::white ? 1 : -1);
    } else if (move.flags & MoveFlags::enPassantCapture) {
        ASSERT(enPassantSquare == (toMove == Color::white ? move.to - 8 : move.to + 8), move.to - 8, (int)enPassantSquare);
        disappearPiece(Piece::pawn, opponent(toMove), enPassantSquare);
        materialDifference += piecesValues[toInt(Piece::pawn)] * (toMove == Color::white ? 1 : -1);
    }
    setFlags(flags & ~toBoardFlags(move.flags));
    setEnPassantSquare(newEnPassantSquare);
    toMove = opponent(toMove);
    HashPolicy::switchPlayer();
    HashPolicy::resetRepetition();
    history.push(HashPolicy::getHash(), move.captured != Piece::empty || pieces[move.to] == Piece::pawn);
    HashPolicy::setRepetition(history.isTopPositionRepeated(2));
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::unmakeMove(const Move &move) {
    ASSERT(piecesColors[move.to] == opponent(toMove), toMove, piecesColors[move.to]);
    history.pop();
    if (move.captured != Piece::empty) {
        untakePiece(pieces[move.to], opponent(toMove), move.captured, move.from, move.to);
    } else {
        movePiece(pieces[move.to], opponent(toMove), move.to, move.from);
    }
    setEnPassantSquare(move.enPassantSquare);
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
        materialDifference += (piecesValues[toInt(promotionPiece(move.flags & promotions))] - piecesValues[toInt(Piece::pawn)]) * (toMove == Color::white ? 1 : -1);
    } else if (move.flags & MoveFlags::enPassantCapture) { // pion zbity w przelocie pojawia sie
        appearPiece(Piece::pawn, toMove, move.enPassantSquare);
        materialDifference += piecesValues[toInt(Piece::pawn)] * (toMove == Color::white ? 1 : -1);
    }
    setFlags(flags | toBoardFlags(move.flags));
    toMove = opponent(toMove);
    HashPolicy::switchPlayer();
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::checkIntegrity() const {
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
    ASSERT(!(flags & BoardFlags::K_castling) || (pieces[ 4] == Piece::king && pieces[ 7] == Piece::rook &&
            piecesColors[ 4] == Color::white && piecesColors[ 7] == Color::white), pieces[ 4], pieces[ 7], piecesColors[ 4], piecesColors[ 7], flags);
    ASSERT(!(flags & BoardFlags::Q_castling) || (pieces[ 4] == Piece::king && pieces[ 0] == Piece::rook &&
            piecesColors[ 4] == Color::white && piecesColors[ 0] == Color::white), pieces[ 4], pieces[ 0], piecesColors[ 4], piecesColors[ 0], flags);
    ASSERT(!(flags & BoardFlags::k_castling) || (pieces[60] == Piece::king && pieces[63] == Piece::rook &&
            piecesColors[60] == Color::black && piecesColors[63] == Color::black), pieces[60], pieces[63], piecesColors[60], piecesColors[63], flags);
    ASSERT(!(flags & BoardFlags::q_castling) || (pieces[60] == Piece::king && pieces[56] == Piece::rook &&
            piecesColors[60] == Color::black && piecesColors[56] == Color::black), pieces[60], pieces[56], piecesColors[60], piecesColors[56], flags);
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::dumpRank(std::ostream &stream, uint8_t rank) const {
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

template <typename HashPolicy>
void ChessBoard<HashPolicy>::dumpMask(std::ostream &stream, Piece piece) const {
    stream << "\t" << piece << "\t" << std::setw(18) << bitmask[toInt(Color::white)][toInt(piece)] <<
        "\t\t" << std::setw(18) << bitmask[toInt(Color::black)][toInt(piece)];
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::dump(std::ostream &stream) const {
    stream << std::hex << std::showbase << std::endl << "__________\tpiece\twhite\t\t\t\tblack" << std::endl;
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
        case 0:
            stream << "\thash\t" << std::noshowbase << (uint64_t)this->getHash() << std::showbase;
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
    stream << std::dec << std::endl << std::endl;;
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::clear() {
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

    this->setEnPassantSquare(0);
    this->flags = BoardFlags::empty;
    HashPolicy::clearHash();
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::initHash() {
    HashPolicy::clearHash();
    for (int i = 0; i < 64; ++i) {
        if (pieces[i] != Piece::empty) {
            HashPolicy::updatePiece(i, toInt(piecesColors[i]), toInt(pieces[i]));
        }
    }
    HashPolicy::updateCastlingCapabilities(toInt(flags));
    HashPolicy::initRepetition();
}

template <typename HashPolicy>
bool ChessBoard<HashPolicy>::isDraw() {
    return this->history.isDraw();
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::setEnPassantSquare(uint8_t enPassantSquare) {
    if (this->enPassantSquare != enPassantSquare) {
        if (this->enPassantSquare != 0) {
            HashPolicy::updateEnPassantFile(file(this->enPassantSquare));
        }
        if (enPassantSquare != 0) {
            HashPolicy::updateEnPassantFile(file(enPassantSquare));
        }
        this->enPassantSquare = enPassantSquare;
    }
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::setFlags(EnumFlags<BoardFlags> flags) {
    if (this->flags != flags) {
        HashPolicy::updateCastlingCapabilities(toInt(this->flags));
        HashPolicy::updateCastlingCapabilities(toInt(flags));
        this->flags = flags;
    }
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::initHistory() {
    history.init(HashPolicy::getHash());
}
