#pragma once

#include "ChessBoardDefines.h"
#include "enums.h"

ENUM(StageOfGame, uint8_t,
        OPENING, 0,
        MIDDLEGAME, 1,
        ENDGAME, 2
)

class ChessEvaluator {
public:
    ChessEvaluator() = delete;
    static int16_t evaluate(const BoardType &board);
    static void updateStageOfGame(const BoardType &board);
    static void initialize();
    static int numberOfCalls;
private:
    static StageOfGame gameStage;
    static const int16_t piecesValues[];
    static const int16_t knight_bonus[];
    static const int16_t bishop_bonus[];
    static const int16_t king_end_bonus[];
    static const int16_t centrum_bonus[];
    static int16_t distance[64][64];

    static constexpr int16_t oneButLastLineBonus = 100;
    static constexpr int16_t neighboardPawnBonus = 3;
    static constexpr int16_t doubledPawnsPenalty = 20;
    static StageOfGame stageOfGame(const BoardType &board);
    template <Color color> static int16_t evaluatePawn(const BoardType &board, uint8_t square);
    template <Color color> static int16_t evaluateRook(const BoardType &board, uint8_t square, uint8_t king_position[2]);
    template <Color color> static int16_t evaluateKnight(const BoardType &board, uint8_t square, uint8_t king_position[2]);
    template <Color color> static int16_t evaluateBishop(const BoardType &board, uint8_t square, uint8_t king_position[2]);
    template <Color color> static int16_t evaluateQueen(const BoardType &board, uint8_t square, uint8_t king_position[2]);
    template <Color color> static int16_t evaluateKing(const BoardType &board, uint8_t square, uint8_t king_position[2]);
};
