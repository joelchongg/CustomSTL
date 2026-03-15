#pragma once

#include <cstdlib>
#include <new>
#include <utility>
#include <cstddef>
#include <type_traits>

namespace CustomSTL {

// Class for a Bump Allocator
// Used for constructing objects that are trivially destructible, such that we don't need to call the destructor
// for each object, and can simply call std::free and reset the allocator in O(1) time
class Arena {
private:
    char* start_;
    char* current_; // point to first unused byte
    char* capacity_; // point to one past capacity

public:
    explicit Arena(size_t capacity) {
        // malloc by default returns memory that is aligned to std::max_align_t
        // in order to align to a cache line size instead, we use std::aligned_alloc
        // and ensure that the alignment is for a cache line size.
        // since aligned_alloc will give nullptr if size % aligned_alloc != 0, we make sure that the capacity is a multiple of a cache line size
        // NOTE: This means that an Arena's capacity must be a multiple of a cache line size
        size_t cache_line_size = std::hardware_destructive_interference_size;
        size_t aligned_capacity = (capacity + cache_line_size - 1) & ~(cache_line_size - 1);

        start_ = static_cast<char *>(std::aligned_alloc(cache_line_size, aligned_capacity));
        if (start_ == nullptr) {
            throw std::bad_alloc();
        }

        current_ = start_;
        capacity_ = start_ + aligned_capacity;
    }

    ~Arena() {
        std::free(start_);
    }

    // copying is prohibited as we are managing raw memory
    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    // alignment is required as non aligned objects is considered UB
    // default alignment is that of std::max_align_t which specifies the alignment that is suitable for all scalar types
    void* allocate(size_t bytes, size_t alignment = alignof(std::max_align_t)) {
        // ensure that the starting allocation is on an aligned address
        size_t padding = ((reinterpret_cast<size_t>(current_) + alignment - 1) & ~(alignment - 1)) - reinterpret_cast<size_t>(current_);
        char* start_addr = current_ + padding;

        if (start_addr + bytes > capacity_) {
            throw std::bad_alloc();
        }

        current_ = start_addr + bytes;
        return start_addr;
    }

    template <typename T, typename... Args>
    requires std::is_trivially_destructible_v<T>
    T* construct(Args&&... args) {
        void* raw_mem_ptr = allocate(sizeof(T), alignof(T));
        new (raw_mem_ptr) T(std::forward<Args>(args)...);
        return static_cast<T*>(raw_mem_ptr);
    }

    void reset() noexcept {
        current_ = start_;
    }

    size_t remaining() const noexcept {
        return static_cast<size_t>(capacity_ - current_);
    }
};

}