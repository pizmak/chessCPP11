#pragma once

#include <type_traits>
#include "bit.h"
#include "enums.h"

template <typename EnumType>
class EnumFlags {
    static_assert(std::is_enum<EnumType>::value, "EnumType should be enum");
public:
    using IntType = typename std::underlying_type<EnumType>::type;
private:
    IntType flags;
    EnumFlags(IntType flags) : flags(flags) {}
public:
    EnumFlags() = default;
    EnumFlags(EnumType enumVal) : flags(toInt(enumVal)) {}
    EnumFlags operator|(EnumType enumVal) const {
        return EnumFlags(flags | toInt(enumVal));
    }
    EnumFlags operator|(EnumFlags<EnumType> enumVal) const {
        return EnumFlags(flags | enumVal.flags);
    }
    EnumFlags operator&(EnumType enumVal) const {
        return EnumFlags(flags & toInt(enumVal));
    }
    EnumFlags operator&(EnumFlags<EnumType> enumVal) const {
        return EnumFlags(flags & enumVal.flags);
    }
    EnumFlags &operator|=(EnumType enumVal) {
        flags |= toInt(enumVal);
        return *this;
    }
    EnumFlags &operator|=(EnumFlags<EnumType> enumVal) {
        flags |= enumVal.flags;
        return *this;
    }
    EnumFlags &operator&=(EnumType enumVal) {
        flags &= toInt(enumVal);
        return *this;
    }
    EnumFlags &operator&=(EnumFlags<EnumType> enumVal) {
        flags &= enumVal.flags;
        return *this;
    }
    EnumFlags operator~() const {
        return EnumFlags(~flags);
    }
    explicit operator IntType() const {
        return flags;
    }
    explicit operator bool() const {
        return flags != 0;
    }
    bool operator==(const EnumFlags<EnumType> &other) const {
        return flags == other.flags;
    }
    bool operator!=(const EnumFlags<EnumType> &other) const {
        return !operator==(other);
    }
};

// this is simpler than specifying full type of return value in explicit cast
template <typename EnumType>
inline auto toInt(EnumFlags<EnumType> flags) -> typename EnumFlags<EnumType>::IntType {
    return (typename EnumFlags<EnumType>::IntType)(flags);
}

template <typename EnumType>
typename std::enable_if<std::is_enum<EnumType>::value, EnumFlags<EnumType>>::type operator|(EnumType e1, EnumType e2) {
    return EnumFlags<EnumType>(e1) | e2;
}

template <typename EnumType>
typename std::enable_if<std::is_enum<EnumType>::value, EnumFlags<EnumType>>::type operator|(EnumType e1, EnumFlags<EnumType> e2) {
    return e2 | e1;
}

template <typename EnumType>
typename std::enable_if<std::is_enum<EnumType>::value, EnumFlags<EnumType>>::type operator&(EnumType e1, EnumType e2) {
    return EnumFlags<EnumType>(e1) & e2;
}

template <typename EnumType>
typename std::enable_if<std::is_enum<EnumType>::value, EnumFlags<EnumType>>::type operator&(EnumType e1, EnumFlags<EnumType> e2) {
    return e2 & e1;
}

template <typename EnumType>
auto operator<<(std::ostream &stream, EnumFlags<EnumType> flags) -> decltype(toString(EnumType()), stream) {
    typename std::underlying_type<EnumType>::type enumVal = flags;
    if (enumVal == 0) {
        stream << EnumType(0);
    } else {
        uint_fast8_t pos = bit::leastSignificantBit(enumVal);
        stream << EnumType(1 << pos);
        bit::unset(enumVal, pos);
        while (enumVal != 0) {
            pos = bit::leastSignificantBit(enumVal);
            stream << "|" << EnumType(1 << pos);
            bit::unset(enumVal, pos);
        }
    }
    return stream;
}
