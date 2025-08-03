#ifndef CUSTOM_STL_UNIQUE_PTR_HPP
#define CUSTOM_STL_UNIQUE_PTR_HPP

#include <concepts>
#include <memory> // For std::default_delete<T>
#include <type_traits>
#include <utility>

namespace customSTL {

    template <typename T, typename Deleter = std::default_delete<T>>
    class unique_ptr : private Deleter {
    public:
        using element_type = T;
        using pointer = T*;

    private:
        pointer ptr_;

    public:
        constexpr unique_ptr() noexcept
            : ptr_ { nullptr }
        { }

        constexpr unique_ptr(std::nullptr_t) noexcept
            : ptr_ { nullptr }
        { }

        explicit constexpr unique_ptr(pointer ptr) noexcept
            requires std::is_default_constructible_v<Deleter> && !std::is_pointer_v<Deleter>
            : ptr_ { ptr }
        { }

        constexpr unique_ptr(pointer ptr, const Deleter& d) noexcept
            requires !std::is_reference_v<Deleter> && std::copy_constructible<Deleter>
            : Deleter { d }
            , ptr_ { ptr }
        { }

        constexpr unique_ptr(pointer ptr, Deleter&& d) noexcept
            requires !std::is_reference_v<Deleter> && std::move_constructible<Deleter>
            : Deleter { std::move(d) }
            , ptr_ { ptr }
        { }

        constexpr unique_ptr(pointer ptr, Deleter& d) noexcept
            requires std::is_lvalue_reference_v<Deleter> &&
                     !std::is_const_v<std::remove_reference_t<Deleter>>
            : Deleter { d }
            , ptr_ { ptr }
        { }
        
        constexpr unique_ptr(pointer ptr, Deleter&& d) noexcept
            requires std::is_lvalue_reference_v<Deleter>
            = delete;

        constexpr unique_ptr(pointer ptr, const Deleter& d) noexcept
            requires std::is_lvalue_reference_v<Deleter> && 
                     std::is_const_v<std::remove_reference_t<Deleter>>
            : Deleter { d }
            , ptr_ { ptr }
        { }

        constexpr unique_ptr(pointer ptr, const Deleter&& d) noexcept
            requires std::is_lvalue_reference_v<Deleter> && 
                     std::is_const_v<std::remove_reference_t<Deleter>>
            = delete;

        template <typename U, typename E>
        constexpr unique_ptr(unique_ptr<U, E>&& other) noexcept
            requires !std::same_as<unique_ptr<U, E>, unique_ptr<T, Deleter>> && 
                     std::convertible_to<unique_ptr<U,E>::pointer, pointer> &&
                     !std::is_array_v<U> &&
                     (
                        (std::is_reference_v<Deleter> && std::same_as<E, Deleter>) ||
                        (!std::is_reference_v<Deleter> && std::convertible_to<E, Deleter>)
                     )
            : Deleter { [](auto&& deleter) noexcept {
                            if constexpr (std::is_reference_v<E>) {
                                return deleter;
                            } else {
                                return std::move(deleter);
                            }
                        } (other.get_deleter())
                      }
            , ptr_ { other.release() }
        { }

        ~unique_ptr() noexcept {
            if (ptr_) {
                get_deleter()(ptr_);
            }
        }

        constexpr unique_ptr(unique_ptr&& other) noexcept 
            : Deleter { std::move(other.get_deleter()) }
            , ptr_ { std::exchange(other.ptr_, nullptr) }
        { }

        constexpr unique_ptr& operator=(unique_ptr&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            get_deleter()(ptr_);
            ptr_ = std::exchange(other.ptr_, nullptr);
            static_cast<Deleter&>(*this) = std::move(other.get_deleter());

            return *this;
        }

        // Copy Semantics Disabled
        unique_ptr(const unique_ptr& ptr) = delete;
        unique_ptr& operator=(const unique_ptr& ptr) = delete;

        constexpr T* release() noexcept {
            return std::exchange(ptr_, nullptr);
        }

        void reset(pointer newPtr = nullptr) noexcept {
            // In case user passes in the original pointer
            if (ptr_ != newPtr) {
                get_deleter()(ptr_);
                ptr_ = newPtr;
            }
        }

        constexpr void swap(unique_ptr& other) noexcept {
            std::swap(ptr_, other.ptr_);
            std::swap(get_deleter(), other.get_deleter());
        }

        constexpr pointer get() noexcept { return ptr_; }

        constexpr const pointer get() const noexcept { return ptr_; }

        constexpr Deleter& get_deleter() noexcept { return static_cast<Deleter&>(*this); }

        constexpr const Deleter& get_deleter() const noexcept { return static_cast<const Deleter&>(*this); }

        constexpr operator bool() const noexcept { return ptr_; }

        constexpr T& operator*() noexcept { return *ptr_; }

        constexpr const T& operator*() const noexcept { return *ptr_; }

        constexpr pointer operator->() noexcept { return ptr_; }

        constexpr const pointer operator->() const noexcept { return ptr_; }
    };

    // Partial template specialization for arrays
    template <typename T, typename Deleter>
    class unique_ptr<T[], Deleter> : private Deleter {
    public:
        using element_type = T;
        using pointer = T*;
    
    private:
        pointer ptr_;

    public:
        constexpr unique_ptr() noexcept
            : ptr_ { nullptr }
        { }

        constexpr unique_ptr(std::nullptr_t) noexcept
            : ptr_ { nullptr }
        { }

        constexpr unique_ptr(pointer ptr) noexcept
            requires std::is_default_constructible_v<Deleter> && !std::is_pointer_v<Deleter>
            : ptr_ { ptr }
        { }



        ~unique_ptr() noexcept {
            if (ptr_) {
                get_deleter()(ptr_);
            }
        }

        constexpr unique_ptr(unique_ptr&& other) noexcept
            : Deleter { std::move(other.get_deleter()) }
            , ptr_ { std::exchange(other.ptr_, nullptr) }
        { }

        constexpr unique_ptr& operator=(unique_ptr&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            get_deleter()(ptr_);
            ptr_ = std::exchange(other.ptr_, nullptr);
            static_cast<Deleter&>(*this) = std::move(other.get_deleter());

            return *this;
        }

        // Copy Semantics Disabled
        unique_ptr(const unique_ptr& other) = delete;
        unique_ptr& operator=(const unique_ptr& other) = delete;

        constexpr pointer release() noexcept {
            return std::exchange(ptr_, nullptr);
        }

        template <typename U>
        void reset(U newPtr) noexcept 
            requires std::same_as<U, pointer> ||
                (std::same_as<pointer, element_type*> &&
                 std::convertible_to<U(*)[], element_type(*)[]>)
        {
            if (ptr_ != newPtr) {
                get_deleter()(ptr_);
                ptr_ = newPtr;
            }
        }

        void reset(std::nullptr_t = nullptr) noexcept {
            reset(static_cast<pointer>(nullptr));
        }

        constexpr void swap(unique_ptr& other) noexcept {
            std::swap(ptr_, other.ptr_);
            std::swap(get_deleter(), other.get_deleter());
        }

        constexpr pointer get() noexcept { return ptr_; }

        constexpr const pointer get() const noexcept { return ptr_; }

        constexpr Deleter& get_deleter() noexcept { return static_cast<Deleter&>(*this); }

        constexpr const Deleter& get_deleter() const noexcept { return static_cast<const Deleter&>(*this); }

        constexpr operator bool() const noexcept { return ptr_; }

        constexpr T& operator[](std::size_t index) const { return ptr_[index]; }
    };

    template <typename T, std::size_t BufferSize = 64>
    class small_unique_ptr {
    public:
        using element_type = T;
        using pointer = T*;

    private:
        pointer ptr_;
        alignas(std::max_align_t) std::byte buffer_[BufferSize];
        bool constructed_inline_;

    public:
        constexpr small_unique_ptr() noexcept
            : ptr_ { nullptr }
            , constructed_inline_ { false }
        { }

        template <typename... Args>
        constexpr explicit small_unique_ptr(Args&&... args) noexcept {
            if (sizeof(element_type) <= BufferSize && alignof(element_type) <= alignof(std::max_align_t)) {
                new (static_cast<void*>(&buffer_)) T(std::forward<Args>(args)...);
                ptr_ = std::launder(reinterpret_cast<pointer>(&buffer_));
                constructed_inline_ = true;
            } else {
                ptr_ = new T(std::forward<Args>(args)...);
                constructed_inline_ = false;
            }
        }

        ~small_unique_ptr() noexcept {
            reset();
        }

        constexpr small_unique_ptr(small_unique_ptr&& other) noexcept {
            move_from(other);
        }
        
        constexpr small_unique_ptr& operator=(small_unique_ptr&& other) noexcept {
            if (this == &other) {
                return *this;
            }

            reset();
            move_from(other);
            
            return *this;
        }

        // Copy Semantics Disabled
        small_unique_ptr(const small_unique_ptr& other) = delete;
        small_unique_ptr& operator=(const small_unique_ptr& other) = delete;

        void reset() noexcept {
            if (ptr_) {
                if (constructed_inline_) {
                    ptr_->~T();
                } else {
                    delete ptr_;
                }
                ptr_ = nullptr;
                constructed_inline_ = false;
            }
        }

        constexpr pointer get() noexcept { return ptr_; }

        constexpr const pointer get() const noexcept { return ptr_; }
        
        constexpr pointer operator->() noexcept { return ptr_; }

        constexpr const pointer operator->() const noexcept { return ptr_; }

        constexpr T& operator*() noexcept { return *ptr_; }

        constexpr const T& operator*() const noexcept { return *ptr_; }

        constexpr explicit operator bool() const noexcept { return ptr_; }

    private:
        constexpr void move_from(small_unique_ptr& other) noexcept {
            constructed_inline_ = other.constructed_inline_;
            if (constructed_inline_) {
                new (static_cast<void*>(&buffer_)) T(std::move(other.get()));
                ptr_ = std::launder(reinterpret_cast<T*>(&buffer_));
                other.reset();
            } else {
                ptr_ = std::exchange(other.ptr_, nullptr);
            }
        }
    };
}

#endif