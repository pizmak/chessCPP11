#include "Move.h"
#include "print.h"
#include <type_traits>

std::ostream &operator<<(std::ostream &stream, const Move &move) {
    stream << "Move(from, to, flags, enPassant, captured, score) = (";
    print(stream, (int)move.from, (int)move.to, move.flags, (int)move.enPassantSquare, move.captured, move.score);
    stream << ")";
    return stream;
}

Piece promotionPiece(EnumFlags<MoveFlags> promotionType) {
    ASSERT(bit::numberOfOnes(toInt(promotionType & promotions)) == 1, promotionType);
    if (promotionType & MoveFlags::queenPromotion) {
        return Piece::queen;
    }
    if (promotionType & MoveFlags::knightPromotion) {
        return Piece::knight;
    }
    if (promotionType & MoveFlags::bishopPromotion) {
        return Piece::bishop;
    }
    if (promotionType & MoveFlags::rookPromotion) {
        return Piece::rook;
    }
    return Piece::empty;
}
