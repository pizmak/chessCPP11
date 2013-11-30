#include "UciProtocol.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "notation.h"
#include "utils.h"

bool isFile(char file) {
    return file >= 'a' && file <= 'h';
}

bool isRank(char rank) {
    return rank >= '1' && rank <= '8';
}

bool isMove(const std::string &move) {
    return move.length() == 4 && isRank(move[0]) && isFile(move[1]) && isRank(move[2]) && isFile(move[3]);
}

std::string UciProtocol::dispatchCommand(std::string command) {
    if (command == "uci") {
        return "uciok";
    }
    if (command == "isready") {
        return "readyok";
    }
    if (command == "new") {
        return "";
    }
    if (command == "ucinewgame") {
        return "";
    }
    if (command.find("position") == 0) {
        setupPosition(command.substr(sizeof("position")));
        return "";
    }
    if (command.find("go") == 0) {
        Move m = engine.go();
        std::ofstream f(R"(c:\arenatest2.txt)");
        if (!f) {
            std::cerr << "file open failure" << std::endl;
            return "";
        }
        f << "out:\t" << m << std::endl;
        f.flush();
        f.close();
        return std::string("bestmove ") + move2String(m);
    }
    return "";
}

void UciProtocol::testArena() {
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

void UciProtocol::setupStartPosition(std::string data) {
    std::list<std::string> moves = split(data, ' ');
    for (auto &move : moves) {
        std::cerr << move << " ";
    }
    engine.reset();
    engine.move(moves);
}

void UciProtocol::setupFenPosition(std::string data) {
    std::list<std::string> fenPosition  = split(data, ' ');
    engine.setupFenPosition(fenPosition);
}

void UciProtocol::start() {
    engine.reset();
    testArena();
}

void UciProtocol::setupPosition(std::string command) {
    engine.reset();
    size_t movesPosition = command.find("moves");
    std::string movesString;
    if (movesPosition != std::string::npos) {
        movesString = command.substr(movesPosition + sizeof("moves"));
        command = command.substr(0, movesPosition);
    }
    if (command.find("startpos") == 0) {
        command = command.substr(std::min(sizeof("startpos"), command.length()));
    } else if (command.find("fen") == 0) {
        command = command.substr(sizeof("fen"));
        setupFenPosition(command);
    }
    std::list<std::string> moves = split(movesString, ' ');
    engine.move(moves);
}
