#pragma once

struct NoHash {
    template <typename ...IntTypes>
    void update(IntTypes ...intTypes) {
    }
};
