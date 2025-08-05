#ifndef CUSTOM_STL_SHARED_PTR_HPP
#define CUSTOM_STL_SHARED_PTR_HPP

#include <atomic>

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
        explicit ControlBlock(pointer ptr)
            : ptr_ { ptr }
            , strong_count_ { 1 }
            , weak_count_ { 0 }
        { }

        ~ControlBlock() {
            delete ptr_;
        }

        pointer get() { return ptr_; }

        void add_strong() { ++strong_count_; }
        void release_strong() { --strong_count_; }
        unsigned long get_strong_count() { return strong_count_.load(); }

        void add_weak() { ++weak_count_; }
        void release_weak() { -- weak_count_; }
        unsigned long get_weak_count() { return weak_count_.load(); }
    };

    template <typename T>
    class shared_ptr {
    public:
        using element_type = T;
        using pointer = T*;
        
    private:
        pointer ptr_;
        ControlBlock<T>* control_ptr_;
    
    public:
        
    };
}

#endif