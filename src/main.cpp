#include "ChessBoard.h"
#include "Move.h"
#include "notation.h"
#include "bit.h"
#include "Engine.h"
#include "MoveGenerator.h"
#include "unitTests.h"
#include "UciProtocol.h"
#include "ScorePosition.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#include <vector>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cerr << std::boolalpha << std::uppercase << std::setfill('0') << std::setiosflags(std::ios::internal) ;
    MoveGenerator::initBitmasks();
    ScorePosition::initialize();
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
