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
        using pointer = std::allocator_traits<Allocator>::pointer // Unused currently
        using const_pointer = std::allocator_traits<Allocator>::const_pointer; 
        // Add in iterator aliases

    private:
        T* ptr_;
        size_type size_;
        size_type capacity_;        
    
    public:
        constexpr vector() noexcept
            : ptr_ { nullptr }
            , size_ { 0 }
            , capacity_ { 0 }
        { }

        ~vector() {
            delete[] ptr_;
        }

        vector(const vector<T>& other)
            : ptr_ { new T[other.size_] }
            , size_ { other.size_ }
            , capacity { other.size_ }
        {
            if constexpr (std::is_trivially_copyable_v<T>) {
                std::memcpy(ptr_, other.ptr_, size_ * sizeof(T));
            } else {
                for (size_type i = 0; i < size_; ++i) {
                    new (&ptr_[i]) T(other.ptr_[i]);
                }
            }
        }

    };
}

#endif