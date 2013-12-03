#pragma once

#include "Board.h"
#include "enums.h"

ENUM(StageOfGame, uint8_t,
        OPENING, 0,
        MIDDLEGAME, 1,
        ENDGAME, 2
)

class ScorePosition {
public:
    ScorePosition();
    int16_t scorePosition(const Board &board);
    void updateStageOfGame(const Board &board);
    static void initialize();
private:
    StageOfGame gameStage;
    static const int16_t piecesValues[];
    static const int16_t knight_bonus[];
    static const int16_t bishop_bonus[];
    static const int16_t king_end_bonus[];
    static int16_t distance[64][64];

    static constexpr int16_t oneButLastLineBonus = 100;
    static constexpr int16_t neighboardPawnBonus = 3;
    static constexpr int16_t doubledPawnsPenalty = 20;
    uint16_t centrumBonus(uint8_t square);
    StageOfGame stageOfGame(const Board &board);
    template <Color color> int16_t scorePawn(const Board &board, uint8_t square);
    template <Color color> int16_t scoreRook(const Board &board, uint8_t square, uint8_t king_position[2]);
};
