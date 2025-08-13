#ifndef CUSTOM_STL_WEAK_PTR_HPP
#define CUSTOM_STL_WEAK_PTR_HPP

#include "shared_ptr.hpp"

namespace CustomSTL {
    
    template <typename T>
    class weak_ptr {
    private:
        CustomSTL::ControlBlock* ptr_;

    public:
        constexpr weak_ptr() noexcept
            : ptr_ { nullptr }
        { }

        constexpr weak_ptr(const weak_ptr& other) noexcept
            : ptr_ { other.ptr_ }
        { 
            if (ptr_ != nullptr) {
                ptr_->add_weak();
            }
        }

        constexpr weak_ptr(weak_ptr&& other) noexcept
            : ptr_ { std::exchange(other.ptr_, nullptr) }
        { }

        ~weak_ptr() noexcept {
            if (ptr_) {
                ptr_->release_weak();
            }
        }

        constexpr weak_ptr& operator=(const weak_ptr& other) noexcept {
            if (&other == this) {

            }
        }



    };
}

#endif