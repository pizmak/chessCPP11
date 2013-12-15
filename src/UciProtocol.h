#pragma once

#include <string>

#include "Engine.h"

class UciProtocol {
    Engine engine;
    void testArena();
    std::string dispatchCommand(std::string command);
    void setupStartPosition(std::string data);
    void setupFenPosition(std::string data);
    void setupPosition(std::string command);
    void setOption(std::string option);
public:
    void start();
};
