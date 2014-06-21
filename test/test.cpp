#include <iostream>
#include "gtest/gtest.h"

#include "ChessBoard.h"
#include "Move.h"
#include "notation.h"
#include "zobristHashing/ZobristHash.h"
#include "Engine.h"
#include "MoveGenerator.h"
#include "utils/NativeArray.h"

#include <list>
#include <iostream>

void testMove(BoardType &b, const Move &m) {
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

TEST(boardUnitTest, test1) {
    BoardType b;
    std::list<Move> moveList = {
        {notation2Number("e2"), notation2Number("e4"), 0, Piece::empty},
        {notation2Number("c7"), notation2Number("c5"), 0, Piece::empty}
        //{notation2Number("e4"), notation2Number("e5"), 0, Piece::empty},
        //{notation2Number("f7"), notation2Number("f5"), 0, Piece::empty},
        //{notation2Number("e5"), notation2Number("f6"), notation2Number("f5"), Piece::empty, MoveFlags::enPassantCapture},
        //{notation2Number("g7"), notation2Number("f6"), 0, Piece::pawn},
        //{notation2Number("f1"), notation2Number("a6"), 0, Piece::empty},
        /*{notation2Number("g8"), notation2Number("h6"), 0, Piece::empty},
        {notation2Number("g1"), notation2Number("f3"), 0, Piece::empty},
        {notation2Number("d8"), notation2Number("a5"), 0, Piece::empty},
        {notation2Number("e1"), notation2Number("g1"), 0, Piece::empty, MoveFlags::castling | MoveFlags::K_castling | MoveFlags::Q_castling},
        {notation2Number("b7"), notation2Number("a6"), 0, Piece::bishop},
        {notation2Number("g2"), notation2Number("g4"), 0, Piece::empty},
        {notation2Number("c8"), notation2Number("b7"), notation2Number("g4"), Piece::empty},
        {notation2Number("g4"), notation2Number("g5"), 0, Piece::empty},
        {notation2Number("b8"), notation2Number("c6"), 0, Piece::empty},
        {notation2Number("g5"), notation2Number("g6"), 0, Piece::empty},
        {notation2Number("a8"), notation2Number("c8"), 0, Piece::empty, MoveFlags::castling | MoveFlags::q_castling},
        {notation2Number("g6"), notation2Number("g7"), 0, Piece::empty},
        {notation2Number("a5"), notation2Number("a4"), 0, Piece::empty},
        {notation2Number("g7"), notation2Number("g8"), 0, Piece::empty, MoveFlags::queenPromotion}*/
    };
    for (const auto &move : moveList) {
        b.makeMove(move);
    }
    ASSERT_EQ(b.getPiece(notation2Number("e4")), Piece::pawn);
    EXPECT_EQ(b.getPiece(notation2Number("e5")), Piece::pawn);

}

/*
TEST(SquareRootTest, PositiveNos) { 
        EXPECT_EQ (18.0, 18.0);
}

TEST (SquareRootTest, ZeroAndNegativeNos) { 
        ASSERT_EQ (0.0, 0);
}

*/
GTEST_API_ int main(int argc, char **argv) {
      std::cout << "Running main() from testmain.cc\n";
       
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}
