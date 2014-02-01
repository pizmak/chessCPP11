#include "ChessZobristHash.h"
#include "ChessBoard.h"

void ChessZobristHash::updatePiece(uint8_t field, uint8_t color, uint8_t piece) {
    update<BOARD>(field, color, piece);
}

void ChessZobristHash::switchPlayer() {
    update<PLAYER>(0);
}

void ChessZobristHash::updateCastlingCapabilities(uint8_t castlingCapabilities) {
    update<CASTLINGS>(castlingCapabilities);
}

void ChessZobristHash::updateEnPassantFile(uint8_t enPassantFile) {
    update<EN_PASSANT>(enPassantFile);
}

void ChessZobristHash::setRepetition(bool repeated) {
    if (repetition != repeated) {
        update<REPETITION>(repetition);
        update<REPETITION>(repeated);
        repetition = repeated;
    }
}

void ChessZobristHash::resetRepetition() {
    setRepetition(false);
}

void ChessZobristHash::initRepetition() {
    update<REPETITION>(false);
}
