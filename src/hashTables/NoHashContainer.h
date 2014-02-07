#pragma once

template <typename HashValue>
struct NoHashContainer {
    void insert(uint64_t, const HashValue &) {
    }
    HashValue &get(uint64_t) {
        static HashValue empty;
        return empty;
    }
    void clear() {
    }
};
