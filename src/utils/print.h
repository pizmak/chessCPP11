#pragma once

// variadic print template
template <typename ...Types> inline void print(std::ostream &stream, Types... types);

template <> inline void print(std::ostream &) {
}

template <typename T1> inline void print(std::ostream &stream, T1 val1) {
    stream << val1;
    print(stream);
}

template <typename T1, typename T2, typename ...Types> inline void print(std::ostream &stream, T1 val1, T2 val2, Types... types) {
    stream << val1 << ", ";
    print(stream, val2, types...);
}

template <typename ... Types>
void println(std::ostream &stream, Types ... vals) {
    print(stream, vals...);
    stream << std::endl;
}
