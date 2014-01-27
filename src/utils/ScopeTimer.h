#pragma once

#include <chrono>
#include <iostream>

class ScopeTimer {
    std::chrono::steady_clock::time_point startPoint;
    std::string text;
public:
    ScopeTimer(const std::string text = "") : startPoint(std::chrono::steady_clock::now()), text(text) {}
    ~ScopeTimer() {
        std::chrono::steady_clock::duration duration = std::chrono::steady_clock::now() - startPoint;
        std::cerr << text << " took: " << (double)duration.count() * std::chrono::steady_clock::duration::period::num / std::chrono::steady_clock::duration::period::den << " seconds" << std::endl;
    }
};
