#include "unitTests.h"
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

void boardUnitTest() {
    BoardType b;
    b.dump(std::cerr);
    b.checkIntegrity();
    std::list<Move> moveList = {
        {notation2Number("e2"), notation2Number("e4"), 0, Piece::empty},
        {notation2Number("c7"), notation2Number("c5"), 0, Piece::empty},
        {notation2Number("e4"), notation2Number("e5"), 0, Piece::empty},
        {notation2Number("f7"), notation2Number("f5"), 0, Piece::empty},
        {notation2Number("e5"), notation2Number("f6"), notation2Number("f5"), Piece::empty, MoveFlags::enPassantCapture},
        {notation2Number("g7"), notation2Number("f6"), 0, Piece::pawn},
        {notation2Number("f1"), notation2Number("a6"), 0, Piece::empty},
        {notation2Number("g8"), notation2Number("h6"), 0, Piece::empty},
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
        {notation2Number("g7"), notation2Number("g8"), 0, Piece::empty, MoveFlags::queenPromotion}
    };
    for (const auto &move : moveList) {
        testMove(b, move);
    }

}

void engineMasksTest() {
    for (int i = 0; i < 64; ++i) {
        std::cerr << i << ", black pawn: " << std::endl;
        printBitmaskAsBoard(MoveGenerator::pawnBitmask[toInt(Color::black)][i], std::cerr);
        std::cerr << std::endl;
        std::cerr << i << ", knight: " << std::endl;
        printBitmaskAsBoard(MoveGenerator::knightBitmask[i], std::cerr);
        std::cerr << std::endl;
        std::cerr << i << ", bishop[1]: " << std::endl;
        printBitmaskAsBoard(MoveGenerator::bishopBitmask[i][1], std::cerr);
        std::cerr << std::endl;
        std::cerr << i << ", rook[2]: " << std::endl;
        printBitmaskAsBoard(MoveGenerator::rookBitmask[i][2], std::cerr);
        std::cerr << std::endl;
        std::cerr << i << ", king: " << std::endl;
        printBitmaskAsBoard(MoveGenerator::kingBitmask[i], std::cerr);
        std::cerr << std::endl;
    }
}

void testEngine() {
    Engine engine;
    std::list<std::string> moves{"e2e4", "a7a6", "d2h4"};
    std::list<std::string> moves2{"d2h4", "a7a6", "e2e4"};

    engine.move(moves);
    std::cerr << "hash after moves: " << engine.getBoard().getHash() <<std::endl;
    engine.reset();
    engine.move(moves2);
    std::cerr << "hash after moves: " << engine.getBoard().getHash() <<std::endl;
}

void hashUnitTest() {
    ZobristHash<4, 2, 2> hash;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
//                std::cerr << hash.randomHash[i][j][k] << " ";
            }
            std::cerr << std::endl;
        }
        std::cerr << std::endl;
    }
//    std::cerr << NativeArray<uint64_t, 4,2,2>::get(hash.randomHash, 0,0,0) << std::endl;
}

#include <chrono>

void nativeArrayTest() {
    using ArrayType = NativeArray<int, 6, 6, 6>;
    typename ArrayType::type arr;
    auto fun = []() { static int i = 0; return ++i; };
    ArrayType::initMulti(fun, arr);
    std::chrono::system_clock c;
    std::cout << c.to_time_t(c.now()) << std::endl;
    int ret = 0;
    int arg = c.to_time_t(c.now()) % 6;
    for (int i = 0; i < 1000000000; ++i) {
        ret += ArrayType::get(arr, arg, arg, arg);
    }
    std::cout << c.to_time_t(c.now()) << std::endl;
    std::cout << "ret: " << ret << std::endl;
}
