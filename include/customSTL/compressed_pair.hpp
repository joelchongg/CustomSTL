#ifndef CUSTOM_STL_COMPRESSED_PAIR_HPP
#define CUSTOM_STL_COMPRESSED_PAIR_HPP

#include <type_traits>

namespace customSTL {
    template <typename S, typename T, bool = std::is_empty<T>::value && !std::is_final<T>::value>
    class CompressedPair;

    // T is an empty class, inherit from it and leverage EBO
    template <typename S, typename T>
    class CompressedPair<S, T, true> : private T {
    private:
        S first_;

    public:
        S& first() noexcept { return first_; }
        T& second() noexcept { return *this; }
    };

    // S and T are not empty
    template <typename S, typename T>
    class CompressedPair<S, T, false> {
    private:
        S first_;
        T second_;
    
    public:
        S& first() noexcept { return first_; }
        T& second() noexcept { return second_; }
    };
}

#endif
