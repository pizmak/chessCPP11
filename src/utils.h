#pragma once
#include <list>
#include <string>

inline std::list<std::string> split(std::string data, char c) {
    std::list<std::string> ret;
    size_t pos;
    while ((pos = data.find_first_of(c)) != std::string::npos) {
        ret.push_back(data.substr(0, pos));
        data = data.substr(pos + 1);
    }
    if (data.length() > 0) {
        ret.push_back(data);
    }
    return ret;
}
