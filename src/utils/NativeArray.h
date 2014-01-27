#pragma once

#include <initializer_list>

template <typename Type, int SizeOne, int...AdditionalSizes>
struct NativeArray;

template <typename Type, int SizeOne>
struct NativeArray<Type, SizeOne> {
    using type = Type[SizeOne];
    template <typename Initializer> static void initMulti(Initializer &initializer, type array) {
        for (int i = 0; i < SizeOne; ++i) {
            array[i] = initializer();
        }
    }
    static Type get(type array, int i) {
        return array[i];
    }
};


template <typename Type, int SizeOne, int ...AdditionalSizes>
struct NativeArray : public NativeArray<Type, AdditionalSizes...> {
    using OneLessDimmArray = NativeArray<Type, AdditionalSizes...>;
    using type = typename OneLessDimmArray::type[SizeOne];
    template <typename Initializer> static void initMulti(Initializer &initializer, type array) {
        for (int i = 0; i < SizeOne; ++i) {
            OneLessDimmArray::initMulti(initializer, array[i]);
        }
    }
    template <typename ...IntTypes>
    static inline Type get(type array, int index, IntTypes... indices) {
        static_assert(sizeof...(AdditionalSizes) == sizeof...(indices), "");
        return OneLessDimmArray::get(array[index], indices...);
    }
};
