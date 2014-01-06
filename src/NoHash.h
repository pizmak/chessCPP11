#pragma once

#include <cstdint>

struct NoHash {
    template <typename ...IntTypes>
    void update(IntTypes ...) {
    }
    uint64_t getHash() const {
        return 0;
    }
    void switchPlayer() {
    }
    void clearHash() {
    }
};
