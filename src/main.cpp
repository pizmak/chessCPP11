#include "ChessBoard.h"
#include "Move.h"
#include "notation.h"
#include "utils/bit.h"
#include "Engine.h"
#include "MoveGenerator.h"
#include "test/unitTests.h"
#include "UciProtocol.h"
#include "ChessEvaluator.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#include <vector>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cerr << std::boolalpha << std::uppercase << std::setfill('0') << std::setiosflags(std::ios::internal) ;
    MoveGenerator::initBitmasks();
    ChessEvaluator::initialize();
    UciProtocol p;
    while (true) {
        std::string command;
        std::getline(std::cin, command);
        if (command == "quit") {
            break;
        }
        if (command == "xboard") {
            std::cerr << "xboard not supported" << std::endl;
            continue;
        }
        TRACE_PARAMS(command);
        const std::string &result = p.dispatchCommand(command);
        TRACE_PARAMS(result);
        if (result.length() > 0) {
            std::cout << result << std::endl;
        }
    }
    return 0;
}
