#ifndef CUSTOM_STL_VECTOR_HPP
#define CUSTOM_STL_VECTOR_HPP

#include <memory>

namespace CustomSTL {

    template <typename T>
    class vector {
    public:
        using value_type = T;
        using pointer = T*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;

    private:
        pointer first_;
        pointer last_;
        pointer endOfStorage_; // pointer to one past allocated block

    public:
        constexpr vector() noexcept
            : first_ { nullptr }
            , last_ { nullptr }
            , endOfStorage_ { nullptr }
        { }

        ~vector() noexcept {
            delete[] first_;
            first_ = last_ = endOfStorage_ = nullptr;
        }

        vector(const vector& other)
            : first_ { new T[other.size()] }
            , endOfStorage_ { first_ + other.size() }
        {
            size_t sz = other.size();
            last_ = sz ? first_ + sz - 1 : first_;

            for (size_t i = 0; i < sz; ++i) {
                first_[i] = other[i];
            }
        }

        vector(vector&& other)
            : first_ { std::exchange(other.first_, nullptr) }
            , last_ { std::exchange(other.last_, nullptr) }
            , endOfStorage_ { std::exchange(other.endOfStorage_, nullptr) }
        { }

        vector& operator=(const vector& other) {
            if (this == &other) {
                return *this;
            }

            if (other.size() > capacity()) {
                size_t sz = other.size();
                std::unique_ptr<T[]> tmp(new T[sz]);
                std::uninitialized_copy(other.begin(), other.end(), tmp.get());
                
                delete[] first_;

                first_ = tmp.release();
                last_ = first_ + sz - 1;
                endOfStorage_ = first + sz;
            } else {
                std::copy(other.begin(), other.end(), first_);

                // destroy excess elements
                size_t old_size = size();
                for (size_t i = other.size(); i < old_size; ++i) {
                    first_[i].~T();
                }

                last_ = first_ + other.size() - 1;
            }

            return *this;
        }

        vector& operator=(vector&& other) {
            if (this == &other) {
                return *this;
            }

            delete[] first_;

            first_ = std::exchange(other.first_, nullptr);
            last_ = std::exchange(other.last_, nullptr);
            endOfStorage_ = std::exchange(other.endOfStorage_, nullptr);

            return *this;
        }
    };
}

#endif