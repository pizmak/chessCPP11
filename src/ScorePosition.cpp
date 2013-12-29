#include "ScorePosition.h"

#include "Color.h"
#include "notation.h"
#include "Piece.h"
#include "Engine.h"
#include "logging.h"

int16_t ScorePosition::distance[64][64];
StageOfGame ScorePosition::gameStage = StageOfGame::OPENING;
const int16_t ScorePosition::piecesValues[] = {100, 300, 300, 500, 900, 9000, 0}; // indexed by Piece
const int16_t ScorePosition::knight_bonus[] = {
        -10, -15, -10, -10, -10, -10, -15, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, -15, -10, -10, -10, -10, -15, -10
};

const int16_t ScorePosition::bishop_bonus[] = {
        -10, -10, -10, -10, -10, -10, -10, -10,
        -10, 7, 0, 0, 0, 0, 7, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -10, 7, 0, 0, 0, 0, 7, -10,
        -10, -10, -10, -10, -10, -10, -10, -10
};

const int16_t ScorePosition::king_end_bonus[] = {
        0, 10, 20, 30, 30, 20, 10, 0,
        10, 20, 30, 40, 40, 30, 20, 10,
        20, 30, 40, 50, 50, 40, 30, 20,
        30, 40, 50, 60, 60, 50, 40, 30,
        30, 40, 50, 60, 60, 50, 40, 30,
        20, 30, 40, 50, 50, 40, 30, 20,
        10, 20, 30, 40, 40, 30, 20, 10,
        0, 10, 20, 30, 30, 20, 10, 0
};

uint16_t ScorePosition::centrumBonus(uint8_t square) {
    if (square >= 18 && square <= 45 && file(square) >= 2 && file(square) <= 5) {
        return square >= 26 && square <= 37 ? 20 : 10;
    } else {
        return 0;
    }
}

template <Color color> int16_t ScorePosition::scorePawn(const BoardType &board, uint8_t square) {
    ASSERT(square > 7 && square < 0x38, square);
    static_assert(color == Color::white || color == Color::black, "invalid color");
    constexpr uint8_t bonusLine = color == Color::white ? 7 : 2;
    int16_t ret = piecesValues[toInt(Piece::pawn)] + centrumBonus(square);
    if (rank(square) == bonusLine) {
        ret += oneButLastLineBonus;
    } else if (Engine::pawnBitmask[toInt(color)][square] & board.bitmask[toInt(color)][toInt(Piece::pawn)]) {
        ret += neighboardPawnBonus;
    } else if (bit::isSet(board.bitmask[toInt(color)][toInt(Piece::pawn)], square - 8)) {
        ret -= doubledPawnsPenalty;
    }
    return color == Color::white ? ret : -ret;
}

template <Color color> int16_t ScorePosition::scoreKnight(const BoardType &board __attribute__((unused)), uint8_t square, uint8_t king_position[2]) {
    int16_t ret = piecesValues[toInt(Piece::knight)] + centrumBonus(square);
    ret += knight_bonus[square];
    ret -= 4 * distance[square][king_position[toInt(opponent(color))]]; // im wieksza odleglosc tym wieksza kara;
    if (gameStage == StageOfGame::OPENING) {
        if (color == Color::white) {
            if (square == 1 || square == 6) {// kara za trzymanie skokow
                ret -= 30;
            }
        } else {
            if (square == 57 || square == 62) {// kara za trzymanie skokow
                ret -= 30;
            }
        }
    }
    return color == Color::white ? ret : -ret;
}

template <Color color> int16_t ScorePosition::scoreBishop(const BoardType &board __attribute__((unused)), uint8_t square, uint8_t king_position[2]) {
    int16_t ret = piecesValues[toInt(Piece::bishop)] + centrumBonus(square);
    ret += bishop_bonus[square];
    ret -= 2 * distance[square][king_position[toInt(opponent(color))]]; // im wieksza odleglosc tym wieksza kara;
    return color == Color::white ? ret : -ret;
}

template <Color color> int16_t ScorePosition::scoreRook(const BoardType &board __attribute__((unused)), uint8_t square, uint8_t king_position[2]) {
    constexpr uint8_t penaltyRank = color == Color::white ? 1 : 6;
    int16_t ret = piecesValues[toInt(Piece::rook)] + centrumBonus(square);
    ret += -2 * distance[square][king_position[toInt(opponent(color))]];
    ret -= int(rank(square) == penaltyRank) * 20;
    return color == Color::white ? ret : -ret;
}

template <Color color> int16_t ScorePosition::scoreQueen(const BoardType &board __attribute__((unused)), uint8_t square, uint8_t king_position[2]) {
    int16_t ret = piecesValues[toInt(Piece::queen)] + centrumBonus(square);
    ret -= 3 * distance[square][king_position[toInt(opponent(color))]];
    if (gameStage == StageOfGame::OPENING) {
        ret += 3 * (distance[square][27] + distance[square][36]); //premia za trzymanie hetmana na wodzy
    }
    return color == Color::white ? ret : -ret;
}

template <Color color> int16_t ScorePosition::scoreKing(const BoardType &board __attribute__((unused)), uint8_t square, uint8_t king_position[2] __attribute__((unused))) {
    int16_t ret = piecesValues[toInt(Piece::king)] + centrumBonus(square);
    if (gameStage == StageOfGame::ENDGAME) {
        ret += king_end_bonus[square];
    } else { // jesli nie koncowka to krol w bezpiecznym miejscu
//                if (color == Color::white) {
//                    sum += white_king_start_bonus[square];
//                    if (square > 60) { // po roszadzie krotkiej // stopien ochrony krola pionami
//                        sum += (15 * (czy_biale_pole(53) + czy_biale_pole(54) + czy_biale_pole(55))
//                                + 5 * (czy_biale_pole(46) + czy_biale_pole(47)));
//                    } else if (square < 59) { // po dlugiej
//                        sum += (15 * (czy_biale_pole(48) + czy_biale_pole(49) + czy_biale_pole(50))
//                                + 5 * (czy_biale_pole(40) + czy_biale_pole(41)));
//                    }
//                } else { // stopien ochrony krola czarnego pionami
//                    sum += black_king_start_bonus[square];
//                    if (square > 4) { // po roszadzie krotkiej // stopien ochrony krola pionami
//                        sum -= (15 * (czy_biale_pole(13) + czy_biale_pole(14) + czy_biale_pole(15))
//                                + 5 * (czy_biale_pole(22) + czy_biale_pole(23)));
//                    } else if (square < 3) { // po dlugiej
//                        sum -= (15 * (czy_biale_pole(8) + czy_biale_pole(9) + czy_biale_pole(10))
//                                + 5 * (czy_biale_pole(16) + czy_biale_pole(17)));
//                    }
//                }
    }
    return color == Color::white ? ret : -ret;
}

void ScorePosition::updateStageOfGame(const BoardType &board) {
    gameStage = stageOfGame(board);
}

StageOfGame ScorePosition::stageOfGame(const BoardType &board) {
    int fullBoard = 2 * 100 + 4 * 50 + 8 * 10 + 16 * 2;
    int currentScore = bit::numberOfOnes(board.bitmask[toInt(Color::white)][toInt(Piece::pawn)]) * 2
            + bit::numberOfOnes(board.bitmask[toInt(Color::black)][toInt(Piece::pawn)]) * 2
            + bit::numberOfOnes(board.bitmask[toInt(Color::white)][toInt(Piece::knight)]) * 10
            + bit::numberOfOnes(board.bitmask[toInt(Color::black)][toInt(Piece::knight)]) * 10
            + bit::numberOfOnes(board.bitmask[toInt(Color::white)][toInt(Piece::bishop)]) * 10
            + bit::numberOfOnes(board.bitmask[toInt(Color::black)][toInt(Piece::bishop)]) * 10
            + bit::numberOfOnes(board.bitmask[toInt(Color::white)][toInt(Piece::rook)]) * 50
            + bit::numberOfOnes(board.bitmask[toInt(Color::black)][toInt(Piece::rook)]) * 50
            + bit::numberOfOnes(board.bitmask[toInt(Color::white)][toInt(Piece::queen)]) * 100
            + bit::numberOfOnes(board.bitmask[toInt(Color::black)][toInt(Piece::queen)]) * 100;
    if (currentScore > fullBoard - 2 * 10 - 4 * 2) {
        return StageOfGame::OPENING;
    }
    if (currentScore <= 100 || bit::numberOfOnes(board.allPieces()) < 6) {
        return StageOfGame::ENDGAME;
    }
    return StageOfGame::MIDDLEGAME;
}

int ScorePosition::numberOfCalls = 0;

int16_t ScorePosition::scorePosition(const BoardType &board) {
    int16_t sum = 0;
    ++numberOfCalls;
//    Piece piece;
//    Color color;

    TRACE(std::dec);
    uint8_t king_position[2] = {bit::mostSignificantBit(board.bitmask[toInt(Color::white)][toInt(Piece::king)]), bit::mostSignificantBit(board.bitmask[toInt(Color::black)][toInt(Piece::king)])};

    bit::foreach_bit(board.bitmask[toInt(Color::white)][toInt(Piece::pawn)], [&board, &sum](uint8_t bit){ sum += scorePawn<Color::white>(board, bit);});
    bit::foreach_bit(board.bitmask[toInt(Color::black)][toInt(Piece::pawn)], [&board, &sum](uint8_t bit){ sum += scorePawn<Color::black>(board, bit);});

    bit::foreach_bit(board.bitmask[toInt(Color::white)][toInt(Piece::rook)], [&board, &sum, &king_position](uint8_t bit){ sum += scoreRook<Color::white>(board, bit, king_position);});
    bit::foreach_bit(board.bitmask[toInt(Color::black)][toInt(Piece::rook)], [&board, &sum, &king_position](uint8_t bit){ sum += scoreRook<Color::black>(board, bit, king_position);});

    bit::foreach_bit(board.bitmask[toInt(Color::white)][toInt(Piece::bishop)], [&board, &sum, &king_position](uint8_t bit){ sum += scoreBishop<Color::white>(board, bit, king_position);});
    bit::foreach_bit(board.bitmask[toInt(Color::black)][toInt(Piece::bishop)], [&board, &sum, &king_position](uint8_t bit){ sum += scoreBishop<Color::black>(board, bit, king_position);});

    bit::foreach_bit(board.bitmask[toInt(Color::white)][toInt(Piece::knight)], [&board, &sum, &king_position](uint8_t bit){ sum += scoreKnight<Color::white>(board, bit, king_position);});
    bit::foreach_bit(board.bitmask[toInt(Color::black)][toInt(Piece::knight)], [&board, &sum, &king_position](uint8_t bit){ sum += scoreKnight<Color::black>(board, bit, king_position);});

    bit::foreach_bit(board.bitmask[toInt(Color::white)][toInt(Piece::queen)], [&board, &sum, &king_position](uint8_t bit){ sum += scoreQueen<Color::white>(board, bit, king_position);});
    bit::foreach_bit(board.bitmask[toInt(Color::black)][toInt(Piece::queen)], [&board, &sum, &king_position](uint8_t bit){ sum += scoreQueen<Color::black>(board, bit, king_position);});

    bit::foreach_bit(board.bitmask[toInt(Color::white)][toInt(Piece::king)], [&board, &sum, &king_position](uint8_t bit){ sum += scoreKing<Color::white>(board, bit, king_position);});
    bit::foreach_bit(board.bitmask[toInt(Color::black)][toInt(Piece::king)], [&board, &sum, &king_position](uint8_t bit){ sum += scoreKing<Color::black>(board, bit, king_position);});

/*
    for (uint8_t square = 0; square < 64; square++) {
        piece = board.pieces[square];
        if (piece == Piece::empty) {
            continue;
        }
        color = board.piecesColors[square];
        multiplier = color == Color::white ? 1 : -1;

        sum += (piecesValues[toInt(piece)] + centrumBonus(square)) * multiplier; // obliczenie materialu

        switch (piece) {
        case Piece::pawn:
            if (color == Color::white) {
                sum += scorePawn<Color::white>(board, square);
            } else {
                sum += scorePawn<Color::black>(board, square);
            }
            break;
        case Piece::rook:
            if (color == Color::white) {
                sum += scoreRook<Color::white>(board, square, king_position);
            } else {
                sum += scoreRook<Color::black>(board, square, king_position);
            }
            break;
        case Piece::knight :
            if (color == Color::white) {
                sum += scoreKnight<Color::white>(board, square, king_position);
            } else {
                sum += scoreKnight<Color::black>(board, square, king_position);
            }
            break;
        case Piece::bishop:
            if (color == Color::white) {
                sum += scoreBishop<Color::white>(board, square, king_position);
            } else {
                sum += scoreBishop<Color::black>(board, square, king_position);
            }
            break;
        case Piece::queen:
            if (color == Color::white) {
                sum += scoreQueen<Color::white>(board, square, king_position);
            } else {
                sum += scoreQueen<Color::black>(board, square, king_position);
            }
            break;
        case Piece::king:
            if (color == Color::white) {
                sum += scoreKing<Color::white>(board, square, king_position);
            } else {
                sum += scoreKing<Color::black>(board, square, king_position);
            }
            break;
        case Piece::empty:
            ASSERT(false, piece);
            break;
        }
    }
    */

    TRACE(std::hex);
    return sum;
}

void ScorePosition::initialize() {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;
    for (uint8_t x = 0; x < 64; x++) {
        for (uint8_t y = 0; y < 64; y++) {
            distance[x][y] = std::max(std::max(file(x), file(y)) - std::min(file(x), file(y)), std::max(rank(x), rank(y)) - std::min(rank(x), rank(y)));
        }
    }
}
