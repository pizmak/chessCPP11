#include "History.h"
#include "utils/asserts.h"

bool History::isDraw() {
    ASSERT(historyTable.size() > 0, historyTable.size());
    return historyTable.back().movesFromLastCaptureOrPawnMove >= 100 || topPositionThreeTimesRepeated();
}

void History::push(uint64_t hash, bool captureOrPawnMove) {
    ASSERT(historyTable.size() > 0, historyTable.size());
    historyTable.emplace_back(hash, (captureOrPawnMove ? 0 : historyTable.back().movesFromLastCaptureOrPawnMove + 1));
}

void History::pop() {
    ASSERT(historyTable.size() > 1, historyTable.size());
    historyTable.pop_back();
}

void History::init(uint64_t initialPositionHash) {
    historyTable.clear();
    historyTable.emplace_back(initialPositionHash, 0);
}

bool History::topPositionThreeTimesRepeated() {
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

bool History::isTopPositionRepeated(int expectedCount) {
    ASSERT(historyTable.size() > 0, historyTable.size());
    int count = 1;
    uint64_t searchedHash = historyTable.back().hash;
    uint16_t lastMoves = historyTable.back().movesFromLastCaptureOrPawnMove;
    for (typename HistoryVector::const_reverse_iterator iter = historyTable.crbegin() + 1;
            iter != historyTable.crend() && iter->movesFromLastCaptureOrPawnMove < lastMoves && count < expectedCount; ++iter) {
        lastMoves = iter->movesFromLastCaptureOrPawnMove;
        count += uint8_t(iter->hash == searchedHash);
    }
    return count >= expectedCount;
}

void History::printHistory() {
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
