#pragma once

#include <type_traits>
#include <string>
#include <sstream>
#include <map>
#include <cstdio>
#include "foreach.h"
#include "asserts.h"

template <typename PossibleEnumType, bool>
struct UnderlyingHelper;

template <typename EnumType>
struct UnderlyingHelper<EnumType, true> {
    using type = typename std::underlying_type<EnumType>::type;
};

template <typename NotEnumType>
struct UnderlyingHelper<NotEnumType, false> {
    using type = NotEnumType;
};

// helper struct to get underlying type of enum but not compiler error for other types, to use in SFINAE
template <typename PossibleEnumType>
struct Underlying : UnderlyingHelper<PossibleEnumType, std::is_enum<PossibleEnumType>::value> {};

// fake, only to silence eclipse complaining about templated toInt
inline int toInt(...) { ASSERT(false, "shoud not happen"); return 0; }

template <typename EnumType>
constexpr typename std::enable_if<std::is_enum<EnumType>::value, typename Underlying<EnumType>::type>::type toInt(EnumType enumVal) {
    return (typename std::underlying_type<EnumType>::type)(enumVal);
}

#define _ENUM_KEY_VAL(x, y) x = (y)
#define _ENUM_DEFINITION(name, underlying, ...) \
enum class name : underlying {\
    FOREACH2(_ENUM_KEY_VAL, COMMA, ##__VA_ARGS__) \
};\

#define _MAP_ELEM(scope, x, y) { scope::x, #x }
#define _ENUM_MAP(name, ...) \
    static std::map<name, std::string> name##Map = { \
            FOREACH2_ARG(_MAP_ELEM, name, COMMA, ##__VA_ARGS__) \
    };

#define _ENUM_TO_STRING(name, ...) \
    inline std::string toString(name v) { \
        if (name##Map.find(v) != name##Map.end()) { \
            return name##Map.at(v); \
        } \
        /* simple printf because ASSERT possibly call this function */ \
        fprintf(stderr, "invalid %s: %d\n", #name, v); \
        std::ostringstream s; \
        s << #name << "(" << toInt(v) << ")"; \
        return s.str(); \
    } \

#define _ENUM_OPERATORS(EnumType) \
    inline std::ostream &operator<<(std::ostream &stream, EnumType enumVal) { \
        return stream << toString(enumVal); \
    }

#define _ENUM_IN_RANGE(EnumType) \
    inline bool inRange(EnumType enumVal) { \
        return EnumType##Map.find(enumVal) != EnumType##Map.end(); \
    }

#define ENUM(name, underlying, ...) \
    _ENUM_DEFINITION(name, underlying, ##__VA_ARGS__) \
    _ENUM_MAP(name, ##__VA_ARGS__) \
    _ENUM_TO_STRING(name, ##__VA_ARGS__) \
    _ENUM_OPERATORS(name) \
    _ENUM_IN_RANGE(name)
