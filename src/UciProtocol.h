#pragma once

#include <string>
#include <atomic>
#include <thread>
#include "Engine.h"

class UciProtocol {
    UciProtocol(const UciProtocol &other) = delete;
    UciProtocol(const UciProtocol &&other) = delete;
    Engine engine;
    void testArena();
    std::string dispatchCommand(std::string command);
    void setupStartPosition(std::string data);
    void setupFenPosition(std::string data);
    void setupPosition(std::string command);
    void setOption(std::string option);
    void processGo();
    void goAsync();
    void printStats(std::string pattern);
    std::thread *currentThread;
    std::atomic<bool> searchInProgress;
public:
    UciProtocol() : currentThread(0) {}
    void start();
};
