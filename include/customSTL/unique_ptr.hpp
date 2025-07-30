#ifndef CUSTOM_STL_UNIQUE_PTR_HPP
#define CUSTOM_STL_UNIQUE_PTR_HPP

namespace customSTL {
    template <typename T>
    class unique_ptr {
    private:
        T* ptr_;

    public:
        unique_ptr() noexcept
            : ptr_ { nullptr }
        { }

        unique_ptr(T* ptr) noexcept
            : ptr_ { ptr }
        { }

        ~unique_ptr() noexcept {
            delete ptr_;
        }

        unique_ptr(unique_ptr&& other) noexcept
            : ptr_ { other.ptr_ }
        {
            other.ptr_ = nullptr;
        }

        unique_ptr& operator=(unique_ptr&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            delete ptr_;
            ptr_ = std::exchange(other.ptr_, nullptr);

            return *this;
        }

        // Copy Capabilities Disabled
        unique_ptr(const unique_ptr& ptr) = delete;
        unique_ptr& operator=(const unique_ptr& ptr) = delete;

        T* release() noexcept {
            return std::exchange(ptr_, nullptr);
        }

        void reset() noexcept {
            delete ptr_;
            ptr_ = nullptr;
        }

        void swap(unique_ptr& other) noexcept {
            std::swap(ptr_, other.ptr_);
        }

        T* get() noexcept { return ptr_; }

        operator bool() const noexcept { return ptr_; }

        T& operator*() const noexcept { return *ptr_; }

        T* operator->() const noexcept { return ptr_; }
    };
}

#endif