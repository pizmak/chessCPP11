#include "Statistics.h"
#include <ostream>
#include <regex>

Statistics::StatsElement::StatsElement() : full(0) {
    partial.push_back(0);
}

void Statistics::increment(const std::string& key) {
    ++stats[key].full;
    ++stats[key].partial.back();
}

void Statistics::checkpoint(const std::string& key) {
    stats[key].partial.push_back(0);
}

void Statistics::checkpointAll() {
    for (auto &stat : stats) {
        stat.second.partial.push_back(0);
    }
}

void Statistics::printSimple(const std::string& key, std::ostream &stream) {
    stream << key << ":\t\t" << stats[key].full << "\t(" << stats[key].partial.back() << ")" << std::endl;
}

void Statistics::printRegexSimple(const std::string& pattern, std::ostream &stream) {
    for (auto &stat : stats) {
        if (std::regex_match(stat.first, std::regex(pattern))) {
            printSimple(stat.first, stream);
        }
    }
}

void Statistics::printAllSimple(std::ostream &stream) {
    for (auto &stat : stats) {
        printSimple(stat.first, stream);
    }
}

void Statistics::printFull(const std::string& key, std::ostream &stream) {
    stream << key << ":\t\t" << stats[key].full << "\t(";
    const char *sep = "";
    const char *commaSep = ",\t";
    for (auto &stat : stats[key].partial) {
        stream << sep << stat;
        sep = commaSep;
    }
    stream << ")" << std::endl;
}

void Statistics::printRegexFull(const std::string& pattern, std::ostream &stream) {
    for (auto &stat : stats) {
        if (std::regex_match(stat.first, std::regex(pattern))) {
            printFull(stat.first, stream);
        }
    }
}

void Statistics::printAllFull(std::ostream &stream) {
    for (auto &stat : stats) {
        printFull(stat.first, stream);
    }
}

void Statistics::reset() {
    stats.clear();
}

Statistics &Statistics::globalStatistics() {
    static Statistics global;
    return global;
}
