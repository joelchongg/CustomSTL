#pragma once

#include <stdlib.h>

namespace CustomSTL {

template <typename T>
class vector {

public:
    using size_type = size_t;

    explicit vector(size_t capacity = 0)
        : data_ { nullptr }, last_ { nullptr }, end_ { nullptr } {
        if (capacity > 0) {
            // allocate raw bytes instead of calling constructor so that we only
            // initialize what we need
            data_ = static_cast<T*>(std::malloc(capacity * sizeof(T)));
        
            if (!data_) throw std::bad_alloc();

            last_ = data_;
            end_ = data_ + capacity;
        }
    }

    ~vector() {
        clear();
        std::free(data_);
    }

    void push_back(const T& value) {
        if (last_ == end_) {
            reallocate();
        }

        new (last_) T(value);
        ++last_;
    }

    void push_back(T&& value) {
        if (last_ == end_) {
            reallocate();
        }

        new (last_) T(std::move(value));
        ++last_;
    }

    void pop_back() {
        if (data_ == last_) [[unlikely]] {
            throw std::logic_error("vector is empty");
        }

        --last_;
        last_->~T();
    }

    void clear() {
        while (last_ != data_) {
            --last_;
            last_->~T();
        }
    }

    ptrdiff_t size() {
        return last_ - data_;
    }

    ptrdiff_t capacity() {
        return end_ - data_; 
    }

private:

    void reallocate() {
        ptrdiff_t old_capacity = last_ - data_;
        ptrdiff_t new_capacity = old_capacity == 0 ? 1 : old_capacity << 1; // exponential increment for reallocation

        if constexpr (std::is_trivially_copyable_v<T>) {
            // realloc may reallocate a different memory location if it cant extend
            // realloc does a memcpy so we should ideally only do it when T is POD
            T* new_data = static_cast<T*>(std::realloc(data_, new_capacity * sizeof(T)));
            if (!new_data) throw std::bad_alloc();

            data_ = new_data;
            last_ = data_ + old_capacity;
            end_ = data_ + new_capacity;
        } else {
            T* new_data = static_cast<T*>(std::malloc(new_capacity * sizeof(T)));
            if (!new_data) throw std::bad_alloc();

            for (size_t i = 0; i < old_capacity; ++i) {
                new (new_data + i) T(std::move_if_noexcept(data_[i]));
                data_[i].~T();
            }

            std::free(data_);

            data_ = new_data;
            last_ = data_ + old_capacity;
            end_ = data_ + new_capacity;
        }
    }

    T* data_;
    T* last_;
    T* end_;

};

} // namespace CustomSTL