#include "Board.h"
#include "Move.h"
#include "notation.h"
#include <iostream>
#include <iomanip>
#include <fstream>

#include "bit.h"
#include <list>
#include <vector>

#include "Engine.h"

#include "unitTests.h"
#include "UciProtocol.h"

int main() {
    std::cerr << std::hex << std::boolalpha << std::uppercase << std::setfill('0') << std::setiosflags(std::ios::internal);
    Engine::initBitmasks();
//    engineMasksTest();
    testEngine();
//    testArena();

    return 0;
}
