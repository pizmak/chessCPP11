#include "UciProtocol.h"
#include "utils/logging.h"
#include "notation.h"
#include "utils/split.h"
#include "utils/Statistics.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <cstdlib>

bool isFile(char file) {
    return file >= 'a' && file <= 'h';
}

bool isRank(char rank) {
    return rank >= '1' && rank <= '8';
}

bool isMove(const std::string &move) {
    return move.length() == 4 && isRank(move[0]) && isFile(move[1]) && isRank(move[2]) && isFile(move[3]);
}

UciProtocol::UciProtocol() : currentThread(0) {
    init();
}

void UciProtocol::init() {
    engine.reset();
    searchInProgress.store(false);
}

std::string UciProtocol::dispatchCommand(std::string command) {
    if (command == "uci") {
        return R"(id name Elizunia
id author robal
option name Hash type spin default 1 min 1 max 700
option name alphaBetaDepth type spin default 5 min 0 max 20
option name Clear Hash type button
uciok)";
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
    if (command.find("setoption") == 0) {
        setOption(command.substr(sizeof("setoption")));
        return "";
    }
    if (command.find("go") == 0) {
        processGo();
        return "";
    }
    if (command.find("stop") == 0) {
        engine.stop();
        return "";
    }
    if (command.find("s") == 0) {
        engine.getBoard().print();
        return "";
    }
    if (command.find("printStats") == 0) {
        printStats(command.substr(sizeof("printStats") - 1));
        return "";
    }
    engine.move(command);
    processGo();
    return "";
}

void UciProtocol::processGo() {
    if (searchInProgress) {
        std::cerr << "cannot start other search when one search is in progress" << std::endl;
        return;
    } else {
        if (currentThread) {
            currentThread->join();
            delete currentThread;
        }
    }
    searchInProgress.store(true);
    currentThread = new std::thread{std::bind(&UciProtocol::goAsync, std::ref(*this))};
}

void UciProtocol::goAsync() {
    std::cerr << "in new thread: " << std::this_thread::get_id() << std::endl;
    Move m = engine.go();
    std::cout << std::string("bestmove ") + move2String(m) << std::endl;
    engine.move(m);
    searchInProgress.store(false);
}

void UciProtocol::setupStartPosition(std::string data) {
    std::list<std::string> moves = split(data, ' ');
#ifdef DEBUG
    for (auto &move : moves) {
        TRACE(move << " ");
    }
#endif
    engine.reset();
    engine.move(moves);
}

void UciProtocol::setupFenPosition(std::string data) {
    std::list<std::string> fenPosition  = split(data, ' ');
    engine.setupFenPosition(fenPosition);
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

void UciProtocol::setOption(std::string option) {
    ASSERT(option.find("name") == 0, option);
    typename std::string::size_type valuePosition = option.find("value");
    std::string optionName;
    std::string optionValue;
    if (valuePosition == std::string::npos) {
        optionName = option.substr(5);
    } else {
        optionName = option.substr(5, valuePosition - 6);
        optionValue = option.substr(valuePosition + 6);
    }
    if (optionName == "alphaBetaDepth") {
        int alphaBetaDepth = atoi(optionValue.c_str());
        engine.setAlphaBetaDepth(alphaBetaDepth);
    } else if (optionName == "Clear Hash") {
        engine.clearHash();
    } else if (optionName == "Hash") {
        std::cerr << "Hash option temporary unsupported" << std::endl;
    } else {
        std::cerr << "invalid option name: |" << optionName << "|" << std::endl;
    }
}

void UciProtocol::printStats(std::string pattern) {
    if (pattern.empty()) {
        Statistics::globalStatistics().printAllFull(std::cerr);
    } else {
        Statistics::globalStatistics().printRegexFull(pattern.substr(1), std::cerr);
    }
}
