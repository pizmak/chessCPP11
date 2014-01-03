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
    std::ios_base::sync_with_stdio(false);
    std::cerr << std::boolalpha << std::uppercase << std::setfill('0') << std::setiosflags(std::ios::internal) ;
    Engine::initBitmasks();
//    nativeArrayTest();
    //    engineMasksTest();
//    testEngine();
//    testArena();
//    hashUnitTest();
//    return 0;
    while (true) {
        std::string command;
        std::getline(std::cin, command);
        if (command == "uci") {
            UciProtocol p;
            p.start();
        }
    }
    return 0;
}
