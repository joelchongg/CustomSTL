#ifndef CUSTOM_STL_SHARED_PTR_HPP
#define CUSTOM_STL_SHARED_PTR_HPP

#include <atomic>
#include <concepts>
#include <type_traits>

namespace CustomSTL {
    template <typename T>
    class ControlBlock {
    public:
        using pointer = T*;
        using count_size = std::atomic<unsigned long>;

    private:
        pointer ptr_;
        count_size strong_count_;
        count_size weak_count_;

    public:
        explicit ControlBlock(pointer ptr) noexcept
            : ptr_ { ptr }
            , strong_count_ { 1 }
            , weak_count_ { 0 }
        { }

        ~ControlBlock() = default;

        pointer get() const noexcept { return ptr_; }

        void add_strong() { ++strong_count_; }
        void release_strong() { 
            --strong_count_;

            if (strong_count_ == 0) {
                delete ptr_;
                ptr_ = nullptr;

                if (weak_count_ == 0) {
                    delete this;
                }
            }
        }
        unsigned long get_strong_count() const noexcept { return strong_count_.load(); }

        void add_weak() { ++weak_count_; }
        void release_weak() { 
            --weak_count_;
            if (weak_count_ == 0 && strong_count_ == 0) {
                delete ptr_;
                ptr_ = nullptr;
                delete this;
            }
        }
        unsigned long get_weak_count() const noexcept { return weak_count_.load(); }
    };

    template <typename Y, typename T>
    concept PointerCompatible = std::convertible_to<Y*, T*> ||
                                (std::is_array_v<Y> &&
                                 std::is_unbounded_array_v<T> &&
                                 std::is_same_v<std::remove_cv_t<std::remove_extent_t<Y>>,
                                                std::remove_cv_t<std::remove_extent_t<T>>
                                                >
                                );

    template <typename T>
    class shared_ptr {
    public:
        using element_type = T;
        using pointer = T*;

    private:
        pointer ptr_;
        ControlBlock<T>* control_ptr_;
    
    public:
        constexpr shared_ptr() noexcept
            : ptr_ { nullptr }
            , control_ptr_ { nullptr }
        { }

        constexpr shared_ptr(std::nullptr_t) noexcept
            : ptr_ { nullptr }
            , control_ptr_ { nullptr }
        { }

        explicit shared_ptr(pointer ptr)
            : ptr_ { ptr }
            , control_ptr_ { new ControlBlock(ptr) }
        { }

        ~shared_ptr() noexcept {
            release();
        }

        shared_ptr(const shared_ptr& other) noexcept
            : ptr_ { other.ptr_ }
            , control_ptr_ { other.control_ptr_ }
        {
            if (control_ptr_) {
                control_ptr_->add_strong();
            }
        }

        shared_ptr(shared_ptr&& other) noexcept
            : ptr_ { std::exchange(other.ptr_, nullptr) }
            , control_ptr_ { std::exchange(other.control_ptr_, nullptr) }
        { }

        shared_ptr& operator=(const shared_ptr& other) noexcept {
            if (this == &other) {
                return *this;
            }

            if (control_ptr_) {
                control_ptr_->release_strong();
            }

            ptr_ = other.ptr_;
            control_ptr_ = other.control_ptr_;
            if (control_ptr_) {
                control_ptr_->add_strong();
            }

            return *this;
        }

        shared_ptr& operator=(shared_ptr&& other) noexcept {
            if (this == &other) {
                return *this;
            }

            if (control_ptr_) {
                control_ptr_->release_strong();
            }

            ptr_ = std::exchange(other.ptr_, nullptr);
            control_ptr_ = std::exchange(other.control_ptr_, nullptr);

            return *this;
        }

        void reset(pointer ptr = nullptr) noexcept {
            release();
            if (ptr) {
                ptr_ = ptr;
                control_ptr_ = new ControlBlock<T>(ptr);
            }
        }

        void swap(shared_ptr& other) noexcept {
            std::swap(ptr_, other.ptr_);
            std::swap(control_ptr_, other.control_ptr_);
        }

        unsigned long long use_count() const noexcept { return control_ptr_ ? control_ptr_->get_strong_count() : 0; }

        constexpr bool owner_before(const shared_ptr& other) const noexcept { return control_ptr_ < other.control_ptr_; }

        constexpr pointer get() noexcept { return ptr_; }
        constexpr const pointer get() const noexcept { return ptr_; }

        constexpr T& operator*() noexcept { return *ptr_; }
        constexpr const T& operator*() const noexcept { return *ptr_; }
        
        constexpr pointer operator->() noexcept { return ptr_; }
        constexpr const pointer operator->() const noexcept { return ptr_; }

        constexpr explicit operator bool() const noexcept { return ptr_; }

    private:
        void release() {
            if (control_ptr_) {
                control_ptr_->release_strong();
            }
            ptr_ = nullptr;
            control_ptr_ = nullptr;
        }
    };

    template <typename T, typename U>
    constexpr bool operator==(const CustomSTL::shared_ptr<T> lhs, const CustomSTL::shared_ptr<U> rhs) noexcept {
        return lhs.get() == rhs.get();
    }

    template <typename T, typename... Args>
    shared_ptr<T> make_shared(Args&&... args) {
       // TODO
       return nullptr; 
    }
} 

#endif