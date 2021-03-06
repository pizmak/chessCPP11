#include "ChessEvaluator.h"

#include "Color.h"
#include "notation.h"
#include "Piece.h"
#include "utils/logging.h"
#include "MoveGenerator.h"
#include "utils/Statistics.h"

int16_t ChessEvaluator::distance[64][64];
StageOfGame ChessEvaluator::gameStage = StageOfGame::OPENING;
const int16_t ChessEvaluator::piecesValues[] = {100, 300, 320, 500, 900, 9000, 0}; // indexed by Piece

const int16_t ChessEvaluator::knight_bonus[] = {
    -20, -15, -10, -10, -10, -10, -15, -20,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,   5,   5,   5,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   5,   5,   5,   5,   0, -10,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -20, -15, -10, -10, -10, -10, -15, -20
};

const int16_t ChessEvaluator::bishop_bonus[] = {
      5, -10, -20, -10, -10, -20, -10,   5,
    -10,  10,   0,   0,   0,   0,  10, -10,
    -10,   0,   7,   5,   5,   7,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   7,   5,   5,   7,   0, -10,
    -10,  10,   0,   0,   0,   0,  10, -10,
      5, -10, -20, -10, -10, -20, -10,   5
};

const int16_t ChessEvaluator::king_end_bonus[] = {
     0, 10, 20, 30, 30, 20, 10,  0,
    10, 20, 30, 40, 40, 30, 20, 10,
    20, 30, 40, 50, 50, 40, 30, 20,
    30, 40, 50, 60, 60, 50, 40, 30,
    30, 40, 50, 60, 60, 50, 40, 30,
    20, 30, 40, 50, 50, 40, 30, 20,
    10, 20, 30, 40, 40, 30, 20, 10,
     0, 10, 20, 30, 30, 20, 10,  0
};

const int16_t ChessEvaluator::king_start_bonus[] = {
    5, 15,  10,   0,   0,  10, 15, 0,
    0,  0,   0, -20, -20,   0,  0, 0,
    0,  0, -10, -10, -10, -10,  0, 0,
    0,  0, -20, -20, -20, -20,  0, 0,
    0,  0, -20, -20, -20, -20,  0, 0,
    0,  0, -10, -10, -10, -10,  0, 0,
    0,  0,   0, -20, -20,   0,  0, 0,
    5, 15,  10,   0,   0,  10, 15, 0
};

const int16_t ChessEvaluator::centrum_bonus[] = {
    0, 0,  0,  0,  0,  0, 0, 0,
    0, 0,  0,  0,  0,  0, 0, 0,
    0, 0, 10, 10, 10, 10, 0, 0,
    0, 0, 20, 20, 20, 20, 0, 0,
    0, 0, 20, 20, 20, 20, 0, 0,
    0, 0, 10, 10, 10, 10, 0, 0,
    0, 0,  0,  0,  0,  0, 0, 0,
    0, 0,  0,  0,  0,  0, 0, 0
};

template <Color color> int16_t ChessEvaluator::evaluatePawn(const BoardType &board, uint8_t square) {
    ASSERT(square >= n2N("a2") && square <= n2N("h7"), square);
    static_assert(color == Color::white || color == Color::black, "invalid color");
    CONSTEXPR uint8_t bonusLine = color == Color::white ? r2N('7') : r2N('2');
    int16_t ret = centrum_bonus[square];
    if (rank(square) == bonusLine) {
        ret += oneButLastLineBonus;
    } else if (MoveGenerator::pawnBitmask[toInt(color)][square] & board.getBitmask(color, Piece::pawn)) {
        ret += neighboardPawnBonus;
    } else if (bit::isSet(board.getBitmask(color, Piece::pawn), square - 8)) {
        ret -= doubledPawnsPenalty;
    }
    return color == Color::white ? ret : -ret;
}

template <Color color> int16_t ChessEvaluator::evaluateKnight(const BoardType &board __attribute__((unused)), uint8_t square, uint8_t king_position[2]) {
    int16_t ret = centrum_bonus[square];
    ret += knight_bonus[square];
    ret -= 4 * distance[square][king_position[toInt(opponent(color))]]; // im wieksza odleglosc tym wieksza kara;
    return color == Color::white ? ret : -ret;
}

template <Color color> int16_t ChessEvaluator::evaluateBishop(const BoardType &board __attribute__((unused)), uint8_t square, uint8_t king_position[2]) {
    int16_t ret = centrum_bonus[square];
    ret += bishop_bonus[square];
    ret -= 2 * distance[square][king_position[toInt(opponent(color))]]; // im wieksza odleglosc tym wieksza kara;
    return color == Color::white ? ret : -ret;
}

template <Color color> int16_t ChessEvaluator::evaluateRook(const BoardType &board __attribute__((unused)), uint8_t square, uint8_t king_position[2]) {
    CONSTEXPR uint8_t penaltyRank = color == Color::white ? r2N('7') : r2N('2');
    int16_t ret = centrum_bonus[square];
    ret += -2 * distance[square][king_position[toInt(opponent(color))]];
    ret += int(rank(square) == penaltyRank) * rookOnOneButLastRankBonus;
    // TODO rooks connected and rooks on open files
    return color == Color::white ? ret : -ret;
}

template <Color color> int16_t ChessEvaluator::evaluateQueen(const BoardType &board __attribute__((unused)), uint8_t square, uint8_t king_position[2]) {
    int16_t ret = centrum_bonus[square];
    ret -= 3 * distance[square][king_position[toInt(opponent(color))]];
    if (gameStage == StageOfGame::OPENING) {
        ret += 3 * (distance[square][n2N("e4")] + distance[square][n2N("e5")]); //premia za trzymanie hetmana na wodzy
    }
    return color == Color::white ? ret : -ret;
}

template <Color color> int16_t ChessEvaluator::evaluateKing(const BoardType &board __attribute__((unused)), uint8_t square, uint8_t king_position[2] __attribute__((unused))) {
    int16_t ret = 0;
    if (gameStage == StageOfGame::ENDGAME) {
        ret += king_end_bonus[square];
    } else { // jesli nie koncowka to krol w bezpiecznym miejscu
        ret += king_start_bonus[square];
        if (color == Color::white) { // po roszadzie
            if (square >= n2N("f1") && square <= n2N("h1") && board.getPiece(n2N("h1")) != Piece::rook && board.getPiece(n2N("g1")) != Piece::rook) {
                ret += castlePawnBonus * ((board.getPieceColor(n2N("f2")) == color) + (board.getPieceColor(n2N("g2")) == color) + (board.getPieceColor(n2N("h2")) == color));
            } else if (square <= n2N("c1") && board.getPiece(n2N("a1")) != Piece::rook && board.getPiece(n2N("b1")) != Piece::rook) {
                ret += castlePawnBonus * ((board.getPieceColor(n2N("a2")) == color) + (board.getPieceColor(n2N("b2")) == color) + (board.getPieceColor(n2N("c2")) == color));
            }
        }
        else /*black*/ {  // po roszadzie
            if (square >= n2N("a8") && square <= n2N("c8") && board.getPiece(n2N("a8")) != Piece::rook && board.getPiece(n2N("b8")) != Piece::rook) {
                ret += castlePawnBonus * ((board.getPieceColor(n2N("a7")) == color) + (board.getPieceColor(n2N("b7")) == color) + (board.getPieceColor(n2N("c7")) == color));
            } else if (square >= n2N("f8") && board.getPiece(n2N("h8")) != Piece::rook && board.getPiece(n2N("g8")) != Piece::rook) {
                ret += castlePawnBonus * ((board.getPieceColor(n2N("f7")) == color) + (board.getPieceColor(n2N("g7")) == color) + (board.getPieceColor(n2N("h7")) == color));
            }
        }
    }
    return color == Color::white ? ret : -ret;
}

void ChessEvaluator::updateStageOfGame(const BoardType &board) {
    gameStage = stageOfGame(board);
}

StageOfGame ChessEvaluator::stageOfGame(const BoardType &board) {
    int fullBoard = 2 * 100 + 4 * 50 + 8 * 10 + 16 * 2;
    int currentScore = bit::numberOfOnes(board.getBitmask(Color::white, Piece::pawn)) * 2
            + bit::numberOfOnes(board.getBitmask(Color::black, Piece::pawn)) * 2
            + bit::numberOfOnes(board.getBitmask(Color::white, Piece::knight)) * 10
            + bit::numberOfOnes(board.getBitmask(Color::black, Piece::knight)) * 10
            + bit::numberOfOnes(board.getBitmask(Color::white, Piece::bishop)) * 10
            + bit::numberOfOnes(board.getBitmask(Color::black, Piece::bishop)) * 10
            + bit::numberOfOnes(board.getBitmask(Color::white, Piece::rook)) * 50
            + bit::numberOfOnes(board.getBitmask(Color::black, Piece::rook)) * 50
            + bit::numberOfOnes(board.getBitmask(Color::white, Piece::queen)) * 100
            + bit::numberOfOnes(board.getBitmask(Color::black, Piece::queen)) * 100;
    if (currentScore > fullBoard - 2 * 10 - 4 * 2) {
        return StageOfGame::OPENING;
    }
    if (currentScore <= 100 || bit::numberOfOnes(board.allPieces()) < 6) {
        return StageOfGame::ENDGAME;
    }
    return StageOfGame::MIDDLEGAME;
}

int16_t ChessEvaluator::evaluate(const BoardType &board) {
    int16_t sum = board.materialDifference;
    Statistics::globalStatistics().increment("evaluator.callsNumber");

    uint8_t king_position[2] = {bit::mostSignificantBit(board.getBitmask(Color::white, Piece::king)), bit::mostSignificantBit(board.getBitmask(Color::black, Piece::king))};

    bit::foreach_bit(board.getBitmask(Color::white, Piece::pawn), [&board, &sum](uint8_t bit){ sum += evaluatePawn<Color::white>(board, bit);});
    bit::foreach_bit(board.getBitmask(Color::black, Piece::pawn), [&board, &sum](uint8_t bit){ sum += evaluatePawn<Color::black>(board, bit);});

    bit::foreach_bit(board.getBitmask(Color::white, Piece::rook), [&board, &sum, &king_position](uint8_t bit){ sum += evaluateRook<Color::white>(board, bit, king_position);});
    bit::foreach_bit(board.getBitmask(Color::black, Piece::rook), [&board, &sum, &king_position](uint8_t bit){ sum += evaluateRook<Color::black>(board, bit, king_position);});

    bit::foreach_bit(board.getBitmask(Color::white, Piece::bishop), [&board, &sum, &king_position](uint8_t bit){ sum += evaluateBishop<Color::white>(board, bit, king_position);});
    bit::foreach_bit(board.getBitmask(Color::black, Piece::bishop), [&board, &sum, &king_position](uint8_t bit){ sum += evaluateBishop<Color::black>(board, bit, king_position);});

    bit::foreach_bit(board.getBitmask(Color::white, Piece::knight), [&board, &sum, &king_position](uint8_t bit){ sum += evaluateKnight<Color::white>(board, bit, king_position);});
    bit::foreach_bit(board.getBitmask(Color::black, Piece::knight), [&board, &sum, &king_position](uint8_t bit){ sum += evaluateKnight<Color::black>(board, bit, king_position);});

    bit::foreach_bit(board.getBitmask(Color::white, Piece::queen), [&board, &sum, &king_position](uint8_t bit){ sum += evaluateQueen<Color::white>(board, bit, king_position);});
    bit::foreach_bit(board.getBitmask(Color::black, Piece::queen), [&board, &sum, &king_position](uint8_t bit){ sum += evaluateQueen<Color::black>(board, bit, king_position);});

    bit::foreach_bit(board.getBitmask(Color::white, Piece::king), [&board, &sum, &king_position](uint8_t bit){ sum += evaluateKing<Color::white>(board, bit, king_position);});
    bit::foreach_bit(board.getBitmask(Color::black, Piece::king), [&board, &sum, &king_position](uint8_t bit){ sum += evaluateKing<Color::black>(board, bit, king_position);});

    return sum;
}

void ChessEvaluator::initialize() {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;
    for (uint8_t x = n2N("a1"); x <= n2N("h8"); x++) {
        for (uint8_t y = n2N("a1"); y <= n2N("h8"); y++) {
            distance[x][y] = std::max(std::max(file(x), file(y)) - std::min(file(x), file(y)), std::max(rank(x), rank(y)) - std::min(rank(x), rank(y)));
        }
    }
}
