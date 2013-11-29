#include "UciProtocol.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

bool isFile(char file) {
    return file >= 'a' && file <= 'h';
}

bool isRank(char rank) {
    return rank >= '1' && rank <= '8';
}

bool isMove(const std::string &move) {
    return move.length() == 4 && isRank(move[0]) && isFile(move[1]) && isRank(move[2]) && isFile(move[3]);
}

std::vector<std::string> moves = {"a7a6", "a6a5", "a5a4"};

std::string dispatchCommand(const std::string &command) {
    static int index = 0;
    if (command == "uci") {
        return "uciok";
    }
    if (command == "isready") {
        return "readyok";
    }
    if (command == "new") {
        return "";
    }
    if (command == "position") {
        return "";
    }
    if (command.find("go") == 0) {
        return std::string("bestmove ") + moves[index++ % 3];
    }
    return "";
}

void testArena() {
    std::ofstream f(R"(c:\arenatest.txt)");
    if (!f) {
        std::cerr << "file open failure" << std::endl;
        return;
    }
    while (true) {
        std::string command;
        std::getline(std::cin, command);
        f << "in:\t" << command << std::endl;
        f.flush();
        std::string resp = dispatchCommand(command);
        if (resp.length() > 0) {
            f << "out:\t" << resp << std::endl;
            f.flush();
            std::cout << resp << std::endl;
        }
    }
    f.close();
}


void UciProtocol::start() {
    testArena();
}
