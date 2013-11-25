#include "Board.h"
#include "Move.h"
#include "notation.h"
#include <iostream>
#include <iomanip>
#include "bit.h"
#include <list>

void testMove(Board &b, const Move &m) {
    std::cerr << std::endl << "make move" << m << std::endl;
    b.makeMove(m);
    b.dump(std::cerr);
    b.checkIntegrity();
    std::cerr << "unmake move" << m << std::endl;
    b.unmakeMove(m);
    b.dump(std::cerr);
    b.checkIntegrity();
    std::cerr << std::endl << "make move" << m << std::endl;
    b.makeMove(m);
    b.dump(std::cerr);
}

int main() {
    std::cerr << std::hex << std::boolalpha << std::uppercase << std::setfill('0') << std::setiosflags(std::ios::internal);
    Board b;
    b.dump(std::cerr);
    b.checkIntegrity();

    std::list<Move> moveList = {
            {notation2Number("e2"), notation2Number("e4"), Piece::empty},
            {notation2Number("c7"), notation2Number("c5"), Piece::empty},
            {notation2Number("e4"), notation2Number("e5"), Piece::empty},
            {notation2Number("f7"), notation2Number("f5"), Piece::empty},
            {notation2Number("e5"), notation2Number("f6"), Piece::empty, MoveFlags::enPassantCapture, notation2Number("f5")},
            {notation2Number("g7"), notation2Number("f6"), Piece::pawn, MoveFlags::empty},
            {notation2Number("f1"), notation2Number("a6"), Piece::empty},
            {notation2Number("g8"), notation2Number("h6"), Piece::empty},
            {notation2Number("g1"), notation2Number("f3"), Piece::empty},
            {notation2Number("d8"), notation2Number("a5"), Piece::empty},
            {notation2Number("e1"), notation2Number("g1"), Piece::empty, MoveFlags::castling | MoveFlags::w_king_first | MoveFlags::w_k_rook_first},
            {notation2Number("b7"), notation2Number("a6"), Piece::bishop},
            {notation2Number("g2"), notation2Number("g4"), Piece::empty},
            {notation2Number("c8"), notation2Number("b7"), Piece::empty, MoveFlags::empty, notation2Number("g4")},
            {notation2Number("g4"), notation2Number("g5"), Piece::empty},
            {notation2Number("b8"), notation2Number("c6"), Piece::empty},
            {notation2Number("g5"), notation2Number("g6"), Piece::empty},
            {notation2Number("e8"), notation2Number("c8"), Piece::empty, MoveFlags::castling | MoveFlags::b_king_first | MoveFlags::b_q_rook_first},
            {notation2Number("g6"), notation2Number("g7"), Piece::empty},
            {notation2Number("a5"), notation2Number("a4"), Piece::empty},
            {notation2Number("g7"), notation2Number("g8"), Piece::empty, MoveFlags::queenPromotion}
    };
    for (const auto &move : moveList) {
        testMove(b, move);
    }
    return 0;
}
