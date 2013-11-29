#pragma once

#include <iostream>
#include <assert.h>
#include "print.h"

#define DEBUG
#ifdef DEBUG
#define ASSERT(cond, ...) \
    if (!(cond)) { \
        std::cerr << "(" << #__VA_ARGS__ << ") = ("; \
        print(std::cerr, ##__VA_ARGS__); \
        std::cerr << ")" << std::endl; \
        assert(cond); \
    }
#else
#define ASSERT(cond, ...)
#endif
