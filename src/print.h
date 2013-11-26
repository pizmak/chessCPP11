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

template <typename ... Types>
struct Print1;

template <typename T, typename ... Types>
struct Print1<T, Types...> : Print1<Types...> {
    Print1<T, Types...>(T t, Types ... vals) : Print1<Types ...>(vals...), t(t) {}
    T t;
    void print(std::ostream &stream) {
        stream << t << ',';
        Print1<Types...>::print(stream);
    }
};

template <>
struct Print1<> {
    void print(std::ostream &) {}
};

template <typename ... Types>
struct Print2;

template <typename ... Types>
struct Print2 : Print2<Types>... {
    Print2<Types...>(Types ... vals) : Print2<Types >(vals)... {}
    void print(std::ostream &stream) {
        std::initializer_list<int> {(Print2<Types>::print(stream), 1)...};
    }
};

template <>
struct Print2<> {
    void print(std::ostream &) { }
};

template <typename T>
struct Print2<T> {
    T t;
    Print2(T t) : t(t) {}
    void print(std::ostream &stream) { stream << t << ','; }
};

// variadic println template
template <typename ...Types> inline void println1(std::ostream &stream, Types... vals) {
    Print1<Types...> p(vals...);
    p.print(stream);
    stream << std::endl;
}

// variadic println template
template <typename ...Types> inline void println2(std::ostream &stream, Types... vals) {
    Print2<Types...> p(vals...);
    p.print(stream);
    stream << std::endl;
}
