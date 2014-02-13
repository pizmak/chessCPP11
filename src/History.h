#pragma once

#include <cstdint>
#include <vector>

class History {
    // TODO hashes are potentially not unique, maybe some better heuristic for positions comparison should be used
    struct HistoryElement {
        uint64_t hash;
        uint16_t movesFromLastCaptureOrPawnMove;
        HistoryElement(uint64_t hash, uint64_t movesFromLastCaptureOrPawnMove) : hash(hash), movesFromLastCaptureOrPawnMove(movesFromLastCaptureOrPawnMove) {}
    };
    using HistoryVector = std::vector<HistoryElement>;
    HistoryVector historyTable;
    bool topPositionThreeTimesRepeated() const;
public:
    bool isTopPositionRepeated(int expectedCount) const;
    bool isDraw() const;
    void push(uint64_t hash, bool captureOrPawnMove);
    void pop();
    void init(uint64_t initialPositionHash);
    void printHistory() const;
};
