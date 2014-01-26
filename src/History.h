#pragma once
#include <cstdint>
#include <iosfwd>
#include <vector>

#include "asserts.h"

template <typename BoardType>
class History {
    // TODO hashes are potentially not unique, maybe some better heuristic for positions comparison should be used
    struct HistoryElement {
        uint64_t hash;
        uint16_t movesFromLastCaptureOrPawnMove;
        HistoryElement(uint64_t hash, uint64_t movesFromLastCaptureOrPawnMove) : hash(hash), movesFromLastCaptureOrPawnMove(movesFromLastCaptureOrPawnMove) {}
    };
    using HistoryVector = std::vector<HistoryElement>;
    HistoryVector historyTable;
    bool topPositionThreeTimesRepeated();
public:
    bool isDraw();
    void push(BoardType &board, bool captureOrPawnMove);
    void pop();
    void clear(BoardType &board);
    void printHistory();
};

template <typename BoardType>
bool History<BoardType>::isDraw() {
    ASSERT(historyTable.size() > 0, historyTable.size());
    return historyTable.back().movesFromLastCaptureOrPawnMove >= 100 || topPositionThreeTimesRepeated();
}

template <typename BoardType>
void History<BoardType>::push(BoardType &board, bool captureOrPawnMove) {
    ASSERT(historyTable.size() > 0, historyTable.size());
    historyTable.emplace_back(board.getHash(), (captureOrPawnMove ? 0 : historyTable.back().movesFromLastCaptureOrPawnMove + 1));
}

template <typename BoardType>
void History<BoardType>::pop() {
    ASSERT(historyTable.size() > 1, historyTable.size());
    historyTable.pop_back();
}

template <typename BoardType>
void History<BoardType>::clear(BoardType &board) {
    historyTable.clear();
    historyTable.emplace_back(board.getHash(), 0);
}

template <typename BoardType>
bool History<BoardType>::topPositionThreeTimesRepeated() {
    ASSERT(historyTable.size() > 0, historyTable.size());
    int count = 1;
    uint64_t searchedHash = historyTable.back().hash;
    uint16_t lastMoves = historyTable.back().movesFromLastCaptureOrPawnMove;
    for (typename HistoryVector::const_reverse_iterator iter = historyTable.crbegin() + 1; iter != historyTable.crend() && iter->movesFromLastCaptureOrPawnMove < lastMoves && count < 3; ++iter) {
        lastMoves = iter->movesFromLastCaptureOrPawnMove;
        count += uint8_t(iter->hash == searchedHash);
    }
    return count >= 3;
}

template <typename BoardType>
void History<BoardType>::printHistory() {
    std::cerr << "current pos: " << historyTable.back().hash << std::endl;
    int count = 1;
    uint64_t searchedHash = historyTable.back().hash;
    uint16_t lastMoves = historyTable.back().movesFromLastCaptureOrPawnMove;
    for (typename HistoryVector::const_reverse_iterator iter = historyTable.crbegin() + 1; iter != historyTable.crend() && iter->movesFromLastCaptureOrPawnMove < lastMoves && count < 3; ++iter) {
        lastMoves = iter->movesFromLastCaptureOrPawnMove;
        count += uint8_t(iter->hash == searchedHash);
        std::cerr << "history element: " << iter->hash << ", " << iter->movesFromLastCaptureOrPawnMove << std::endl;
    }
}
