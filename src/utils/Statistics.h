#pragma once

#include <iosfwd>
#include <map>
#include <vector>
#include <string>

class Statistics {
    struct StatsElement {
        uint64_t full;
        std::vector<int> partial;
        StatsElement();
    };
    using MapType = std::map<std::string, StatsElement>;
    MapType stats;
public:
    void increment(const std::string &key);
    void checkpoint(const std::string &key);
    void checkpointAll();
    void printSimple(const std::string &key, std::ostream &stream);
    void printRegexSimple(const std::string &key, std::ostream &stream);
    void printAllSimple(std::ostream &stream);
    void printFull(const std::string &key, std::ostream &stream);
    void printRegexFull(const std::string &key, std::ostream &stream);
    void printAllFull(std::ostream &stream);
    void reset();
    static Statistics &globalStatistics();
};
