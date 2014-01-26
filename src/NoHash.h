#pragma once

#include <cstdint>

struct NoHash {
    template <typename ...IntTypes>
    void updatePiece(IntTypes ...) {
    }
    uint64_t getHash() const {
        return 0;
    }
    void setPlayer(int) {
    }
    void clearHash() {
    }
};
