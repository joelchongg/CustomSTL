#ifndef CUSTOM_STL_VECTOR_HPP
#define CUSTOM_STL_VECTOR_HPP

#include <memory>

namespace CustomSTL {

    template <typename T, class Allocator = std::allocator<T>>
    class vector {
    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = std::allocator_traits<Allocator>::pointer // See what this is for
        using const_pointer = std::allocator_traits<Allocator>::const_pointer;
        // Add in iterator aliases

    private:
        
    };
}

#endif