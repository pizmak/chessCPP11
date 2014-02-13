#include "ChessBoard.h"
#include "Move.h"
#include "utils/asserts.h"
#include "notation.h"
#include "utils/bit.h"
#include "utils/logging.h"
#include "utils/split.h"
#include <iomanip>
#include <cstdint>
#include <iosfwd>
#include <algorithm>

template <typename HashPolicy>
std::ostream &operator<<(std::ostream &stream, ChessBoard<HashPolicy> &board) {
    board.dump(stream);
    return stream;
}

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
            number2Notation(from), pieces[from], piece, piecesColors[from], color, *this);
    HashPolicy::updatePiece(from, toInt(piecesColors[from]), toInt(pieces[from]));
    pieces[from] = Piece::empty;
    piecesColors[from] = Color::empty;
    bit::unset(bitmask[toInt(color)][toInt(piece)], from);
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::appearPiece(Piece piece, Color color, uint8_t to) {
    // we can appear piece on square which is empty on opponents square (capture) or our square (promotion)
    ASSERT(piecesColors[to] != color || color == Color::white && pieces[to] == Piece::pawn && rank(to) == rank2N('8')
            || color == Color::black && pieces[to] == Piece::pawn && rank(to) == rank2N('1'), piece, color, int(to), *this);
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
    checkIntegrity(move);
    ASSERT(piecesColors[move.from] == toMove, number2Notation(move.from), piecesColors[move.from], toMove, move, *this);
    uint8_t newEnPassantSquare = pieces[move.from] == Piece::pawn && (move.to - move.from == 16 || move.from - move.to == 16) ? move.to : 0;
    if (pieces[move.from] == Piece::king) {
        if (move.flags & MoveFlags::castling) {
            ASSERT(flags & allCastlings, flags);
            ASSERT(file(move.to) == file2N('g') || file(move.to) == file2N('c') ,  move, *this);
            ASSERT(rank(move.to) == rank2N('1') && toMove == Color::white || rank(move.to) == rank2N('8') && toMove == Color::black ,  move, *this);
            uint8_t rookStartFile = file(move.to) == file2N('g') ? this->kingSideRookFile :  this->queenSideRookFile;
            uint8_t offset = file(move.to) == file2N('g') ? file2N('f') - this->kingSideRookFile : file2N('d') - this->queenSideRookFile;
            uint8_t colorOffset = 56 * int(toMove);
            disappearPiece(Piece::rook, toMove, rookStartFile + colorOffset);
            movePiece(pieces[move.from], toMove, move.from, move.to);
            appearPiece(Piece::rook, toMove, rookStartFile + colorOffset + offset);
        } else {
            if (move.captured != Piece::empty) {
                takePiece(pieces[move.from], toMove, move.captured, move.from, move.to);
            } else {
                movePiece(pieces[move.from], toMove, move.from, move.to);
            }
        }
    } else {
        if (move.captured != Piece::empty) {
            takePiece(pieces[move.from], toMove, move.captured, move.from, move.to);
        } else {
            movePiece(pieces[move.from], toMove, move.from, move.to);
        }
        if (move.flags & promotions) {
            disappearPiece(pieces[move.to], toMove, move.to);
            appearPiece(promotionPiece(move.flags & promotions), toMove, move.to);
            materialDifference += (piecesValues[toInt(promotionPiece(move.flags & promotions))] - piecesValues[toInt(Piece::pawn)]) * (toMove == Color::white ? 1 : -1);
        } else if (move.flags & MoveFlags::enPassantCapture) {
            ASSERT(enPassantSquare == (toMove == Color::white ? move.to - 8 : move.to + 8), move.to - 8, (int)enPassantSquare);
            disappearPiece(Piece::pawn, opponent(toMove), enPassantSquare);
            materialDifference += piecesValues[toInt(Piece::pawn)] * (toMove == Color::white ? 1 : -1);
        }
    }
    setFlags(flags & ~toBoardFlags(move.flags));
    setEnPassantSquare(newEnPassantSquare);
    toMove = opponent(toMove);
    HashPolicy::switchPlayer();
    HashPolicy::resetRepetition();
    history.push(HashPolicy::getHash(), move.captured != Piece::empty || pieces[move.to] == Piece::pawn);
    HashPolicy::setRepetition(history.isTopPositionRepeated(2));
    checkIntegrity(move);
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::unmakeMove(const Move &move) {
    checkIntegrity(move);
    ASSERT(piecesColors[move.to] == opponent(toMove), toMove, piecesColors[move.to]);
    history.pop();
    setEnPassantSquare(move.enPassantSquare);
    if (pieces[move.to] == Piece::king) {
        if (move.flags & MoveFlags::castling) {
            ASSERT(file(move.to) == file2N('g') || file(move.to) == file2N('c') ,  move, *this);
            ASSERT(rank(move.to) == rank2N('1') && toMove == Color::black || rank(move.to) == rank2N('8') && toMove == Color::white ,  move, *this);
            uint8_t rookStartFile = file(move.to) == file2N('g') ? this->kingSideRookFile : this->queenSideRookFile;
            uint8_t offset = file(move.to) == file2N('g') ? file2N('f') - this->kingSideRookFile : file2N('d') - this->queenSideRookFile;
            uint8_t colorOffset = 56 * int(opponent(toMove));
            disappearPiece(Piece::rook, opponent(toMove), rookStartFile + colorOffset + offset);
            movePiece(pieces[move.to], opponent(toMove), move.to, move.from);
            appearPiece(Piece::rook, opponent(toMove), rookStartFile + colorOffset);
        } else {
            if (move.captured != Piece::empty) {
                untakePiece(pieces[move.to], opponent(toMove), move.captured, move.from, move.to);
            } else {
                movePiece(pieces[move.to], opponent(toMove), move.to, move.from);
            }
        }
    } else {
        if (move.captured != Piece::empty) {
            untakePiece(pieces[move.to], opponent(toMove), move.captured, move.from, move.to);
        } else {
            movePiece(pieces[move.to], opponent(toMove), move.to, move.from);
        }
        if (move.flags & promotions) {
            disappearPiece(pieces[move.from], opponent(toMove), move.from);
            appearPiece(Piece::pawn, opponent(toMove), move.from);
            materialDifference += (piecesValues[toInt(promotionPiece(move.flags & promotions))] - piecesValues[toInt(Piece::pawn)]) * (toMove == Color::white ? 1 : -1);
        } else if (move.flags & MoveFlags::enPassantCapture) { // pion zbity w przelocie pojawia sie
            appearPiece(Piece::pawn, toMove, move.enPassantSquare);
            materialDifference += piecesValues[toInt(Piece::pawn)] * (toMove == Color::white ? 1 : -1);
        }
    }
    setFlags(flags | toBoardFlags(move.flags));
    toMove = opponent(toMove);
    HashPolicy::switchPlayer();
    checkIntegrity(move);
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::checkIntegrity(const Move &move) const {
#ifndef DEBUG
    (void)move;
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
                                   (toMove == Color::white && enPassantSquare >= 32 && enPassantSquare < 40), toMove, (int)enPassantSquare, *this);
    ASSERT(!(flags & BoardFlags::K_castling) || (pieces[getKingStartSquare<Color::white>()] == Piece::king && pieces[getKingSideRookSquare<Color::white>()] == Piece::rook &&
            piecesColors[getKingStartSquare<Color::white>()] == Color::white && piecesColors[getKingSideRookSquare<Color::white>()] == Color::white),
            pieces[getKingStartSquare<Color::white>()], pieces[getKingSideRookSquare<Color::white>()], piecesColors[getKingStartSquare<Color::white>()], piecesColors[getKingSideRookSquare<Color::white>()], flags, move, *this);
    ASSERT(!(flags & BoardFlags::Q_castling) || (pieces[getKingStartSquare<Color::white>()] == Piece::king && pieces[getQueenSideRookSquare<Color::white>()] == Piece::rook &&
            piecesColors[getKingStartSquare<Color::white>()] == Color::white && piecesColors[getQueenSideRookSquare<Color::white>()] == Color::white),
            pieces[getKingStartSquare<Color::white>()], pieces[getQueenSideRookSquare<Color::white>()], piecesColors[getKingStartSquare<Color::white>()], piecesColors[getQueenSideRookSquare<Color::white>()], flags, move, *this);
    ASSERT(!(flags & BoardFlags::k_castling) || (pieces[getKingStartSquare<Color::black>()] == Piece::king && pieces[getKingSideRookSquare<Color::black>()] == Piece::rook &&
            piecesColors[getKingStartSquare<Color::black>()] == Color::black && piecesColors[getKingSideRookSquare<Color::black>()] == Color::black),
            pieces[getKingStartSquare<Color::black>()], pieces[getKingSideRookSquare<Color::black>()], piecesColors[getKingStartSquare<Color::black>()], piecesColors[getKingSideRookSquare<Color::black>()], flags, move, *this);
    ASSERT(!(flags & BoardFlags::q_castling) || (pieces[getKingStartSquare<Color::black>()] == Piece::king && pieces[getQueenSideRookSquare<Color::black>()] == Piece::rook &&
            piecesColors[getKingStartSquare<Color::black>()] == Color::black && piecesColors[getQueenSideRookSquare<Color::black>()] == Color::black),
            pieces[getKingStartSquare<Color::black>()], pieces[getQueenSideRookSquare<Color::black>()], piecesColors[getKingStartSquare<Color::black>()], piecesColors[getQueenSideRookSquare<Color::black>()], flags, move, *this);
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
bool ChessBoard<HashPolicy>::isDraw() const {
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

template <typename HashPolicy>
Piece ChessBoard<HashPolicy>::getPiece(uint8_t index) const {
    return pieces[index];
}

template <typename HashPolicy>
Color ChessBoard<HashPolicy>::getPieceColor(uint8_t index) const {
    return piecesColors[index];
}

template <typename HashPolicy>
uint64_t ChessBoard<HashPolicy>::getBitmask(Color color, Piece piece) const {
    return bitmask[toInt(color)][toInt(piece)];
}

template <typename HashPolicy>
Color ChessBoard<HashPolicy>::getMoveSide() const {
    return toMove;
}

template <typename HashPolicy>
uint8_t ChessBoard<HashPolicy>::getEnPassantSquare() const {
    return enPassantSquare;
}

template <typename HashPolicy>
EnumFlags<BoardFlags> ChessBoard<HashPolicy>::getFlags() const {
    return flags;
}

template <typename HashPolicy>
uint8_t ChessBoard<HashPolicy>::getKingStartFile() const {
    return kingStartFile;
}

template <typename HashPolicy>
uint8_t ChessBoard<HashPolicy>::getQueenSideRookFile() const {
    return queenSideRookFile;
}

template <typename HashPolicy>
uint8_t ChessBoard<HashPolicy>::getKingSideRookFile() const {
    return kingSideRookFile;
}

#define INVALID_FILE 0xFF

template <typename HashPolicy>
void ChessBoard<HashPolicy>::initFromFen(std::list<std::string> fenPosition) {
    clear();
    kingSideRookFile = INVALID_FILE;
    queenSideRookFile = INVALID_FILE;
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
                appearPiece(piece, rank[0] >= 'a' ? Color::black : Color::white, number(_rank, file));
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
    this->toMove = toMove == "w" ? Color::white : Color::black;

    std::string castlings = fenPosition.front();
    fenPosition.pop_front();
    int whiteKingStartFile = INVALID_FILE;
    int blackKingStartFile = INVALID_FILE;
    if (std::find_if(castlings.begin(), castlings.end(), [](char c) { return c >= 'A' && c <= 'Z'; }) != castlings.end()) {
        kingStartFile = whiteKingStartFile = file(bit::mostSignificantBit(getBitmask(Color::white, Piece::king)));
    }
    if (std::find_if(castlings.begin(), castlings.end(), [](char c) { return c >= 'a' && c <= 'z'; }) != castlings.end()) {
        kingStartFile = blackKingStartFile = file(bit::mostSignificantBit(getBitmask(Color::black, Piece::king)));
        ASSERT(whiteKingStartFile == INVALID_FILE || whiteKingStartFile == blackKingStartFile, whiteKingStartFile, blackKingStartFile);
    }

    initCastlingsFromFen(castlings);

    std::string enPassant = fenPosition.front();
    fenPosition.pop_front();
    if (enPassant != "-") {
        setEnPassantSquare(notation2Number(enPassant));
    }
    initHistory();
}

template <typename HashPolicy>
void ChessBoard<HashPolicy>::initCastlingsFromFen(std::string castlings) {
    if (castlings == "-") {
        return;
    }
    EnumFlags<BoardFlags> flags{};
    uint64_t firstRankWhiteRooksFiles = 0xFF & getBitmask(Color::white, Piece::rook);
    uint64_t lastRankBlackRooksFiles = (0xFF00000000000000 & getBitmask(Color::black, Piece::rook)) >> 56;
    for (char c : castlings) {
        if (c == 'K') {
            flags |= BoardFlags::K_castling;
            kingSideRookFile = bit::mostSignificantBit(firstRankWhiteRooksFiles);
        } else if (c == 'Q') {
            flags |= BoardFlags::Q_castling;
            queenSideRookFile = bit::leastSignificantBit(firstRankWhiteRooksFiles);
        } else if (c == 'k') {
            flags |= BoardFlags::k_castling;
            ASSERT(kingSideRookFile == INVALID_FILE || kingSideRookFile == bit::mostSignificantBit(lastRankBlackRooksFiles), int(kingSideRookFile), int(bit::mostSignificantBit(lastRankBlackRooksFiles)));
            kingSideRookFile = bit::mostSignificantBit(lastRankBlackRooksFiles);
        } else if (c == 'q') {
            flags |= BoardFlags::q_castling;
            ASSERT(queenSideRookFile == INVALID_FILE || queenSideRookFile == bit::leastSignificantBit(lastRankBlackRooksFiles), int(queenSideRookFile), int(bit::leastSignificantBit(lastRankBlackRooksFiles)));
            queenSideRookFile = bit::leastSignificantBit(lastRankBlackRooksFiles);
        } else if (c >= 'A' && c <= 'H') {
            ASSERT(bit::isSet(getBitmask(Color::white, Piece::rook), file2N(c)), getBitmask(Color::white, Piece::rook), c);
            if (c < kingStartFile) {
                flags |=  BoardFlags::Q_castling;
                ASSERT(queenSideRookFile == INVALID_FILE || queenSideRookFile == file2N(c), int(queenSideRookFile), c);
                queenSideRookFile = file2N(c);
            } else {
                flags |=  BoardFlags::K_castling;
                ASSERT(kingSideRookFile == INVALID_FILE || kingSideRookFile == file2N(c), int(kingSideRookFile), c);
                kingSideRookFile = file2N(c);
            }
        } else if (c >= 'a' && c <= 'h') {
            ASSERT(bit::isSet(getBitmask(Color::black, Piece::rook), file2N(c)), getBitmask(Color::black, Piece::rook), c);
            if (c < kingStartFile) {
                flags |=  BoardFlags::Q_castling;
                ASSERT(queenSideRookFile == INVALID_FILE || queenSideRookFile == file2N(c), int(queenSideRookFile), c);
                queenSideRookFile = file2N(c);
            } else {
                flags |=  BoardFlags::K_castling;
                ASSERT(kingSideRookFile == INVALID_FILE || kingSideRookFile == file2N(c), int(queenSideRookFile), c);
                kingSideRookFile = file2N(c);
            }
        } else {
            ASSERT(false, c);
        }
    }
    TRACE_PARAMS(int(kingSideRookFile), int(queenSideRookFile), flags);
    setFlags(flags);
}

template <typename HashPolicy>
template<Color color>
uint8_t ChessBoard<HashPolicy>::getKingStartSquare() const {
    static_assert(color == Color::white || color == Color::black, "invalid color");
    return number(rank2N(color == Color::white ? '1' : '8'), kingStartFile);
}

template <typename HashPolicy>
template<Color color>
uint8_t ChessBoard<HashPolicy>::getQueenSideRookSquare() const {
    static_assert(color == Color::white || color == Color::black, "invalid color");
    return number(rank2N(color == Color::white ? '1' : '8'), queenSideRookFile);
}

template <typename HashPolicy>
template<Color color>
uint8_t ChessBoard<HashPolicy>::getKingSideRookSquare() const {
    static_assert(color == Color::white || color == Color::black, "invalid color");
    return number(rank2N(color == Color::white ? '1' : '8'), kingSideRookFile);
}
